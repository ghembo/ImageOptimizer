#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"

#include "opencv2/core/hal/interface.h"

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
	~ImageOptimizerImplementation();

	void OptimizeImage(const std::string& imagePath);
	
private:
	cv::Mat loadImage(const std::string& imagePath);
	unsigned int optimizeImage(const cv::Mat& image);
	OptimizationSequence searchBestQuality(const cv::Mat& image, float targetSsim);
	float computeSsim(const cv::Mat& image, unsigned int quality);

	unsigned int getNextQuality(unsigned int minQuality, unsigned int maxQuality);

	void handleInvalidArgument(const char* message);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__