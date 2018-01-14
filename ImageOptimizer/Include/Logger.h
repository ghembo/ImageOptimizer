#pragma once

#include <string>

using traceCallback_t = void(*)(const char*);
using warningCallback_t = void(*)(const char*);
using errorCallback_t = void(*)(const char*);

class Logger
{
public:
	void setCallbacks(traceCallback_t traceCallback, warningCallback_t warningCallback, errorCallback_t errorCallback);

	void trace(const char* message);
	void warning(const char* message);
	void error(const char* message);

	void trace(const std::string& message);
	void warning(const std::string& message);
	void error(const std::string& message);

private:
	traceCallback_t m_traceCallback = nullptr;
	warningCallback_t m_warningCallback = nullptr;
	errorCallback_t m_errorCallback = nullptr;
};
