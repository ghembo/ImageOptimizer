#include "ImageProcessorImplementation.h"

#include "Logger.h"
#include "ImageSimilarity.h"

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
	m_logger.Log("Created");
}

ImageProcessorImplementation::~ImageProcessorImplementation()
{
	m_logger.Log("Destroyed");
}

void ImageProcessorImplementation::OptimizeImage( const std::string& imagePath )
{
 	m_logger.Log("Loading image");
 
 	path p(imagePath);
 
	if (!exists(p))
	{
		throw std::invalid_argument("File doesn't exist");
	}

 	if(!is_regular_file(p))
 	{
		throw std::invalid_argument("Path is not a file");
 	}

	auto referenceImage = cv::imread(imagePath, CV_LOAD_IMAGE_GRAYSCALE);

	if (referenceImage.data == NULL)
	{
		throw std::invalid_argument("Image format not supported");
	}

	if (!referenceImage.isContinuous())
	{
		throw std::invalid_argument("Cannot efficiently process input image");
	}

	optimizeImage(referenceImage);
}

void ImageProcessorImplementation::optimizeImage(const cv::Mat& referenceImage)
{
	constexpr float targetSsim = 0.999f;
	constexpr unsigned int maxNumberOfIterations = 10;
	constexpr unsigned int startQuality = 70;

	std::chrono::milliseconds millliseconds(0);

	std::vector<std::pair<unsigned int, float>> qualities;

	unsigned int minQuality = 0;
	unsigned int maxQuality = 100;

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < maxNumberOfIterations; i++)
	{
		auto currentQuality = getNextQuality(minQuality, maxQuality);

		std::vector<uchar> buffer = memoryEncodeJpeg(referenceImage, currentQuality);
		
		auto compressedImage = memoryDecodeGrayscaleJpeg(buffer);

		assert(compressedImage.data != NULL);
		assert(compressedImage.isContinuous());

		float ssim = ImageSimilarity::ssim(referenceImage, compressedImage);

		qualities.push_back(std::make_pair(currentQuality, ssim));

		if (ssim > targetSsim)
		{
			maxQuality = currentQuality;
		}
		else
		{
			minQuality = currentQuality;
		}

		if (getNextQuality(minQuality, maxQuality) == currentQuality)
		{
			break;
		}
	}

	auto finish = std::chrono::steady_clock::now();
	millliseconds += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

	auto numberOfIterations = qualities.size();

	std::cout << millliseconds.count() << "ms - " << numberOfIterations << " iterations" << std::endl;

	std::cout.precision(std::numeric_limits<float>::max_digits10);

	for each (auto quality in qualities)
	{
		std::cout << quality.first << " - " << quality.second << std::endl;
	}
}

std::vector<uchar> ImageProcessorImplementation::memoryEncodeJpeg(const cv::Mat& image, unsigned int quality)
{
	std::vector<int> parameters;
	parameters.push_back(CV_IMWRITE_JPEG_QUALITY);
	parameters.push_back(quality);

	std::vector<uchar> buffer;

	cv::imencode(".jpg", image, buffer, parameters);

	return buffer;
}

cv::Mat ImageProcessorImplementation::memoryDecodeGrayscaleJpeg(const std::vector<uchar>& buffer)
{
	return cv::imdecode(buffer, CV_LOAD_IMAGE_GRAYSCALE);
}

unsigned int ImageProcessorImplementation::getNextQuality(unsigned int minQuality, unsigned int maxQuality)
{
	return (minQuality + maxQuality) / 2;
}