// svg2raster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "command_line_args.h"
#include "image_converter.h"
#include <cpprest/base_uri.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/locale.hpp>

namespace
{
	void convert(const command_line::cmd_args_t& p_args)
	{
		// create/parse uri
		web::uri uri(p_args._url);

		// get svg filename
		fs::path fs_uri_path{ uri.path() };
		const auto svg_filename = fs_uri_path.filename();

		//get full path to svg
		fs::path svg_path{p_args._svg_path};
		svg_path /= svg_filename;

		if (!fs::exists(svg_path))
		{
			throw std::runtime_error(std::string("file not found [") + svg_path.string() + "]");
		}

#if _WIN32
		// TODO : avoid wstring->string conversion using another URL parser (folly ? beast ?)
		const auto uri_query_str = 
			boost::locale::conv::utf_to_utf<std::string::value_type, std::wstring::value_type>(uri.query().c_str(), boost::locale::conv::stop);
#else
		const auto uri_query_str = uri.query();
#endif
		auto png_path = svg_path;
		png_path.replace_extension(p_args._rtype);
		image_converter{ uri_query_str }.apply(svg_path.string(), png_path.string());
 	}
}

int main(int argc, char **argv)
{
	try
	{
		Magick::InitializeMagick(*argv);

		// parse arguments
		const auto p_args = command_line::ParseArguments(argc, argv);
		convert(p_args);
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

