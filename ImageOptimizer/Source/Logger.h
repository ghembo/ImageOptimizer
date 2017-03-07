#ifndef Logger_h__
#define Logger_h__

#include "Severity.h"

#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <string>

class Logger
{
public:
	Logger();
	Logger(const char* channel);

	void Log(const char* message);
	void Log(SeverityLevel severity, const char* message);

	static void EnableFileLogging(SeverityLevel minimumSeverity, const std::string& component);
	static void DisableFileLogging();

private:
	void TryInitialize();
	void Initialize();

	using text_file_sink_t = boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend >;

	static bool s_initialized;
	static boost::shared_ptr<text_file_sink_t> s_textFileSink;

	boost::log::sources::severity_channel_logger<SeverityLevel, std::string> m_logger;
};

#endif