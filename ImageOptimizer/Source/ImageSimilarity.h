#ifndef ImageSimilarity_h__
#define ImageSimilarity_h__

#include <ostream>

namespace cv
{
	class Mat;
}

namespace ImageSimilarity
{
	class Similarity
	{
	public:
		constexpr Similarity(float similarity) : m_value{ similarity }
		{
		}

		float GetValue() const { return m_value; }

	private:
		float m_value;
	};

	inline bool operator> (const Similarity& lhs, const Similarity& rhs)
	{
		return lhs.GetValue() > rhs.GetValue();
	}

	std::ostream& operator<< (std::ostream& stream, Similarity similarity);

	Similarity ComputeSsim(const cv::Mat& referenceImage, const cv::Mat& compareImage);
}

#endif // !ImageSimilarity_h__

