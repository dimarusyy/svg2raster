#pragma once

#include <boost/program_options.hpp>

namespace command_line
{
	//////////////////////////////////////////////////////////////////////////

	struct cmd_args_t
	{
		std::string _url;
		std::string _rtype;
		std::string _svg_path;
	};

	cmd_args_t ParseArguments(int argc, char** argv);

	//////////////////////////////////////////////////////////////////////////
}
