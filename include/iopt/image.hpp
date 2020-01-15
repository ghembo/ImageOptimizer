#pragma once

#include <memory>

struct Image {
	int width;
	int height;
	std::unique_ptr<unsigned char[]> buffer;
};
