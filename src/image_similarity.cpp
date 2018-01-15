#include "iopt/image_similarity.hpp"

#include "jpeg.hpp"

#include <algorithm>
#include <tuple>
#include <vector>
#include <memory>


namespace ImageSimilarity
{
	// Formatting logic for ImageSimilarity
	std::ostream& operator<< (std::ostream& stream, Similarity similarity)
	{
		auto originalPrecision = stream.precision();

		stream.precision(std::numeric_limits<float>::max_digits10);

		stream << similarity.GetValue();

		stream.precision(originalPrecision);

		return stream;
	}



	class Size
	{
	public:
		Size() :
			m_width{ 0 }, m_height{ 0 }
		{
		}

		Size(unsigned int width, unsigned int height) :
			m_width{ width }, m_height{ height }
		{
		}

		unsigned int Total() const { return m_width * m_height; };

		Size operator/(unsigned int divisor) const { return{ m_width / divisor, m_height / divisor }; };
		Size operator*(unsigned int divisor) const { return{ m_width * divisor, m_height * divisor }; };

		unsigned int m_width;
		unsigned int m_height;
	};

	Size ImageSize(const Image& image)
	{
		return{ (unsigned int)image.width, (unsigned int)image.height};
	}

	#define SQUARE_LEN 8

	Size convolve(float *img, Size size, float *result)
	{
		int width = size.m_width;
		unsigned int height = size.m_height;
		unsigned int dst_w = width - SQUARE_LEN + 1;
		unsigned int dst_h = height - SQUARE_LEN + 1;
		float* dst = result ? result : img;

		double* boxes = new double[size.Total()];

		boxes[0] = img[0];
		for (int x = 1; x < width; x++)
		{
			boxes[x] = boxes[x-1] + img[x];
		}

		for (unsigned int y = 1; y < height; y++)
		{
			double* b = &boxes[y * width];
			*b++ = boxes[(y - 1) * width] + img[y * width];
			for (int x = 1; x < width; x++)
			{
				double val = b[-1] + b[-width] - b[-width - 1] + img[y * width + x];
				*b++ = val;
			}
		}
			
 		dst[0] = static_cast<float>(boxes[(SQUARE_LEN - 1) * width + SQUARE_LEN - 1]);

		for (unsigned int x = 1; x < dst_w; x++)
		{
			dst[x] = static_cast<float>(boxes[(SQUARE_LEN - 1) * width + x + SQUARE_LEN - 1] - boxes[(SQUARE_LEN - 1) * width + x - 1]);
		}

		for (unsigned int y = 1; y < dst_h; ++y)
		{
			dst[y * dst_w] = static_cast<float>(boxes[(y + SQUARE_LEN - 1) * width + SQUARE_LEN - 1] - boxes[(y - 1) * width + SQUARE_LEN - 1]);

			for (unsigned int x = 1; x < dst_w; ++x)
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
	
	std::pair<std::unique_ptr<float[]>, Size> decimate(const uint8_t* image, Size size, unsigned int scaling) {
		const unsigned int width = size.m_width;
		Size scaledSize = size / scaling;
		const Size efectiveSize = scaledSize * scaling;

		auto destination = std::make_unique<float[]>(scaledSize.Total());
		auto destinationPointer = destination.get();

		const float normalization = 1.0f / (scaling * scaling);

		for (unsigned int y = 0; y < efectiveSize.m_height; y += scaling)
		{
			for (unsigned int x = 0; x < efectiveSize.m_width; x += scaling)
			{
				unsigned int sum = 0;
				for (unsigned int row = 0; row < scaling; row++) // unroll agambini
				{
					const uint8_t* curr_img = image + (y + row) * width + x;
					for (unsigned int col = 0; col < scaling; col++)
					{
						sum += curr_img[col];
					}
				}

				*destinationPointer++ = sum * normalization;
			}
		}

		return{ std::move(destination), std::move(scaledSize) };
	}

	float ssim(float *ref, float *cmp, Size size)
	{
		unsigned int totalSize = size.Total();

		float* ref_mu = new float[totalSize];
		float* cmp_mu = new float[totalSize];
		float* ref_sigma_sqd = new float[totalSize];
		float* cmp_sigma_sqd = new float[totalSize];
		float* sigma_both = new float[totalSize];

		/* Calculate mean */
		convolve(ref, size, ref_mu);
		convolve(cmp, size, cmp_mu);
		
		for (unsigned int offset = 0; offset < totalSize; offset++)
		{
			ref_sigma_sqd[offset] = ref[offset] * ref[offset];
			cmp_sigma_sqd[offset] = cmp[offset] * cmp[offset];
			sigma_both[offset] = ref[offset] * cmp[offset];
		}

		/* Calculate sigma */
		convolve(ref_sigma_sqd, size, 0);
		convolve(cmp_sigma_sqd, size, 0);
		size = convolve(sigma_both, size, 0);

		totalSize = size.Total();

		constexpr int L = 255;
		constexpr float K1 = 0.01f;
		constexpr float K2 = 0.03f;

		constexpr float C1 = (K1*L) * (K1*L);
		constexpr float C2 = (K2*L) * (K2*L);

		double ssim_sum = 0.0;
		float norm = 1.0f / (SQUARE_LEN * SQUARE_LEN);

		for (unsigned int offset = 0; offset < totalSize; offset++)
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

		return (float)(ssim_sum / totalSize);
	}

	int computeScale(Size size)
	{
		return std::max(1, (int)(std::min(size.m_width, size.m_height) / 256.0f + 0.5f)); // TODO
	}

	std::unique_ptr<float[]> convertToFloat(const uint8_t* image, Size size)
	{
		unsigned int totalSize = size.Total();

		auto floatImage = std::make_unique<float[]>(totalSize);
		auto floatImagePointer = floatImage.get();

		for (unsigned int i = 0; i < totalSize; i++) // *p++ agambini unroll
		{
			floatImagePointer[i] = static_cast<float>(image[i]);
		}

		return floatImage;
	}

	// TODO usare int
	Similarity ComputeSsim(const Image& referenceImage, const Image& compareImage)
	{
		if (referenceImage.width != compareImage.width || referenceImage.height != compareImage.height)
		{
			throw std::invalid_argument("Images must be same size");
		}

		Size size = ImageSize(referenceImage);

		std::unique_ptr<float[]> referenceFloat;
		std::unique_ptr<float[]> compareFloat;

		int scale = computeScale(size);

		if (scale > 1)
		{
			Size newSize;
			std::tie(referenceFloat, newSize) = decimate(referenceImage.data.data(), size, scale);
			std::tie(compareFloat, newSize) = decimate(compareImage.data.data(), size, scale);
			
			size = newSize;
		}
		else
		{
			referenceFloat = convertToFloat(referenceImage.data.data(), size);
			compareFloat = convertToFloat(compareImage.data.data(), size);
		}
		
		return{ ssim(referenceFloat.get(), compareFloat.get(), size) };
	}
}
