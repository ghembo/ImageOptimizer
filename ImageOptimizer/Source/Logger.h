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

	template<typename T> void Log(const T& message) { BOOST_LOG(m_logger) << message; };
	template<typename First, typename... Rest> void Log(const First& first, const Rest&... rest) { Log(first); Log(rest...); };
	template<typename T> void Log(SeverityLevel severity, const T& message) { BOOST_LOG_SEV(m_logger, severity) << message...; };
	template<typename First, typename... Rest> void Log(SeverityLevel severity, const First& first, const Rest&... rest) { Log(severity, first); Log(severity, rest...); };

	static void EnableFileLogging(SeverityLevel minimumSeverity, const std::string& component);
	static void DisableFileLogging();
	static void SetMinimumLoggingLevel(SeverityLevel minimumSeverity);

private:
	void TryInitialize();
	void Initialize();

	using text_file_sink_t = boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend >;

	static bool s_initialized;
	static boost::shared_ptr<text_file_sink_t> s_textFileSink;

	boost::log::sources::severity_channel_logger<SeverityLevel, std::string> m_logger;
};

#endif