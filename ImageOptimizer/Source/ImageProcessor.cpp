#include "ImageProcessor.h"

#include "Logger.h"
#include "ImageSimilarity.h"
#include "JpegEncoderDecoder.h"
#include "OptimizationSequence.h"

#include "opencv2\core\core.hpp"

#include <chrono>

#include <cassert>



ImageProcessor::ImageProcessor():
	m_logger("ImgProc")
{
}

Quality ImageProcessor::OptimizeImage(const cv::Mat& image, sim::Similarity targetSimilarity)
{
	auto start = std::chrono::steady_clock::now();

	auto qualities = searchBestQuality(image, targetSimilarity);

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	logDurationAndResults(duration, qualities);

	return qualities.BestQuality();
}

OptimizationSequence ImageProcessor::searchBestQuality(const cv::Mat& image, sim::Similarity targetSsim)
{
	QualityRange qualityRange{ 0, 100 };

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

ImageSimilarity::Similarity ImageProcessor::computeSsim(const cv::Mat& image, Quality quality)
{
	std::vector<uchar> buffer = JpegEncoderDecoder::MemoryEncodeJpeg(image, quality);

	auto compressedImage = JpegEncoderDecoder::MemoryDecodeGrayscaleJpeg(buffer);

	assert(compressedImage.data != NULL);
	assert(compressedImage.isContinuous());

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

	m_logger.Log(message.str().c_str());
}
