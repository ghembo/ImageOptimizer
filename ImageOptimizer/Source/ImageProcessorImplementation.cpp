#include "ImageProcessorImplementation.h"

#include "Logger.h"
#include "ImageSimilarity.h"
#include "JpegEncoderDecoder.h"
#include "OptimizationSequence.h"

#include "opencv2\opencv.hpp"
#include "opencv2\core\core.hpp"

#include <boost/filesystem.hpp>
#include <chrono>

#include <cassert>
#include <string>
#include <utility>

using namespace boost::filesystem;



ImageProcessorImplementation::ImageProcessorImplementation():
	m_logger("ImgProc")
{
}

ImageProcessorImplementation::~ImageProcessorImplementation()
{
}

void ImageProcessorImplementation::OptimizeImage( const std::string& imagePath )
{
 	m_logger.Log(imagePath.data());

	auto referenceImage = loadImage(imagePath);

	if (referenceImage.data == NULL)
	{
		handleInvalidArgument("Image format not supported");
	}

	if (!referenceImage.isContinuous())
	{
		handleInvalidArgument("Cannot efficiently process input image");
	}

	optimizeImage(referenceImage);
}

cv::Mat ImageProcessorImplementation::loadImage(const std::string& imagePath)
{
	path p(imagePath);

	if (!exists(p))
	{
		handleInvalidArgument("File doesn't exist");
	}

	if (!is_regular_file(p))
	{
		handleInvalidArgument("Path is not a file");
	}

	return JpegEncoderDecoder::LoadImage(imagePath);
}

void ImageProcessorImplementation::optimizeImage(const cv::Mat& image)
{
	constexpr float targetSsim = 0.999f;

	auto start = std::chrono::steady_clock::now();

	auto qualities = computeBestQuality(image, targetSsim);

	for (size_t i = 0; i < 10; i++)
	{
		computeBestQuality(image, targetSsim);
	}

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	logDurationAndResults(duration, qualities);
}

OptimizationSequence ImageProcessorImplementation::computeBestQuality(const cv::Mat& image, float targetSsim)
{
	constexpr unsigned int maxNumberOfIterations = 10;

	unsigned int minQuality = 0;
	unsigned int maxQuality = 100;
	unsigned int currentQuality = 70;

	OptimizationSequence qualities;

	for (int i = 0; i < maxNumberOfIterations; i++)
	{
		auto ssim = computeSsim(image, currentQuality);

		qualities.AddOptimizationResult(currentQuality, ssim);

		if (ssim > targetSsim)
		{
			maxQuality = currentQuality;
		}
		else
		{
			minQuality = currentQuality;
		}

		auto nextQuality = getNextQuality(minQuality, maxQuality);

		if (nextQuality == currentQuality)
		{
			break;
		}

		currentQuality = nextQuality;
	}

	return qualities;
}

float ImageProcessorImplementation::computeSsim(const cv::Mat& image, unsigned int quality)
{
	std::vector<uchar> buffer = JpegEncoderDecoder::MemoryEncodeJpeg(image, quality);

	auto compressedImage = JpegEncoderDecoder::MemoryDecodeGrayscaleJpeg(buffer);

	assert(compressedImage.data != NULL);
	assert(compressedImage.isContinuous());

	return ImageSimilarity::ComputeSsim(image, compressedImage);
}

unsigned int ImageProcessorImplementation::getNextQuality(unsigned int minQuality, unsigned int maxQuality)
{
	return (minQuality + maxQuality) / 2;
}

void ImageProcessorImplementation::handleInvalidArgument(const char* message)
{
	m_logger.Log(message);
	throw std::invalid_argument(message);
}

void ImageProcessorImplementation::logDurationAndResults(long long duration, const OptimizationSequence& results)
{
	std::cout << duration << "ms - " << results.GetNumberOfIterations() << " iterations" << std::endl;

	std::cout.precision(std::numeric_limits<float>::max_digits10);

	for (auto result : results)
	{
		std::cout << result.first << " - " << result.second << std::endl;
	}
}
