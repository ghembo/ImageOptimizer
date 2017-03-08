#include "JpegEncoderDecoder.h"

#include "opencv2\opencv.hpp"



std::vector<uchar> JpegEncoderDecoder::MemoryEncodeJpeg(const cv::Mat& image, unsigned int quality)
{
	std::vector<int> parameters;
	parameters.push_back(CV_IMWRITE_JPEG_QUALITY);
	parameters.push_back(quality);

	std::vector<uchar> buffer;

	auto success = cv::imencode(".jpg", image, buffer, parameters);

	assert(success);

	return buffer;
}

cv::Mat JpegEncoderDecoder::MemoryDecodeGrayscaleJpeg(const std::vector<uchar>& buffer)
{
	return cv::imdecode(buffer, CV_LOAD_IMAGE_GRAYSCALE);
}