#include "OptimizationResult.h"

OptimizationResult::OptimizationResult()
	: m_originalSize(0), m_compressedSize(0)
{
}

OptimizationResult::OptimizationResult(filesize_t originalSize, filesize_t compressedSize)
	: m_originalSize(originalSize), m_compressedSize(compressedSize)
{
}

unsigned int OptimizationResult::GetCompressionPercentage() const
{
	return static_cast<int>((m_compressedSize * 100) / m_originalSize);
}

bool OptimizationResult::IsCompressed() const
{
	return m_compressedSize < m_originalSize;
}

OptimizationResult OptimizationResult::GetUncompressedResult() const
{
	return{ m_originalSize , m_originalSize };
}

OptimizationResult& OptimizationResult::operator+=(const OptimizationResult & other)
{
	m_originalSize += other.GetOriginalSize();
	m_compressedSize += other.GetCompressedSize();

	return *this;
}
