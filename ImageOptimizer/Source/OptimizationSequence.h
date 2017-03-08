#ifndef OptimizationSequence_h__
#define OptimizationSequence_h__

#include <vector>
#include <utility>



class OptimizationSequence
{
private:
	using sequence_t = std::vector<std::pair<unsigned int, float>>;

public:
	void AddOptimizationResult(unsigned int quality, float ssim);
	unsigned int GetBestQuality();
	unsigned int GetNumberOfIterations();

	using iterator = sequence_t::iterator;
	using const_iterator = sequence_t::const_iterator;

	iterator begin() { return m_optimizationResults.begin(); }
	const_iterator begin() const { return m_optimizationResults.cbegin(); }
	iterator end() { return m_optimizationResults.end(); }
	const_iterator end() const { return m_optimizationResults.cend(); }

private:
	sequence_t m_optimizationResults;
};

#endif // !OptimizationSequence_h__
