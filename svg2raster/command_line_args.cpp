#include "stdafx.h"
#include "command_line_args.h"

#include <iostream>

namespace command_line
{
	namespace po = boost::program_options;

	cmd_args_t ParseArguments(int argc, char **argv)
	{
		cmd_args_t ret;
		po::variables_map vm;
		po::options_description config("Program Usage");
		try
		{
			config.add_options()
				("help,h", "produce help message")
				("url,u", po::value<std::string>(&ret._url)->required(), "[string] - <url> to be processed")
				("format,f", po::value<std::string>(&ret._rtype)->required(), "[string: png|jpeg] - output file format")
				("svg_path,p", po::value<std::string>(&ret._svg_path)->required(), "[string] - path to svg files")
				;

			po::store(po::parse_command_line(argc, argv, config), vm);

			if (vm.count("help"))
			{
				std::cout << config << "\n";
				exit(0);
			}

			po::notify(vm);
		}
		catch (const boost::program_options::required_option&)
		{
			std::cout << config << "\n";
			throw std::runtime_error("missing required arguments");
		}
		catch (const std::exception& ex)
		{
			std::cerr << "exception : " << ex.what() << std::endl;
			throw;
		}
		return ret;
	}
}