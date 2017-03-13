#include "OptimizationSequence.h"

#include <algorithm>



void OptimizationSequence::AddOptimizationResult(Quality quality, ImageSimilarity::Similarity ssim)
{
	m_optimizationResults.push_back(std::make_pair(quality, ssim));
}

Quality OptimizationSequence::BestQuality() const
{
	return (*std::min_element(m_optimizationResults.begin(), m_optimizationResults.end(), 
		[targetSimilarity = m_targetSimilarity](const auto& first, const auto& second) {return abs(first.second - targetSimilarity) < abs(second.second - targetSimilarity); })).first;
}

unsigned int OptimizationSequence::NumberOfIterations() const
{
	return m_optimizationResults.size();
}

bool OptimizationSequence::HasBeenTried(Quality quality) const
{
	return std::find_if(m_optimizationResults.begin(), m_optimizationResults.end(), [quality](auto const& item) {return item.first == quality; }) != m_optimizationResults.end();
}
