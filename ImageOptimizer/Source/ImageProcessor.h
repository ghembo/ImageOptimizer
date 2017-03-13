#ifndef ImageProcessor_h__
#define ImageProcessor_h__

#include "Logger.h"
#include "Quality.h"

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
	Quality OptimizeImage(const cv::Mat& image, sim::Similarity targetSimilarity);
	
private:
	static OptimizationSequence searchBestQuality(const cv::Mat& image, sim::Similarity targetSsim);
	static sim::Similarity computeSsim(const cv::Mat& image, Quality quality);

	static Quality getNextQuality(QualityRange qualityRange);
	static QualityRange getNextQualityRange(Quality quality, sim::Similarity currentSsim, sim::Similarity targetSsim, QualityRange currentRange);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger m_logger;
};

#endif // ImageProcessor_h__