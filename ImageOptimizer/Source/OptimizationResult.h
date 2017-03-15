#ifndef OptimizationResult_h__
#define OptimizationResult_h__



class OptimizationResult
{
public:
	using filesize_t = unsigned long long;

	OptimizationResult(filesize_t originalSize, filesize_t compressedSize);

	unsigned int GetCompressionPercentage();
	filesize_t GetOriginalSize() { return m_originalSize; };
	filesize_t GetCompressedSize() { return m_compressedSize; };
	bool IsCompressed();

private:

	filesize_t m_originalSize;
	filesize_t m_compressedSize;
};

#endif // !OptimizationResult_h__
