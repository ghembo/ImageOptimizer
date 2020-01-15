#include "jpeg.hpp"

#include "turbojpeg.h"

#include <fstream>
#include <chrono>


TJSAMP chromaSampling(TJPF colorspace) {
	return colorspace == TJPF_GRAY ? TJSAMP_GRAY : TJSAMP_420;
}

namespace jpeg {

	std::vector<uint8_t> memory_encode(const Image& image, TJPF colorspace, unsigned int quality) {
		auto imageData{ image.data.data() };

		tjhandle _jpegCompressor = tjInitCompress();

		unsigned char* compressedImage = nullptr;
		unsigned long size;

		auto res = tjCompress2(_jpegCompressor, imageData, image.width, 0, image.height, colorspace,
			&compressedImage, &size, chromaSampling(colorspace), quality, TJFLAG_ACCURATEDCT);

		tjDestroy(_jpegCompressor);

		if (res != 0) {
			tjFree(compressedImage);

			throw std::runtime_error(tjGetErrorStr());
		}

		std::vector<uint8_t> compressed(compressedImage, compressedImage + size);

		tjFree(compressedImage);
		
		return compressed;
	}

	Image memory_decode(const std::vector<uint8_t>& buffer, TJPF colorspace) {
		int jpegSubsamp;

		tjhandle _jpegDecompressor = tjInitDecompress();
		Image image;

		tjDecompressHeader2(_jpegDecompressor, const_cast<uint8_t*>(buffer.data()), buffer.size(), &image.width, &image.height, &jpegSubsamp);

		auto channels{ tjPixelSize[colorspace] };

		image.data = std::vector<unsigned char>(image.width * image.height * channels);

		tjDecompress2(_jpegDecompressor, buffer.data(), buffer.size(), image.data.data(), image.width, 0/*pitch*/, image.height, colorspace, TJFLAG_ACCURATEDCT);

		tjDestroy(_jpegDecompressor);

		return image;
	}

	std::vector<uint8_t> memory_encode_grayscale(const Image& image, unsigned int quality) {
		return memory_encode(image, TJPF_GRAY, quality);
	}

	Image memory_decode_grayscale(const std::vector<uint8_t>& buffer) {
		return memory_decode(buffer, TJPF_GRAY);
	}

	void save(const Image& image, const std::string& filename, unsigned int quality) {
		auto compressedImage = memory_encode(image, TJPF_RGB, quality);

		std::ofstream imOut(filename, std::ios::binary);

		imOut.write(reinterpret_cast<char*>(compressedImage.data()), compressedImage.size());
	}

	Image load(const std::string& imagePath, TJPF colorspace) {
		std::ifstream file(imagePath, std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<unsigned char> buffer(size);
		if (file.read(reinterpret_cast<char*>(buffer.data()), size))
		{
			/* worked! */
		}

		return memory_decode(buffer, colorspace);
	}

	Image load_color(const std::string& imagePath) {
		return load(imagePath, TJPF_RGB);
	}

	Image load_grayscale(const std::string& imagePath) {
		return load(imagePath, TJPF_GRAY);
	}
}