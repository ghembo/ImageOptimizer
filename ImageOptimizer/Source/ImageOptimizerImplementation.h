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
	static std::string getNewFilename(const std::string& filename);

	static unsigned long long getFileSize(const std::string& fileName);
	static unsigned int computeCompression(unsigned long long originalSize, unsigned long long newSize);

	static bool isJpegFile(const boost::filesystem::directory_entry& file);
	static auto getJpegInFolder(const std::string& imageFolderPath);
	static auto getAllFoldersInFolder(const std::string& folderPath);

	cv::Mat loadImage(const std::string& imagePath);

	void validateFolderPath(const std::string& imageFolderPath);
	void validateImagePath(const std::string& imagePath);
	void validateImage(const cv::Mat& image);
	void handleInvalidArgument(const char* message);
	void logFileSizesAndCompression(const std::string& originalFileName, const std::string& newFileName);

	ImageProcessor m_imageProcessor;

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__