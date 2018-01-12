#include "stdafx.h"

#include "ImageOptimizer.h"

#include "Options.h"

#include <iostream>
#include <chrono>
#include <filesystem>
#include <numeric>
#include <regex>

namespace fs = std::experimental::filesystem;


void displayResults(std::vector<OptimizationResult> results)
{
	constexpr int MB = 1024 * 1024;

	auto result = std::accumulate(results.begin(), results.end(), OptimizationResult());

	std::cout << "Original size: " << result.GetOriginalSize() / MB << " MB" <<
		" Compressed size: " << result.GetCompressedSize() / MB << " MB" <<
		" Compression: " << result.GetCompressionPercentage() << "%" <<
		" Saved: " << (result.GetOriginalSize() - result.GetCompressedSize()) / MB << "MB" << std::endl;
}

Options parseInput(int argc, char* argv[])
{
	try
	{
		return Options::parse(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error parsing options: " << e.what() << std::endl;

		exit(1);
	}
}

OptimizationResult processFileOrFolder(const std::string& input, float targetSimilarity, bool recursive)
{
	ImageOptimizer imageOptimizer;

	if (fs::is_regular_file(input))
	{
		return imageOptimizer.OptimizeImage(input, targetSimilarity);
	}
	else if (fs::is_directory(input))
	{
		if (recursive)
		{
			return imageOptimizer.OptimizeFolderRecursive(input, targetSimilarity);
		}
		else
		{
			return imageOptimizer.OptimizeFolder(input, targetSimilarity);
		}
	}
	else
	{
		throw std::exception("Input isn't a regular file or directory");
	}
}

std::string rtrim(std::string input)
{
	input.erase(std::find_if(input.rbegin(), input.rend(), [](int ch) { return !std::isspace(ch); }).base(), input.end());

	return input;
}

int main(int argc, char* argv[])
{
	auto options = parseInput(argc, argv);

	if (options.help())
	{
		std::cout << options.helpMessage() << std::endl;

		return 0;
	}

	for (const auto& input : options.input())
	{
		if (!fs::exists(input))
		{
			std::cout << "Input file or folder " + input + " doesn't exist" << std::endl;

			return 1;
		}
		else if (fs::is_regular_file(input))
		{
			const std::regex jpegExtension(R"(\.jpe?g$)", std::regex_constants::icase);

			if (!std::regex_match(rtrim(input), jpegExtension))
			{
				std::cout << "Input file " + input + " is not a Jpeg" << std::endl;

				return 1;
			}
		}
		else if (!fs::is_directory(input))
		{
			std::cout << "Input file or folder " + input + " isn't a regular file or directory" << std::endl;

			return 1;
		}
	}	

	constexpr float targetSimilarity = 0.9999f;

	std::cout << ImageOptimizer::GetVersion() << std::endl;

	ImageOptimizer::EnableFileLogging();

	std::vector<OptimizationResult> results;

	auto start = std::chrono::steady_clock::now();

	try
	{
		for (const auto& input : options.input())
		{
			results.push_back(processFileOrFolder(input, targetSimilarity, options.recursive()));
		}		
	}
	catch (const std::exception& e)
	{
		std::cout << "Error during optimization:" << std::endl << e.what() << std::endl;
	}

	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	std::cout << "Done! Processing time: " << duration << "ms" << std::endl;

	displayResults(results);

	std::getchar();

    return 0;
}

