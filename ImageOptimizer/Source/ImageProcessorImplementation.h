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
	void optimizeImage(const cv::Mat& referenceImage);

	std::vector<uchar> memoryEncodeJpeg(const cv::Mat & image, unsigned int quality);
	cv::Mat memoryDecodeGrayscaleJpeg(const std::vector<uchar>& buffer);

	unsigned int getNextQuality(unsigned int minQuality, unsigned int maxQuality);

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__