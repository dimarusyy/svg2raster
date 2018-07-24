// svg2raster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "command_line_args.h"
#include <cpprest/base_uri.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <Magick++/Magick++.h>

namespace
{
	bool convert(const command_line::cmd_args_t& p_args)
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

		const auto convert_to = [&svg_path](const fs::path& path, command_line::cmd_args_t::raster_t rtype)
		{
			bool rc = true;
			try
			{
				Magick::Image image;

				// Determine if Warning exceptions are thrown.
				// Use is optional.  Set to true to block Warning exceptions.
				image.quiet(true);

				// Read a file into image object
				image.read(svg_path.string());

				// Crop the image to specified size (width, height, xOffset, yOffset)
				image.crop(Magick::Geometry(20, 20, 20, 20));

				// Write the image to a file
				image.write("E:\\beacon.png");
			}
			catch (Magick::Exception &ex)
			{
				std::cout << "exception: " << ex.what() << std::endl;
				rc = false;
			}
			return rc;
		};

		const auto rc = convert_to(svg_path, p_args._rtype);
		return rc;
	}
}

int main(int argc, char **argv)
{
	try
	{
		Magick::InitializeMagick(*argv);

		// parse arguments
		const auto p_args = command_line::ParseArguments(argc, argv);
		const auto rc = convert(p_args);
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

