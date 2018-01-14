#ifndef ImageProcessorImplementation_h__
#define ImageProcessorImplementation_h__

#include "Logger.h"
#include "ImageProcessor.h"
#include "ImageSimilarity.h"
#include "OptimizationResult.h"

#include <string>
#include <filesystem>

namespace cv
{
	class Mat;
}

class OptimizationSequence;

class  ImageOptimizer
{
public:
	ImageOptimizer();

	void SetLogCallbacks(traceCallback_t traceCallback, warningCallback_t warningCallback, errorCallback_t errorCallback);

	OptimizationResult OptimizeImage(const std::string& imagePath, ImageSimilarity::Similarity similarity);
	OptimizationResult OptimizeFolder(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity);
	OptimizationResult OptimizeFolderRecursive(const std::string& imageFolderPath, ImageSimilarity::Similarity similarity);

	static std::string GetVersion();
	
private:
	using filesize_t = unsigned long long;
	using iterator_t = std::vector<std::string>::const_iterator;

	static std::string addSuffixToFileName(const std::string& filename, const std::string& suffix);

	static bool isJpegFile(const std::experimental::filesystem::directory_entry& file);
	static std::vector<std::string> getJpegInFolder(const std::string& imageFolderPath);
	static std::vector<std::string> getAllFoldersInFolder(const std::string& folderPath);
	static std::string getSuffixedFilename(const std::string& filename, const std::string& suffix);

	OptimizationResult parallelOptimizeImages(const std::vector<std::string>& filenames, ImageSimilarity::Similarity similarity);
	OptimizationResult optimizeImages(const iterator_t& first, const iterator_t& last, ImageSimilarity::Similarity similarity);

	cv::Mat loadImage(const std::string& imagePath);

	void validateFolderPath(const std::string& imageFolderPath);
	void validateImagePath(const std::string& imagePath);
	void validateImage(const cv::Mat& image);
	void handleInvalidArgument(const char* message);
	void logFileSizesAndCompression(OptimizationResult optimizationResult);

	static const std::string s_version;

	Logger m_logger;

	ImageProcessor m_imageProcessor;
};

#endif // ImageProcessorImplementation_h__