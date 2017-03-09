#include "ImageOptimizerImplementation.h"

#include "Logger.h"
#include "JpegEncoderDecoder.h"

#include "opencv2\core\core.hpp"

#include <boost/filesystem.hpp>

#include <string>

using namespace boost::filesystem;



ImageOptimizerImplementation::ImageOptimizerImplementation():
	m_logger("ImgOpt")
{
}

std::string getNewFilename(const std::string& filename)
{
	path p(filename);

	path newFilename = p.parent_path() / path(p.stem().string() + "_compressed" + p.extension().string());

	return newFilename.string();
}

void ImageOptimizerImplementation::OptimizeImage( const std::string& imagePath )
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
	
	auto bestQuality = m_imageProcessor.OptimizeImage(image);

	image.release();

	image = JpegEncoderDecoder::LoadColorImage(imagePath);

	JpegEncoderDecoder::SaveJpeg(image, getNewFilename(imagePath), bestQuality);
}

cv::Mat ImageOptimizerImplementation::loadImage(const std::string& imagePath)
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

void ImageOptimizerImplementation::handleInvalidArgument(const char* message)
{
	m_logger.Log(message);
	throw std::invalid_argument(message);
}
