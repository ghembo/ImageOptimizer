#include "OptimizationSequence.h"



void OptimizationSequence::AddOptimizationResult(unsigned int quality, float ssim)
{
	m_optimizationResults.push_back(std::make_pair(quality, ssim));
}

unsigned int OptimizationSequence::BestQuality() const
{
	return m_optimizationResults[m_optimizationResults.size() - 1].first;
}

unsigned int OptimizationSequence::NumberOfIterations() const
{
	return m_optimizationResults.size();
}
