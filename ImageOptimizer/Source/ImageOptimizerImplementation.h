#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"
#include "ImageProcessor.h"

#include <string>

namespace cv
{
	class Mat;
}

class OptimizationSequence;

class  ImageOptimizerImplementation
{
public:
	ImageOptimizerImplementation();

	void OptimizeImage(const std::string& imagePath);
	
private:
	cv::Mat loadImage(const std::string& imagePath);

	void handleInvalidArgument(const char* message);

	ImageProcessor m_imageProcessor;

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__