#include "jpeg.hpp"

#include "turbojpeg.h"

#include <fstream>
#include <chrono>


TJSAMP chromaSampling(TJPF colorspace) {
	return colorspace == TJPF_GRAY ? TJSAMP_GRAY : TJSAMP_420;
}

namespace jpeg {

	std::pair<std::unique_ptr<unsigned char[]>, size_t> memory_encode(const Image& image, TJPF colorspace, unsigned int quality) {
		auto imageData{ image.buffer.get() };

		tjhandle _jpegCompressor = tjInitCompress();

		unsigned long size = tjBufSize(image.width, image.height, chromaSampling(colorspace));
		auto buffer = std::make_unique<unsigned char[]>(size);
		unsigned char* compressedImage = buffer.get();

		auto res = tjCompress2(_jpegCompressor, imageData, image.width, 0, image.height, colorspace,
			&compressedImage, &size, chromaSampling(colorspace), quality, TJFLAG_ACCURATEDCT | TJFLAG_NOREALLOC);

		tjDestroy(_jpegCompressor);

		if (res != 0) {
			throw std::runtime_error(tjGetErrorStr());
		}

		return { std::move(buffer), static_cast<size_t>(size) };
	}

	Image memory_decode(const std::unique_ptr<unsigned char[]>& buffer, const int size, TJPF colorspace) {
		int jpegSubsamp;

		tjhandle _jpegDecompressor = tjInitDecompress();
		Image image;

		tjDecompressHeader2(_jpegDecompressor, const_cast<uint8_t*>(buffer.get()), size, &image.width, &image.height, &jpegSubsamp);

		auto channels{ tjPixelSize[colorspace] };

		image.buffer = std::make_unique<unsigned char[]>(image.width * image.height * channels);

		tjDecompress2(_jpegDecompressor, buffer.get(), size, image.buffer.get(), image.width, 0/*pitch*/, image.height, colorspace, TJFLAG_ACCURATEDCT);

		tjDestroy(_jpegDecompressor);

		return image;
	}

	Image memory_encode_decode_grayscale(const Image& image, unsigned int quality) {
		auto imageData{ image.buffer.get() };
		
		tjhandle _jpegCompressor = tjInitCompress();
		
		unsigned char* compressedImage = nullptr;
		unsigned long size;
		
		auto res = tjCompress2(_jpegCompressor, imageData, image.width, 0, image.height, TJPF_GRAY,
			&compressedImage, &size, chromaSampling(TJPF_GRAY), quality, TJFLAG_ACCURATEDCT);
		
		tjDestroy(_jpegCompressor);
		
		if (res != 0) {
			tjFree(compressedImage);
		
			throw std::runtime_error(tjGetErrorStr());
		}
		
		int jpegSubsamp;
		
		tjhandle _jpegDecompressor = tjInitDecompress();
		Image outImage;
		
		tjDecompressHeader2(_jpegDecompressor, compressedImage, size, &outImage.width, &outImage.height, &jpegSubsamp);
		
		auto channels{ tjPixelSize[TJPF_GRAY] };
		
		outImage.buffer = std::make_unique<unsigned char[]>(outImage.width * outImage.height * channels);
		
		tjDecompress2(_jpegDecompressor, compressedImage, size, outImage.buffer.get(), outImage.width, 0/*pitch*/, outImage.height, TJPF_GRAY, TJFLAG_ACCURATEDCT);
		
		tjFree(compressedImage);
		tjDestroy(_jpegDecompressor);
		
		return outImage;
	}


	void save(const Image& image, const std::string& filename, unsigned int quality) {
		auto compressedImage = memory_encode(image, TJPF_RGB, quality);

		std::ofstream imOut(filename, std::ios::binary);

		imOut.write(reinterpret_cast<const char*>(compressedImage.first.get()), compressedImage.second);
	}

	Image load(const std::string& imagePath, TJPF colorspace) {
		std::ifstream file(imagePath, std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		auto buffer = std::make_unique<unsigned char[]>(size);
		if (file.read(reinterpret_cast<char*>(buffer.get()), size))
		{
			/* worked! */
		}

		return memory_decode(buffer, size, colorspace);
	}

	Image load_color(const std::string& imagePath) {
		return load(imagePath, TJPF_RGB);
	}

	Image load_grayscale(const std::string& imagePath) {
		return load(imagePath, TJPF_GRAY);
	}
}