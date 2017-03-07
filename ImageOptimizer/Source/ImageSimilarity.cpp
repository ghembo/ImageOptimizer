#include "ImageSimilarity.h"

#include "opencv2\core\core.hpp"

#include <algorithm>
#include <tuple>
#include <vector>
#include <memory>

namespace ImageSimilarity
{
	#define SQUARE_LEN 8

	std::pair<int, int> convolve(float *img, int width, int height, float *result)
	{
		int dst_w = width - SQUARE_LEN + 1;
		int dst_h = height - SQUARE_LEN + 1;
		float* dst = result ? result : img;

		double* boxes = new double[width * height];

		boxes[0] = img[0];
		for (int x = 1; x < width; x++)
		{
			boxes[x] = boxes[x-1] + img[x];
		}

		for (int y = 1; y < height; y++)
		{
			double* b = &boxes[y * width];
			*b++ = boxes[(y - 1) * width] + img[y * width];
			for (int x = 1; x < width; x++)
			{
				double val = b[-1] + b[-width] - b[-width - 1] + img[y * width + x];
				*b++ = val;
			}
		}
			
 		dst[0] = boxes[(SQUARE_LEN - 1) * width + SQUARE_LEN - 1];

		for (int x = 1; x < dst_w; x++)
		{
			dst[x] = (boxes[(SQUARE_LEN - 1) * width + x + SQUARE_LEN - 1] - boxes[(SQUARE_LEN - 1) * width + x - 1]);
		}

		for (int y = 1; y < dst_h; ++y)
		{
			dst[y * dst_w] = (boxes[(y + SQUARE_LEN - 1) * width + SQUARE_LEN - 1] - boxes[(y - 1) * width + SQUARE_LEN - 1]);

			for (int x = 1; x < dst_w; ++x)
			{
				double box_sum = boxes[(y + SQUARE_LEN - 1) * width + x - 1 + SQUARE_LEN]
								- boxes[(y + SQUARE_LEN - 1) * width + x - 1]
								- boxes[(y - 1) * width + x - 1 + SQUARE_LEN]
								+ boxes[(y - 1) * width + x - 1];

				dst[y*dst_w + x] = (float)box_sum;
			}
		}

		delete[] boxes;

		return{ dst_w , dst_h };
	}

	std::pair<int, int> decimate(float *image, int width, int height, int scaling)
	{
		int scaledWidth = width / scaling;
		int effectiveWidth = scaledWidth * scaling;
		int scaledHeight = height / scaling;
		int effectiveHeight = scaledHeight * scaling;
		float* destination = image;

		float normalization = 1.0f / (scaling * scaling);

		for (int y = 0; y < effectiveHeight; y += scaling)
		{
			for (int x = 0; x < effectiveWidth; x += scaling)
			{
				float sum = 0.0f;
				for (int row = 0; row < scaling; row++) // unroll agambini
				{
					float* curr_img = image + (y + row) * width + x;
					for (int col = 0; col < scaling; col++)
					{
						sum += *curr_img++;
					}
				}

				*destination++ = sum * normalization;
			}
		}

		return { scaledWidth , scaledHeight };
	}

	float ssim(float *ref, float *cmp, int width, int height)
	{
		float* ref_mu = new float[width*height];
		float* cmp_mu = new float[width*height];
		float* ref_sigma_sqd = new float[width*height];
		float* cmp_sigma_sqd = new float[width*height];
		float* sigma_both = new float[width*height];

		/* Calculate mean */
		convolve(ref, width, height, ref_mu);
		convolve(cmp, width, height, cmp_mu);
		
		for (int offset = 0; offset < width * height; offset++)
		{
			ref_sigma_sqd[offset] = ref[offset] * ref[offset];
			cmp_sigma_sqd[offset] = cmp[offset] * cmp[offset];
			sigma_both[offset] = ref[offset] * cmp[offset];
		}

		/* Calculate sigma */
		convolve(ref_sigma_sqd, width, height, 0);
		convolve(cmp_sigma_sqd, width, height, 0);
		std::tie(width, height) = convolve(sigma_both, width, height, 0);

		constexpr int L = 255;
		constexpr float K1 = 0.01f;
		constexpr float K2 = 0.03f;

		constexpr float C1 = (K1*L) * (K1*L);
		constexpr float C2 = (K2*L) * (K2*L);

		double ssim_sum = 0.0;
		float norm = 1.0f / (SQUARE_LEN * SQUARE_LEN);

		for (int offset = 0; offset < width * height; offset++)
		{
			ref_mu[offset] *= norm;
			cmp_mu[offset] *= norm;

			float ref_mu_sq =  ref_mu[offset] * ref_mu[offset];
			float cmp_mu_sq = cmp_mu[offset] * cmp_mu[offset];

			float ref_sigma_sqd_value = ref_sigma_sqd[offset] * norm - ref_mu_sq;
			float cmp_sigma_sqd_value = cmp_sigma_sqd[offset] * norm - cmp_mu_sq;

			float denominator = (ref_mu_sq + cmp_mu_sq + C1) * (ref_sigma_sqd_value + cmp_sigma_sqd_value + C2);

			float sigma_both_value = sigma_both[offset] * norm - ref_mu[offset] * cmp_mu[offset];

			float numerator   = (2.0f * ref_mu[offset] * cmp_mu[offset] + C1) * (2.0f * sigma_both_value + C2);
			ssim_sum += numerator / denominator;
		}

		delete[] ref_mu;
		delete[] cmp_mu;
		delete[] ref_sigma_sqd;
		delete[] cmp_sigma_sqd;
		delete[] sigma_both;

		return (float)(ssim_sum / (width * height));
	}

	int computeScale(int width, int height)
	{
		return std::max(1, (int)(std::min(width, height) / 256.0f + 0.5f)); // TODO
	}

	std::unique_ptr<float[]> convertToFloat(uchar* image, int width, int height)
	{
		int size = width * height;

		auto floatImage = std::make_unique<float[]>(size);
		auto floatImagePointer = floatImage.get();

		for (int i = 0; i < size; i++) // *p++ agambini unroll
		{
			floatImagePointer[i] = static_cast<float>(image[i]);
		}

		return floatImage;
	}

	// TODO usare int
	float ComputeSsim(const cv::Mat& referenceImage, const cv::Mat& compareImage)
	{
		if (!referenceImage.isContinuous() || !compareImage.isContinuous())
		{
			throw std::invalid_argument("Images must be continuous for efficient processing");
		}

		if (referenceImage.cols != compareImage.cols || referenceImage.rows != compareImage.rows)
		{
			throw std::invalid_argument("Images must be same size");
		}

		int width = referenceImage.cols;
		int height = referenceImage.rows;

		/* Convert image values to floats. Forcing stride = width. */
		auto referenceFloat = convertToFloat(referenceImage.data, width, height);
		auto compareFloat = convertToFloat(compareImage.data, width, height);

		int scale = computeScale(width, height);

		/* Scale the images down if required */
		if (scale > 1)
		{
			decimate(referenceFloat.get(), width, height, scale);
			std::tie(width, height) = decimate(compareFloat.get(), width, height, scale);
		}
		
		return ssim(referenceFloat.get(), compareFloat.get(), width, height);
	}
}
