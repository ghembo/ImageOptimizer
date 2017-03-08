#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"

#include "opencv2/core/hal/interface.h"

#include <string>
#include <vector>

namespace cv
{
	class Mat;
}

class  ImageProcessorImplementation
{
public:
	ImageProcessorImplementation();
	~ImageProcessorImplementation();

	void OptimizeImage(const std::string& imagePath);
	
private:
	void optimizeImage(const cv::Mat& image);
	std::vector<std::pair<unsigned int, float>> computeBestQuality(const cv::Mat& image, float targetSsim);
	float computeSsim(const cv::Mat& image, unsigned int quality);

	unsigned int getNextQuality(unsigned int minQuality, unsigned int maxQuality);

	void handleInvalidArgument(const char* message);

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__