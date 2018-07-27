#pragma once

#include <boost/program_options.hpp>

namespace command_line
{
	//////////////////////////////////////////////////////////////////////////

	struct cmd_args_t
	{
		std::wstring _url;
		std::wstring _rtype;
		std::wstring _svg_path;
	};

	cmd_args_t ParseArguments(int argc, char** argv);

	//////////////////////////////////////////////////////////////////////////
}
