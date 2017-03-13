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
	using Quality = unsigned int;

	ImageProcessor();
	Quality OptimizeImage(const cv::Mat& image);
	
private:
	static OptimizationSequence searchBestQuality(const cv::Mat& image, sim::Similarity targetSsim);
	static sim::Similarity computeSsim(const cv::Mat& image, Quality quality);

	static Quality getNextQuality(Quality minQuality, Quality maxQuality);
	static std::pair<Quality, Quality> getNextQualityRange(Quality quality, sim::Similarity currentSsim, sim::Similarity targetSsim, Quality minQuality, Quality maxQuality);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger m_logger;
};

#endif // ImageProcessor_h__