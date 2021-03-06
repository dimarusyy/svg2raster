// svg2raster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "command_line_args.h"
#include "image_converter.h"

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/locale.hpp>

namespace parser
{
	namespace x3 = boost::spirit::x3;
	struct attributes
	{
		std::string _protocol;
		std::string _domain;
		uint32_t _port{0};
		std::string _path;
		std::string _query;
	};

	static auto r_url = x3::rule<struct _, attributes>{ "attributes" };
	static auto r_url_def =
		*x3::char_("a-x") >> x3::lit("://") >> *(x3::char_ - x3::char_("/ :")) >> -(':' >> -x3::uint_parser<uint32_t>{})
		>> '/' >> *(x3::char_ - x3::char_("? #")) >> '?' >> *x3::char_;

	BOOST_SPIRIT_DEFINE(r_url)
}

BOOST_FUSION_ADAPT_STRUCT(parser::attributes,
(std::string, _protocol)(std::string, _domain)(uint32_t, _port)(std::string, _path)(std::string, _query))

namespace
{
	namespace x3 = boost::spirit::x3;
	void convert(const command_line::cmd_args_t& p_args)
	{
		parser::attributes url_attr;
		auto f = p_args._url.begin(), l = p_args._url.end();
		const auto ok = x3::parse(f, l, parser::r_url, url_attr);
		if (!ok)
		{
			std::stringstream ss;
			ss << "failed to parse url, remaining : [" << std::quoted(std::string{ f, l }) << "]";
			throw std::runtime_error(ss.str());
		}

		// get svg filename
		fs::path fs_uri_path(url_attr._path);
		const auto svg_filename = fs_uri_path.filename();

		//get full path to svg
		fs::path svg_path(p_args._svg_path);
		svg_path /= svg_filename;

		if (!fs::exists(svg_path))
		{
			throw std::runtime_error(std::string("file not found [") + svg_path.string() + "]");
		}

		auto png_path = svg_path;
		png_path.replace_extension(p_args._rtype);
		image_converter{ url_attr._query }.apply(svg_path.string(), png_path.string());
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
		std::cerr << "Exception : " << ex.what() << ".exit(0)." << std::endl;
		exit(0);
	}
	catch (...)
	{
		std::cerr << "Unexpected exception. exit(1)." << std::endl;
		exit(1);
	}
    return 0;
}

