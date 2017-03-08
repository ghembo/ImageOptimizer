#ifndef ImageProcessor_h__
#define ImageProcessor_h__

#include <string>
#include <memory>

#include "Severity.h"

class ImageOptimizerImplementation;


class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();
	
	static void EnableFileLogging(SeverityLevel minimumSeverity = SeverityLevel::trace, const std::string& component = "");
	static void SetMinimumLoggingLevel(SeverityLevel minimumSeverity);

	static std::string GetVersion();

	void OptimizeImage(const std::string& imagePath);

private:
	std::unique_ptr<ImageOptimizerImplementation> m_implementation;

	static const std::string s_version;
};

#endif // ImageProcessor_h__