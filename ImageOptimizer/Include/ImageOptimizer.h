#ifndef ImageOptimizer_h__
#define ImageOptimizer_h__

#include <string>
#include <memory>

#include "Severity.h"

class ImageOptimizerImplementation;


class ImageOptimizer
{
public:
	ImageOptimizer();
	~ImageOptimizer();
	
	static void EnableFileLogging(SeverityLevel minimumSeverity = SeverityLevel::trace, const std::string& component = "");
	static void SetMinimumLoggingLevel(SeverityLevel minimumSeverity);

	static std::string GetVersion();

	void OptimizeImage(const std::string& imagePath, float similarity = 0.9999f);
	void OptimizeFolder(const std::string& folderPath, float similarity = 0.9999f);
	void OptimizeFolderRecursive(const std::string& imageFolderPath, float similarity = 0.9999f);

private:
	std::unique_ptr<ImageOptimizerImplementation> m_implementation;

	static const std::string s_version;
};

#endif // ImageOptimizer_h__