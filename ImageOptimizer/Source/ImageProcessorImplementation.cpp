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

std::string getNewFilename(const std::string& filename)
{
	path p(filename);

	path newFilename = p.parent_path() / path(p.stem().string() + "_compressed" + p.extension().string());

	return newFilename.string();
}

void ImageProcessorImplementation::OptimizeImage( const std::string& imagePath )
{
 	m_logger.Log(imagePath.data());

	auto image = loadImage(imagePath);

	if (image.data == NULL)
	{
		handleInvalidArgument("Image format not supported");
	}

	if (!image.isContinuous())
	{
		handleInvalidArgument("Cannot efficiently process input image");
	}	
	
	auto bestQuality = optimizeImage(image);

	image.release();

	image = JpegEncoderDecoder::LoadColorImage(imagePath);

	JpegEncoderDecoder::SaveJpeg(image, getNewFilename(imagePath), bestQuality);
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

	return JpegEncoderDecoder::LoadGrayscaleImage(imagePath);
}

unsigned int ImageProcessorImplementation::optimizeImage(const cv::Mat& image)
{
	constexpr float targetSsim = 0.999f;

	auto start = std::chrono::steady_clock::now();

	auto qualities = searchBestQuality(image, targetSsim);

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	logDurationAndResults(duration, qualities);

	return qualities.BestQuality();
}

OptimizationSequence ImageProcessorImplementation::searchBestQuality(const cv::Mat& image, float targetSsim)
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
	std::ostringstream message;

	message << duration << "ms - " << results.NumberOfIterations() << " iterations" << std::endl;

	message.precision(std::numeric_limits<float>::max_digits10);

	for (auto result : results)
	{
		message << result.first << " - " << result.second << std::endl;
	}

	m_logger.Log(message.str().c_str());
}
