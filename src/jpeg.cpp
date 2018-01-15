#include "jpeg.hpp"

#include "turbojpeg.h"

#include <fstream>
#include <chrono>


FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

TJSAMP chromaSampling(TJPF colorspace) {
	return colorspace == TJPF_GRAY ? TJSAMP_GRAY : TJSAMP_420;
}

namespace jpeg {

	std::vector<uint8_t> encodeImageTurbo(const unsigned char* image, int width, int height, TJPF colorspace, unsigned int quality) {
		tjhandle _jpegCompressor = tjInitCompress();

		unsigned char* compressedImage = nullptr;
		unsigned long size;

		auto res = tjCompress2(_jpegCompressor, image, width, 0, height, colorspace,
			&compressedImage, &size, chromaSampling(colorspace), quality, TJFLAG_ACCURATEDCT);

		tjDestroy(_jpegCompressor);

		if (res != 0) {
			tjFree(compressedImage);

			throw std::exception(tjGetErrorStr());
		}

		std::vector<uint8_t> compressed(compressedImage, compressedImage + size);

		tjFree(compressedImage);
		
		return compressed;
	}

	std::vector<unsigned char> decodeImageTurbo(const uint8_t* buffer, int size, int* width, int* height, TJPF colorspace) {
		int jpegSubsamp;

		tjhandle _jpegDecompressor = tjInitDecompress();

		tjDecompressHeader2(_jpegDecompressor, const_cast<uint8_t*>(buffer), size, width, height, &jpegSubsamp);

		auto channels{ tjPixelSize[colorspace] };

		std::vector<unsigned char> uncompressedBuffer(*width**height * channels);

		tjDecompress2(_jpegDecompressor, buffer, size, uncompressedBuffer.data(), *width, 0/*pitch*/, *height, colorspace, TJFLAG_ACCURATEDCT);

		tjDestroy(_jpegDecompressor);

		return uncompressedBuffer;
	}

	std::vector<uint8_t> memory_encode(const Image& image, unsigned int quality) {
		return encodeImageTurbo(image.data.data(), image.width, image.height, TJPF_GRAY, quality);
	}

	Image memory_decode_grayscale(const std::vector<uint8_t>& buffer)
	{
		Image image;

		image.data = decodeImageTurbo(buffer.data(), buffer.size(), &image.width, &image.height, TJPF_GRAY);

		return image;
	}

	void save(const Image& image, const std::string& filename, unsigned int quality)
	{
		auto compressedImage = encodeImageTurbo(image.data.data(), image.width, image.height, TJPF_RGB, quality);

		std::ofstream imOut(filename, std::ios::binary);

		imOut.write(reinterpret_cast<char*>(compressedImage.data()), compressedImage.size());
	}

	std::vector<unsigned char> loadImageTurbo(const std::string& imagePath, int* width, int* height, TJPF colorspace) {
		std::ifstream file(imagePath, std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<unsigned char> buffer(size);
		if (file.read(reinterpret_cast<char*>(buffer.data()), size))
		{
			/* worked! */
		}

		return decodeImageTurbo(buffer.data(), buffer.size(), width, height, colorspace);
	}

	Image load_color(const std::string& imagePath) {
		Image image;

		image.data = loadImageTurbo(imagePath, &image.width, &image.height, TJPF_RGB);

		return image;
	}

	Image load_grayscale(const std::string& imagePath)
	{
		Image image;

		image.data = loadImageTurbo(imagePath, &image.width, &image.height, TJPF_GRAY);

		return image;
	}
}