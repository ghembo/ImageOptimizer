#include "stdafx.h"

#include "ImageOptimizer.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <sstream>
#include <iomanip>



int main(int argc, char* argv[])
{
	std::string input;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("input,i", po::value<std::string>(&input)->default_value("."), "image or folder to process");

	po::positional_options_description p;
	p.add("input", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	std::cout << ImageOptimizer::GetVersion() << std::endl;

	ImageOptimizer::EnableFileLogging();

	ImageOptimizer imageOptimizer;

	try
	{
		imageOptimizer.OptimizeFolderRecursive(input, 0.9999f);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error during optimization:" << std::endl;
		std::cout << e.what();
	}

	std::cout << "Done!";

	std::getchar();

    return 0;
}

