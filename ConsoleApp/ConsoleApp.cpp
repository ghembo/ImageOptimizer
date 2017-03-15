#include "stdafx.h"

#include "ImageOptimizer.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace po = boost::program_options;
namespace fs = boost::filesystem;



int main(int argc, char* argv[])
{
	std::string input;
	bool recursive;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("input,i", po::value<std::string>(&input)->default_value("."), "image or folder to process")
		("recursive,r", po::bool_switch(&recursive)->default_value(false), "recursive folder processing");

	po::positional_options_description p;
	p.add("input", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (!fs::exists(input))
	{
		std::cout << "Input file or folder doesn't exist" << std::endl;

		return 1;
	}

	constexpr float targetSimilarity = 0.9999f;

	std::cout << ImageOptimizer::GetVersion() << std::endl;

	ImageOptimizer::EnableFileLogging();

	ImageOptimizer imageOptimizer;

	OptimizationResult result;

	auto start = std::chrono::steady_clock::now();

	try
	{
		if (fs::is_regular_file(input))
		{
			result = imageOptimizer.OptimizeImage(input, targetSimilarity);
		}
		else if (fs::is_directory(input))
		{
			if (recursive)
			{
				result = imageOptimizer.OptimizeFolderRecursive(input, targetSimilarity);
			}
			else
			{
				result = imageOptimizer.OptimizeFolder(input, targetSimilarity);
			}
		}
		else
		{
			std::cout << "Input isn't a regular file or directory" << std::endl;

			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error during optimization:" << std::endl;
		std::cout << e.what();
	}

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	std::cout << "Done! Processing time: " << duration << "ms" << std::endl;

	constexpr int MB = 1024 * 1024;

	std::cout << "Original size: " << result.GetOriginalSize() / MB << " MB" <<
				" Compressed size: " << result.GetCompressedSize() / MB << " MB" <<
				" Compression: " << result.GetCompressionPercentage() << "%" << std::endl;

	std::getchar();

    return 0;
}

