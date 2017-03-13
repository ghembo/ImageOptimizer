#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"
#include "ImageProcessor.h"

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

	void OptimizeImage(const std::string& imagePath);
	void OptimizeFolder(const std::string& imageFolderPath);
	void OptimizeFolderRecursive(const std::string& imageFolderPath);
	
private:
	static std::string getNewFilename(const std::string& filename);

	static bool isJpegFile(const boost::filesystem::directory_entry& file);
	static auto getJpegInFolder(const std::string& imageFolderPath);
	static auto getAllFoldersInFolder(const std::string& folderPath);

	cv::Mat loadImage(const std::string& imagePath);

	void validateFolderPath(const std::string& imageFolderPath);
	void validateImagePath(const std::string& imagePath);
	void handleInvalidArgument(const char* message);	

	ImageProcessor m_imageProcessor;

	Logger m_logger;
};

#endif // ImageProcessorImplementation_h__