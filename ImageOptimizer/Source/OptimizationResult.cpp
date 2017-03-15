#include "OptimizationResult.h"

OptimizationResult::OptimizationResult(filesize_t originalSize, filesize_t compressedSize)
	: m_originalSize(originalSize), m_compressedSize(compressedSize)
{
}

unsigned int OptimizationResult::GetCompressionPercentage()
{
	return static_cast<int>((m_compressedSize * 100) / m_originalSize);
}

bool OptimizationResult::IsCompressed()
{
	return m_compressedSize < m_originalSize;
}
