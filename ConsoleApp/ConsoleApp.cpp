#include "stdafx.h"

#include "ImageOptimizer.h"

#include <iostream>



int main()
{
	std::cout << ImageOptimizer::GetVersion() << std::endl;

	ImageOptimizer::EnableFileLogging();

	ImageOptimizer imageOptimizer;

	imageOptimizer.OptimizeImage("C:\\Proj\\ImageOptimizer\\ImageOptimizer\\test.jpg");

	std::getchar();

    return 0;
}

