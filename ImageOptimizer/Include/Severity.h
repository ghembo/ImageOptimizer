#ifndef Severity_h__
#define Severity_h__

#include <string>
#include <array>

enum class SeverityLevel
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

class Severity
{
public:
	static const std::string& GetSeverityLevelName(SeverityLevel severity);

	static SeverityLevel GetSeverityLevel(const std::string& severityName);

	static bool IsValid(const std::string& severityName);

private:
	using severity_array_t = std::array<std::string, 6>;
	using array_size_t = severity_array_t::size_type;

	static const severity_array_t s_severityNames;
	static const SeverityLevel s_defaultSeverityLevel;
	static const std::string& s_defaultSeverityLevelName;
};

#endif