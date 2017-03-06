namespace cv
{
	class Mat;
}

namespace ImageSimilarity
{
	struct _kernel {
		int w;                  /**< The kernel width */
		int h;                  /**< The kernel height */
	};

	float ComputeSsim(const cv::Mat& referenceImage, const cv::Mat& compareImage);
	void decimate(float *img, int width, int height, int factor, float *result, int *rw, int *rh);
	void convolve(float *img, int width, int height, const _kernel *k, float *result, int *rw, int *rh);
}