#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"
#include "ImageProcessor.h"
#include "ImageSimilarity.h"

#include <string>

namespace cv
{
	class Mat;
}

namespace boost
{
	namespace filesystem
	{
		class directory_entry;
	}
}

class OptimizationSequence;

class  ImageOptimizerImplementation
{
public:
	ImageOptimizerImplementation();

	void OptimizeImage(const std::string& imagePath, ImageSimilarity::Similarity similarity);
	void OptimizeFolder(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity);
	void OptimizeFolderRecursive(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity);
	
private:
	using filesize_t = unsigned long long;

	static std::string addSuffixToFileName(const std::string& filename, const std::string& suffix);

	static filesize_t getFileSize(const std::string& fileName);
	static unsigned int computeCompression(filesize_t originalSize, filesize_t newSize);

	static bool isJpegFile(const boost::filesystem::directory_entry& file);
	static auto getJpegInFolder(const std::string& imageFolderPath);
	static auto getAllFoldersInFolder(const std::string& folderPath);
	static std::string getTemporaryFilename(const std::string& filename);

	void optimizeImages(const std::vector<std::string>& filenames, ImageSimilarity::Similarity similarity);

	cv::Mat loadImage(const std::string& imagePath);

	void validateFolderPath(const std::string& imageFolderPath);
	void validateImagePath(const std::string& imagePath);
	void validateImage(const cv::Mat& image);
	void handleInvalidArgument(const char* message);
	void logFileSizesAndCompression(filesize_t originalFileSize, filesize_t newFileSize);

	ImageProcessor m_imageProcessor;

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__