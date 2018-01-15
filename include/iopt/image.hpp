#pragma once

#include <vector>

struct Image {
	int width;
	int height;
	std::vector<unsigned char> data;
};
