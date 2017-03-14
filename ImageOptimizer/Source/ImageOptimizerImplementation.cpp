#include "ImageOptimizerImplementation.h"

#include "Logger.h"
#include "JpegEncoderDecoder.h"

#include "opencv2\core\core.hpp"

#include <boost/filesystem.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/filtered.hpp>

#include <string>
#include <regex>
#include <thread>

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

	optimizeImages(filenames, similarity);
}

void ImageOptimizerImplementation::OptimizeFolderRecursive(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity)
{
	std::vector<std::string> filenames{ getJpegInFolder(imageFolderPath) };

	auto folders = getAllFoldersInFolder(imageFolderPath);

	for (const auto& folder : folders)
	{
		auto images{ getJpegInFolder(folder) };
		filenames.insert(filenames.end(), images.begin(), images.end()); // v1.insert(v1.end(), make_move_iterator(v2.begin()), make_move_iterator(v2.end()));
	}

	optimizeImages(filenames, similarity);
}

void ImageOptimizerImplementation::optimizeImages(const std::vector<std::string>& filenames, ImageSimilarity::Similarity similarity)
{
	auto nthreads = std::thread::hardware_concurrency();
	auto nfiles = filenames.size();

	std::vector<std::thread> threads(nthreads);

	int imagesPerThread = nfiles / nthreads;

	for (size_t t = 0; t < nthreads; t++)
	{
		size_t first = imagesPerThread * t;
		size_t last = imagesPerThread * (t + 1);

		threads[t] = std::thread([this, similarity, &filenames, first, last]()
		{
			for (size_t i = first; i < last; i++)
			{
				OptimizeImage(filenames[i], similarity);
			}
		});
	}

	int first = imagesPerThread * nthreads;

	std::thread lastThread{ [this, similarity, &filenames, first, nfiles]()
	{
		for (size_t i = first; i < nfiles; i++)
		{
			OptimizeImage(filenames[i], similarity);
		}
	} };

	std::for_each(threads.begin(), threads.end(), [](std::thread& t) {t.join(); });
	lastThread.join();
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
		
	auto temporaryFilename(getTemporaryFilename(imagePath));

	JpegEncoderDecoder::SaveJpeg(image, temporaryFilename, bestQuality);

	auto originalFileSize = getFileSize(imagePath);
	auto newFileSize = getFileSize(temporaryFilename);

	logFileSizesAndCompression(originalFileSize, newFileSize);

	if (newFileSize >= originalFileSize)
	{
		remove(temporaryFilename);

		m_logger.Log("Couldn't compress more");
	}
	else
	{
		auto newFileName(addSuffixToFileName(imagePath, "_compressed"));

		rename(temporaryFilename, newFileName);
	}
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

std::string ImageOptimizerImplementation::addSuffixToFileName(const std::string& filename, const std::string& suffix)
{
	path p(filename);

	path newFilename = p.parent_path() / path(p.stem().string() + suffix + p.extension().string());

	return newFilename.string();
}

std::string ImageOptimizerImplementation::getTemporaryFilename(const std::string& filename)
{
	for (unsigned long long counter = 0; ; counter++)
	{
		auto temporaryFileName = addSuffixToFileName(filename, "_tmp" + std::to_string(counter));

		if (!exists(temporaryFileName))
		{
			return temporaryFileName;
		}
	}
}

ImageOptimizerImplementation::filesize_t ImageOptimizerImplementation::getFileSize(const std::string& fileName)
{
	return file_size(fileName);
}

unsigned int ImageOptimizerImplementation::computeCompression(filesize_t originalSize, filesize_t newSize)
{
	return static_cast<int>((newSize * 100) / originalSize);
}

bool ImageOptimizerImplementation::isJpegFile(const directory_entry& file)
{
	if (!is_regular_file(file))
	{
		return false;
	}

	const std::regex jpegExtension(R"(^\.jpe?g$)", std::regex_constants::icase);

	return std::regex_match(file.path().extension().string(), jpegExtension);
}

void ImageOptimizerImplementation::logFileSizesAndCompression(filesize_t originalFileSize, filesize_t newFileSize)
{
	auto compression = computeCompression(originalFileSize, newFileSize);

	m_logger.Log("Original size: " + std::to_string(originalFileSize) + " New size: " + std::to_string(newFileSize) + " Compression: " + std::to_string(compression) + "%");
}
