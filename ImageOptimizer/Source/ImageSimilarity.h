namespace cv
{
	class Mat;
}

namespace ImageSimilarity
{
	float ComputeSsim(const cv::Mat& referenceImage, const cv::Mat& compareImage);
}