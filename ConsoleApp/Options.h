#include <string>
#include <vector>

#include "cxxopts.hpp"

class Options
{
public:
	static Options parse(int argc, char* argv[])
	{
		cxxopts::Options options("ImageOptimizer", "Optimize Jpeg images size without quality loss");
		options.positional_help("Image or folder to process");
		options.show_positional_help();

		options.add_options()
			("h,help", "Print help")
			("i,input", "Image or folder to process", cxxopts::value<std::vector<std::string>>()->default_value("."))
			("r,recursive", "Recursive folder processing", cxxopts::value<bool>()->default_value("false"))
			("s,ssim", "Similarity score", cxxopts::value<float>()->default_value("0.9999"));

		options.parse_positional("input");

		Options option;

		try
		{
			auto result = options.parse(argc, argv);

			option.m_helpMessage = options.help();

			if (result.count("help"))
			{
				option.m_help = true;
			}

			option.m_recursive = result["recursive"].as<bool>();
			option.m_input = result["input"].as<std::vector<std::string>>();
			option.m_ssimScore = result["ssim"].as<float>();
		}
		catch (const cxxopts::OptionException& e)
		{
			throw std::exception(e.what());
		}

		return option;
	}

	bool help() const
	{
		return m_help;
	}

	bool recursive() const
	{
		return m_recursive;
	}

	std::vector<std::string> input() const
	{
		return m_input;
	}

	std::string helpMessage() const
	{
		return m_helpMessage;
	}

	float ssimScore() const
	{
		return m_ssimScore;
	}

private:
	Options() = default;

	std::vector<std::string> m_input;
	std::string m_helpMessage;
	float m_ssimScore;
	bool m_recursive;
	bool m_help;
};
