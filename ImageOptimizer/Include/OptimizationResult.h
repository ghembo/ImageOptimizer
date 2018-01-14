#pragma once

class OptimizationResult
{
public:
	using filesize_t = unsigned long long;

	OptimizationResult();
	OptimizationResult(filesize_t originalSize, filesize_t compressedSize);
	OptimizationResult(const OptimizationResult& other) = default;

	unsigned int GetCompressionPercentage() const;
	filesize_t GetOriginalSize() const { return m_originalSize; };
	filesize_t GetCompressedSize() const { return m_compressedSize; };
	bool IsCompressed() const;

	OptimizationResult GetUncompressedResult() const;

	OptimizationResult& operator=(const OptimizationResult& other) = default;
	OptimizationResult& operator+=(const OptimizationResult& other);

private:

	filesize_t m_originalSize;
	filesize_t m_compressedSize;
};

inline OptimizationResult operator+(OptimizationResult first, const OptimizationResult& second)
{
	return first += second;
}
