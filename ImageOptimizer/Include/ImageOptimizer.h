#ifndef ImageOptimizer_h__
#define ImageOptimizer_h__

#include <string>
#include <memory>

#include "Severity.h"
#include "OptimizationResult.h"


using traceCallback_t = void(*)(const char*);
using warningCallback_t = void(*)(const char*);
using errorCallback_t = void(*)(const char*);

class ImageOptimizerImplementation;

class ImageOptimizer
{
public:
	ImageOptimizer();
	~ImageOptimizer();
	
	static std::string GetVersion();

	void SetLogCallbacks(traceCallback_t traceCallback, warningCallback_t warningCallback, errorCallback_t errorCallback);

	OptimizationResult OptimizeImage(const std::string& imagePath, float similarity = 0.9999f);
	OptimizationResult OptimizeFolder(const std::string& folderPath, float similarity = 0.9999f);
	OptimizationResult OptimizeFolderRecursive(const std::string& imageFolderPath, float similarity = 0.9999f);

private:
	std::unique_ptr<ImageOptimizerImplementation> m_implementation;

	static const std::string s_version;
};

#endif // ImageOptimizer_h__