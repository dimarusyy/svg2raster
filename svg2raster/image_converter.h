#pragma once

#ifdef _DEBUG
#define BOOST_SPIRIT_X3_DEBUG
#endif

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <Magick++/Image.h>

#include <iomanip>

//////////////////////////////////////////////////////////////////////////

namespace converter
{
	namespace x3 = boost::spirit::x3;
	struct settings
	{
		std::string _converter;
		std::string _scale_type;
		uint32_t _x;
		uint32_t _y;
	};

	static auto r_parser = x3::rule<struct _, converter::settings>{ "processor_cfg" };
	static auto r_parser_def = +x3::char_("a-z") >> '&' >> +x3::char_("a-z") >> '=' >> x3::uint_ >> 'x' >> x3::uint_;

	BOOST_SPIRIT_DEFINE(r_parser)
};

BOOST_FUSION_ADAPT_STRUCT(converter::settings, (std::string, _converter)(std::string, _scale_type)(uint32_t, _x)(uint32_t, _y))

//////////////////////////////////////////////////////////////////////////

struct image_converter 
{
	image_converter(const std::string& img_path, const std::string& query_str)
		: _img_path(img_path)
	{
		namespace x3 = boost::spirit::x3;

		auto f = query_str.begin(), l = query_str.end();
		const auto ok = x3::parse(f, l, converter::r_parser , _cfg);
		if (!ok)
		{
			std::stringstream ss;
			ss << "failed to parse, remaining : [" << std::quoted(std::string{ f, l }) << "]";
			throw std::runtime_error(ss.str());
		}
	}

	void process()
	{

	}

private:
	std::string _img_path;
	converter::settings _cfg;
};

//////////////////////////////////////////////////////////////////////////
