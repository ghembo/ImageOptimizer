#pragma once

#include "opencv2/core/hal/interface.h"

#include <vector>
#include <string>



namespace cv
{
	class Mat;
}

class JpegEncoderDecoder
{
public:
	static cv::Mat LoadColorImage(const std::string& imagePath);
	static cv::Mat LoadGrayscaleImage(const std::string& imagePath);

	static std::vector<uchar> MemoryEncodeJpeg(const cv::Mat & image, unsigned int quality);
	static cv::Mat MemoryDecodeGrayscaleJpeg(const std::vector<uchar>& buffer);

	static void SaveJpeg(const cv::Mat & image, const std::string& filename, unsigned int quality);
};
