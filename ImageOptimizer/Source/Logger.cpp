#include "Logger.h"

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <string>
#include <iostream>
#include <fstream>

using namespace boost::log;

bool Logger::s_initialized = false;
boost::shared_ptr< Logger::text_file_sink_t > Logger::s_textFileSink;

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SeverityLevel)

static const auto FORMAT = expressions::format("[%1%] <%2%> %3%: %4%\n")
						% expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S")
						% severity
						% expressions::attr< std::string >("Channel")
						% expressions::smessage;

static const auto CONSOLE_FORMAT = expressions::format("%1%: %2%\n")
									% expressions::attr< std::string >("Channel")
									% expressions::smessage;

// Formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& stream, SeverityLevel severity)
{
	stream << Severity::GetSeverityLevelName(severity);

	return stream;
}

Logger::Logger() :
	m_logger(keywords::channel = "Global")
{
	TryInitialize();
}

Logger::Logger( const char* channel ) :
	m_logger(keywords::channel = channel)
{
	TryInitialize();
}

void Logger::Log( const char* message )
{
	BOOST_LOG(m_logger) << message;
}

void Logger::Log( SeverityLevel severity, const char* message )
{
	BOOST_LOG_SEV(m_logger, severity) << message;
}

void Logger::TryInitialize()
{
	if (!s_initialized)
	{
		s_initialized = true;

		Initialize();
	}
}

void Logger::Initialize()
{
	register_simple_formatter_factory< SeverityLevel, char >("Severity");

	boost::shared_ptr< core > core = core::get();

	core->add_global_attribute("TimeStamp", attributes::local_clock());
	
	typedef sinks::synchronous_sink< sinks::debug_output_backend > sink_t;

	boost::shared_ptr< sink_t > sink(new sink_t());

	sink->set_filter(expressions::is_debugger_present());

	sink->set_formatter(FORMAT);

	core->add_sink(sink);

	add_console_log(std::cout, keywords::format= CONSOLE_FORMAT);
}

void Logger::EnableFileLogging(SeverityLevel minimumSeverity, const std::string& component)
{
	DisableFileLogging();
	
	const char* logPattern = "ImageProcessorLog_%3N.txt";

	boost::shared_ptr< sinks::text_file_backend > backend =
		boost::make_shared< sinks::text_file_backend >(
			keywords::file_name = logPattern,
			keywords::rotation_size = 5 * 1024 * 1024);

	const char* logFolder = "logs";

	backend->set_file_collector(sinks::file::make_collector(keywords::target = logFolder));

	backend->scan_for_files();

	s_textFileSink.reset(new text_file_sink_t(backend));

	s_textFileSink->set_formatter(FORMAT);

	if (component.empty())
	{
		s_textFileSink->set_filter(severity >= minimumSeverity);
	}
	else
	{
		s_textFileSink->set_filter(
			severity >= minimumSeverity &&
			expressions::attr< std::string >("Channel") == component);
	}
	
	core::get()->add_sink(s_textFileSink);
}

void Logger::DisableFileLogging()
{
	core::get()->remove_sink(s_textFileSink);

	if(s_textFileSink)
	{
		s_textFileSink->flush();
	}

	s_textFileSink.reset();
}

void Logger::SetMinimumLoggingLevel(SeverityLevel minimumSeverity)
{
	core::get()->set_filter(severity >= minimumSeverity);
}
