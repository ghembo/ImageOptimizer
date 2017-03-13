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

unsigned int ImageProcessor::OptimizeImage(const cv::Mat& image)
{
	constexpr float targetSsim = 0.999f;

	auto start = std::chrono::steady_clock::now();

	auto qualities = searchBestQuality(image, targetSsim);

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	logDurationAndResults(duration, qualities);

	return qualities.BestQuality();
}

OptimizationSequence ImageProcessor::searchBestQuality(const cv::Mat& image, float targetSsim)
{
	unsigned int minQuality = 0;
	unsigned int maxQuality = 100;

	OptimizationSequence qualities;

	auto quality = getNextQuality(minQuality, maxQuality);

	while (!qualities.HasBeenTried(quality))
	{
		auto ssim = computeSsim(image, quality);

		qualities.AddOptimizationResult(quality, ssim);

		std::tie(minQuality, maxQuality) = getNextQualityRange(quality, ssim, targetSsim, minQuality, maxQuality);

		quality = getNextQuality(minQuality, maxQuality);
	}

	return qualities;
}

float ImageProcessor::computeSsim(const cv::Mat& image, unsigned int quality)
{
	std::vector<uchar> buffer = JpegEncoderDecoder::MemoryEncodeJpeg(image, quality);

	auto compressedImage = JpegEncoderDecoder::MemoryDecodeGrayscaleJpeg(buffer);

	assert(compressedImage.data != NULL);
	assert(compressedImage.isContinuous());

	return ImageSimilarity::ComputeSsim(image, compressedImage);
}

std::pair<unsigned int, unsigned int> ImageProcessor::getNextQualityRange(unsigned int quality, float currentSsim, float targetSsim, unsigned  int minQuality, unsigned  int maxQuality)
{
	return (currentSsim > targetSsim) ? std::make_pair(minQuality, quality) : std::make_pair(quality, maxQuality);
}

unsigned int ImageProcessor::getNextQuality(unsigned int minQuality, unsigned int maxQuality)
{
	return (minQuality + maxQuality) / 2;
}

void ImageProcessor::logDurationAndResults(long long duration, const OptimizationSequence& results)
{
	std::ostringstream message;

	message << duration << "ms - " << results.NumberOfIterations() << " iterations - Best quality: " << results.BestQuality() << std::endl;

	message.precision(std::numeric_limits<float>::max_digits10);

	for (auto result : results)
	{
		message << result.first << " - " << result.second << std::endl;
	}

	m_logger.Log(message.str().c_str());
}
