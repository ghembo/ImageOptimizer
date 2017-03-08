#ifndef JpegEncoderDecoder_h__
#define JpegEncoderDecoder_h__

#include "opencv2/core/hal/interface.h"

#include <vector>



namespace cv
{
	class Mat;
}

class JpegEncoderDecoder
{
public:
	static std::vector<uchar> MemoryEncodeJpeg(const cv::Mat & image, unsigned int quality);
	static cv::Mat MemoryDecodeGrayscaleJpeg(const std::vector<uchar>& buffer);
};

#endif // JpegEncoderDecoder_h__