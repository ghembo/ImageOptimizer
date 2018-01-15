#pragma once

#include "iopt/image_similarity.hpp"
#include "quality.hpp"

#include <vector>
#include <utility>



class OptimizationSequence
{
private:
	using sequence_t = std::vector<std::pair<Quality, ImageSimilarity::Similarity>>;

public:
	OptimizationSequence(ImageSimilarity::Similarity targetSimilarity) : m_targetSimilarity{ targetSimilarity }
	{
	}

	void AddOptimizationResult(Quality quality, ImageSimilarity::Similarity ssim);

	Quality BestQuality() const;
	size_t NumberOfIterations() const;
	bool HasBeenTried(Quality quality) const;

	using iterator = sequence_t::iterator;
	using const_iterator = sequence_t::const_iterator;

	iterator begin() { return m_optimizationResults.begin(); }
	const_iterator begin() const { return m_optimizationResults.cbegin(); }
	iterator end() { return m_optimizationResults.end(); }
	const_iterator end() const { return m_optimizationResults.cend(); }

private:
	sequence_t m_optimizationResults;
	ImageSimilarity::Similarity m_targetSimilarity;
};
