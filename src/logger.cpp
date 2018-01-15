#include "iopt/logger.hpp"

#include <string>

void Logger::setCallbacks(traceCallback_t traceCallback, warningCallback_t warningCallback, errorCallback_t errorCallback)
{
	m_traceCallback = traceCallback;
	m_warningCallback = warningCallback;
	m_errorCallback = errorCallback;
}

void Logger::trace(const char* message)
{
	if (m_traceCallback != nullptr)
	{
		m_traceCallback(message);
	}
}

void Logger::warning(const char* message)
{
	if (m_warningCallback != nullptr)
	{
		m_warningCallback(message);
	}
}

void Logger::error(const char* message)
{
	if (m_errorCallback != nullptr)
	{
		m_errorCallback(message);
	}
}

void Logger::trace(const std::string & message)
{
	trace(message.c_str());
}

void Logger::warning(const std::string & message)
{
	warning(message.c_str());
}

void Logger::error(const std::string & message)
{
	error(message.c_str());
}
