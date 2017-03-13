#include "ImageOptimizer.h"

#include "ImageOptimizerImplementation.h"

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

void ImageOptimizer::OptimizeImage( const std::string& imagePath )
{
	m_implementation->OptimizeImage(imagePath);
}

void ImageOptimizer::OptimizeFolder(const std::string& folderPath)
{
	m_implementation->OptimizeFolder(folderPath);
}

void ImageOptimizer::OptimizeFolderRecursive(const std::string& folderPath)
{
	m_implementation->OptimizeFolderRecursive(folderPath);
}

void ImageOptimizer::EnableFileLogging( SeverityLevel minimumSeverity /*= trace*/, const std::string& component /*= ""*/ )
{
	Logger::EnableFileLogging(minimumSeverity, component);
}

void ImageOptimizer::SetMinimumLoggingLevel(SeverityLevel minimumSeverity)
{
	Logger::SetMinimumLoggingLevel(minimumSeverity);
}
