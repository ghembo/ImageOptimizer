#ifndef ImageProcessor_h__
#define ImageProcessor_h__

#include "Logger.h"

namespace cv
{
	class Mat;
}

namespace ImageSimilarity
{
	class Similarity;
}

namespace sim = ImageSimilarity;

class OptimizationSequence;

class  ImageProcessor
{
public:
	ImageProcessor();
	unsigned int OptimizeImage(const cv::Mat& image);
	
private:
	static OptimizationSequence searchBestQuality(const cv::Mat& image, sim::Similarity targetSsim);
	static sim::Similarity computeSsim(const cv::Mat& image, unsigned int quality);

	static unsigned int getNextQuality(unsigned int minQuality, unsigned int maxQuality);
	static std::pair<unsigned int, unsigned int> getNextQualityRange(unsigned int quality, sim::Similarity currentSsim, sim::Similarity targetSsim, unsigned  int minQuality, unsigned  int maxQuality);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger m_logger;
};

#endif // ImageProcessor_h__