#pragma once

#include "iopt/logger.hpp"
#include "quality.hpp"

namespace ImageSimilarity
{
	class Similarity;
}

namespace sim = ImageSimilarity;

class OptimizationSequence;
struct Image;

class  ImageProcessor
{
public:
	ImageProcessor(Logger& logger);
	Quality OptimizeImage(const Image& image, sim::Similarity targetSimilarity);
	
private:
	static OptimizationSequence searchBestQuality(const Image& image, sim::Similarity targetSsim);
	static sim::Similarity computeSsim(const Image& image, Quality quality);

	static Quality getNextQuality(QualityRange qualityRange);
	static QualityRange getNextQualityRange(Quality quality, sim::Similarity currentSsim, sim::Similarity targetSsim, QualityRange currentRange);

	void logDurationAndResults(long long duration, const OptimizationSequence& results);

	Logger& m_logger;
};
