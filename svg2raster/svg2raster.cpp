// svg2raster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "command_line_args.h"
#include <cpprest/base_uri.h>

int main(int argc, char **argv)
{
	try
	{
		// parse arguments
		const auto p_args = command_line::ParseArguments(argc, argv);

		// create/parse uri
		web::uri uri(p_args._url);
	}
	catch (std::exception& ex)
	{
		std::cerr << "exception : " << ex.what() << std::endl;
		exit(0);
	}
	catch (...)
	{
		std::cerr << "unexpected exception" << std::endl;
		exit(1);
	}
    return 0;
}

