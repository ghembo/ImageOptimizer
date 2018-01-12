#ifndef Logger_h__
#define Logger_h__

#include "Severity.h"

#include <string>

class Logger
{
public:
	Logger();
	Logger(const char* channel);

	template<typename T> void Log(const T& message) { /*BOOST_LOG(m_logger) << message;*/ };
	template<typename T> void Log(SeverityLevel severity, const T& message) { /*BOOST_LOG_SEV(m_logger, severity) << message;*/ };

	static void EnableFileLogging(SeverityLevel minimumSeverity, const std::string& component);
	static void DisableFileLogging();
	static void SetMinimumLoggingLevel(SeverityLevel minimumSeverity);

private:
	void TryInitialize();
	void Initialize();

	//using text_file_sink_t = boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend >;

	static bool s_initialized;
	//static boost::shared_ptr<text_file_sink_t> s_textFileSink;

	//boost::log::sources::severity_channel_logger<SeverityLevel, std::string> m_logger;
};

#endif