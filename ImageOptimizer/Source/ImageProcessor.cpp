#include "ImageProcessor.h"

#include "ImageOptimizer.h"

#include "opencv2/opencv.hpp"

#define VERSION "0.0.0.0_OPENCV" CV_VERSION

const std::string ImageProcessor::s_version = VERSION;

std::string ImageProcessor::GetVersion()
{
	return s_version;
}

ImageProcessor::ImageProcessor() :
	m_implementation(new ImageOptimizer())
{
}

ImageProcessor::~ImageProcessor()
{	
}

void ImageProcessor::OptimizeImage( const std::string& imagePath )
{
	m_implementation->OptimizeImage(imagePath);
}

void ImageProcessor::EnableFileLogging( SeverityLevel minimumSeverity /*= trace*/, const std::string& component /*= ""*/ )
{
	Logger::EnableFileLogging(minimumSeverity, component);
}

void ImageProcessor::SetMinimumLoggingLevel(SeverityLevel minimumSeverity)
{
	Logger::SetMinimumLoggingLevel(minimumSeverity);
}
