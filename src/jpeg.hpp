#pragma once

#include "iopt/image.hpp"

#include <vector>
#include <string>

namespace jpeg {
	Image load_color(const std::string& imagePath);
	Image load_grayscale(const std::string& imagePath);
	
	void save(const Image & image, const std::string& filename, unsigned int quality);

	Image memory_encode_decode_grayscale(const Image& image, unsigned int quality);
};
