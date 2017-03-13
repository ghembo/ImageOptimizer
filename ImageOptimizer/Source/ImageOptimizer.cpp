#include "ImageOptimizer.h"

#include "ImageOptimizerImplementation.h"
#include "ImageSimilarity.h"

#include "opencv2/opencv.hpp"

#define VERSION "0.0.0.0_OPENCV" CV_VERSION

const std::string ImageOptimizer::s_version = VERSION;

std::string ImageOptimizer::GetVersion()
{
	return s_version;
}

ImageOptimizer::ImageOptimizer() :
	m_implementation(new ImageOptimizerImplementation())
{
}

ImageOptimizer::~ImageOptimizer()
{	
}

void ImageOptimizer::OptimizeImage( const std::string& imagePath, float similarity /*= 0.999*/)
{
	m_implementation->OptimizeImage(imagePath, ImageSimilarity::Similarity{ similarity });
}

void ImageOptimizer::OptimizeFolder(const std::string& folderPath, float similarity /*= 0.999*/)
{
	m_implementation->OptimizeFolder(folderPath, ImageSimilarity::Similarity{ similarity });
}

void ImageOptimizer::OptimizeFolderRecursive(const std::string& folderPath, float similarity /*= 0.999*/)
{
	m_implementation->OptimizeFolderRecursive(folderPath, ImageSimilarity::Similarity{ similarity });
}

void ImageOptimizer::EnableFileLogging( SeverityLevel minimumSeverity /*= trace*/, const std::string& component /*= ""*/ )
{
	Logger::EnableFileLogging(minimumSeverity, component);
}

void ImageOptimizer::SetMinimumLoggingLevel(SeverityLevel minimumSeverity)
{
	Logger::SetMinimumLoggingLevel(minimumSeverity);
}
