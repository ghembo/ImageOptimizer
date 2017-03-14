#include "ImageOptimizerImplementation.h"

#include "Logger.h"
#include "JpegEncoderDecoder.h"

#include "opencv2\core\core.hpp"

#include <boost/filesystem.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/filtered.hpp>

#include <string>
#include <regex>

using namespace boost::filesystem;



ImageOptimizerImplementation::ImageOptimizerImplementation():
	m_logger("ImgOpt")
{
}

auto ImageOptimizerImplementation::getJpegInFolder(const std::string& imageFolderPath)
{
	std::vector<std::string> filenames;

	boost::transform(boost::make_iterator_range(directory_iterator(path(imageFolderPath)), {}) | boost::adaptors::filtered(isJpegFile),
		back_inserter(filenames), [](const auto& file) {return file.path().string(); });

	return filenames;
}

auto ImageOptimizerImplementation::getAllFoldersInFolder(const std::string& folderPath)
{
	std::vector<std::string> folders;

	boost::transform(boost::make_iterator_range(recursive_directory_iterator(path(folderPath)), {}) | boost::adaptors::filtered([](const auto& entry) {return is_directory(entry); }),
		back_inserter(folders), [](const auto& file) {return file.path().string(); });

	return folders;
}

void ImageOptimizerImplementation::OptimizeFolder(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity)
{
	m_logger.Log(imageFolderPath.data());

	validateFolderPath(imageFolderPath);

	auto filenames = getJpegInFolder(imageFolderPath);

	for (const auto& filename : filenames)
	{
		OptimizeImage(filename, similarity);
	}
}

void ImageOptimizerImplementation::OptimizeFolderRecursive(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity)
{
	m_logger.Log(imageFolderPath.data());

	OptimizeFolder(imageFolderPath, similarity);

	auto folders = getAllFoldersInFolder(imageFolderPath);

	for (const auto& folder : folders)
	{
		OptimizeFolder(folder, similarity);
	}
}

void ImageOptimizerImplementation::OptimizeImage( const std::string& imagePath, ImageSimilarity::Similarity similarity)
{
 	m_logger.Log(imagePath.data());

	auto image = loadImage(imagePath);

	validateImage(image);

	m_logger.Log("Target ssim: " + std::to_string(similarity.GetValue()));
	
	auto bestQuality = m_imageProcessor.OptimizeImage(image, similarity);

	image.release();

	image = JpegEncoderDecoder::LoadColorImage(imagePath);

	auto newFileName(getNewFilename(imagePath));

	JpegEncoderDecoder::SaveJpeg(image, newFileName, bestQuality);

	auto originalFileSize = file_size(path(imagePath));
	auto newFileSize = file_size(path(newFileName));

	auto compression = (newFileSize * 100) / originalFileSize;

	m_logger.Log("Original size: ", originalFileSize, " New size: ", newFileSize, " Compression: ", compression);
}

cv::Mat ImageOptimizerImplementation::loadImage(const std::string& imagePath)
{
	validateImagePath(imagePath);

	return JpegEncoderDecoder::LoadGrayscaleImage(imagePath);
}

void ImageOptimizerImplementation::validateFolderPath(const std::string& imageFolderPath)
{
	path p(imageFolderPath);

	if (!exists(p))
	{
		handleInvalidArgument("Folder doesn't exist");
	}

	if (!is_directory(p))
	{
		handleInvalidArgument("Path is not a folder");
	}
}

void ImageOptimizerImplementation::validateImagePath(const std::string& imagePath)
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
}

void ImageOptimizerImplementation::validateImage(const cv::Mat& image)
{
	if (image.data == NULL)
	{
		handleInvalidArgument("Image format not supported");
	}

	if (!image.isContinuous())
	{
		handleInvalidArgument("Cannot efficiently process input image");
	}
}

void ImageOptimizerImplementation::handleInvalidArgument(const char* message)
{
	m_logger.Log(message);
	throw std::invalid_argument(message);
}

std::string ImageOptimizerImplementation::getNewFilename(const std::string& filename)
{
	path p(filename);

	path newFilename = p.parent_path() / path(p.stem().string() + "_compressed" + p.extension().string());

	return newFilename.string();
}

bool ImageOptimizerImplementation::isJpegFile(const directory_entry& file)
{
	if (is_regular_file(file))
	{
		return false;
	}

	const std::regex jpegExtension(R"(^\.jpe?g$)", std::regex_constants::icase);

	return std::regex_match(file.path().extension().string(), jpegExtension);
}
