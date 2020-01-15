#include "image_processor.hpp"

#include "iopt/logger.hpp"
#include "iopt/image_similarity.hpp"
#include "jpeg.hpp"
#include "optimization_sequence.hpp"

#include "iopt/image.hpp"

#include <chrono>
#include <cassert>
#include <sstream>


ImageProcessor::ImageProcessor(Logger& logger) :
	m_logger(logger)
{
}

Quality ImageProcessor::OptimizeImage(const Image& image, sim::Similarity targetSimilarity)
{
	auto start = std::chrono::steady_clock::now();

	auto qualities = searchBestQuality(image, targetSimilarity);

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	logDurationAndResults(duration, qualities);

	return qualities.BestQuality();
}

OptimizationSequence ImageProcessor::searchBestQuality(const Image& image, sim::Similarity targetSsim)
{
	QualityRange qualityRange{ 50, 100 };

	OptimizationSequence qualities{ targetSsim };

	auto quality = getNextQuality(qualityRange);

	while (!qualities.HasBeenTried(quality))
	{
		auto ssim = computeSsim(image, quality);

		qualities.AddOptimizationResult(quality, ssim);

		qualityRange = getNextQualityRange(quality, ssim, targetSsim, qualityRange);

		quality = getNextQuality(qualityRange);
	}

	return qualities;
}

ImageSimilarity::Similarity ImageProcessor::computeSsim(const Image& image, Quality quality)
{
	std::vector<uint8_t> buffer = jpeg::memory_encode_grayscale(image, quality);

	auto compressedImage = jpeg::memory_decode_grayscale(buffer);

	assert(compressedImage.data.size());

	return ImageSimilarity::ComputeSsim(image, compressedImage);
}

Quality ImageProcessor::getNextQuality(QualityRange qualityRange)
{
	return (qualityRange.GetMinimum() + qualityRange.GetMaximum()) / 2;
}

QualityRange ImageProcessor::getNextQualityRange(Quality quality, sim::Similarity currentSsim, sim::Similarity targetSsim, QualityRange qualityRange)
{
	return (currentSsim > targetSsim) ? QualityRange{ qualityRange.GetMinimum(), quality } : QualityRange{ quality, qualityRange.GetMaximum() };
}

void ImageProcessor::logDurationAndResults(long long duration, const OptimizationSequence& results)
{
	std::ostringstream message;

	message << duration << "ms - " << results.NumberOfIterations() << " iterations - Best quality: " << results.BestQuality() << std::endl;	

	for (auto result : results)
	{
		message << result.first << " - " << result.second << std::endl;
	}

	m_logger.trace(message.str());
}
