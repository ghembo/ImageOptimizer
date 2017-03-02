#include "stdafx.h"

#include "ImageProcessor.h"

#include <iostream>



int main()
{
	std::cout << ImageProcessor::GetVersion() << std::endl;

	ImageProcessor::EnableFileLogging(SeverityLevel::warning, "Global");

	ImageProcessor imageOptimizer;

	imageOptimizer.OptimizeImage("C:\\Proj\\ImageOptimizer\\ImageOptimizer\\test.jpg");

	std::getchar();

    return 0;
}

