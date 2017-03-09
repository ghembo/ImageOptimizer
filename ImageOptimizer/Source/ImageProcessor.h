#ifndef ImageProcessor_h__
#define ImageProcessor_h__

#include "Logger.h"

namespace cv
{
	class Mat;
}

class OptimizationSequence;

class  ImageProcessor
{
public:
	ImageProcessor();
	unsigned int OptimizeImage(const cv::Mat& image);
	
private:
	OptimizationSequence searchBestQuality(const cv::Mat& image, float targetSsim);
	float computeSsim(const cv::Mat& image, unsigned int quality);

	unsigned int getNextQuality(unsigned int minQuality, unsigned int maxQuality);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger m_logger;
};

#endif // ImageProcessor_h__