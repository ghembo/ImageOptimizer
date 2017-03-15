#ifndef OptimizationResult_h__
#define OptimizationResult_h__



class OptimizationResult
{
public:
	using filesize_t = unsigned long long;

	OptimizationResult(filesize_t originalSize, filesize_t compressedSize);

	unsigned int GetCompressionPercentage() const;
	filesize_t GetOriginalSize() const { return m_originalSize; };
	filesize_t GetCompressedSize() const { return m_compressedSize; };
	bool IsCompressed() const;

	OptimizationResult GetUncompressedResult() const;

private:

	filesize_t m_originalSize;
	filesize_t m_compressedSize;
};

#endif // !OptimizationResult_h__
