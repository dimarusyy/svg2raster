#pragma once

#ifdef _DEBUG
#define BOOST_SPIRIT_X3_DEBUG
#endif

#include "ipc_cache.h"

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>

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
		uint32_t _scale_w;
		uint32_t _scale_h;
	};

	static auto r_parser = x3::rule<struct _, converter::settings>{ "settings" };
	static auto r_parser_def = +x3::char_("a-z") >> '&' >> +x3::char_("a-z") >> '=' >> x3::uint_ >> 'x' >> x3::uint_;

	BOOST_SPIRIT_DEFINE(r_parser)
};

BOOST_FUSION_ADAPT_STRUCT(converter::settings, (std::string, _converter)(std::string, _scale_type)(uint32_t, _scale_w)(uint32_t, _scale_h))

//////////////////////////////////////////////////////////////////////////

struct image_converter
{
	image_converter(const std::string& query_str)
	{
		namespace x3 = boost::spirit::x3;

		auto f = query_str.begin(), l = query_str.end();
		const auto ok = x3::parse(f, l, converter::r_parser, _cfg);
		if (!ok)
		{
			std::stringstream ss;
			ss << "failed to parse, remaining : [" << std::quoted(std::string{ f, l }) << "]";
			throw std::runtime_error(ss.str());
		}
	}

	void apply(const std::string& img_path, const std::string& img_out_path)
	{
		namespace fs = boost::filesystem;

		const auto cached_info = _cache.get(img_path);
		if (cached_info != boost::none)
		{
			if (fs::exists(cached_info.get()) && cached_info.get() == img_out_path)
			{
				std::cout << ">> no conversion, using cached value\n";
				return;
			}
		}

		const auto apply_mutex_name = (fs::path(img_path).filename().string() + ".lock");
		boost::interprocess::named_recursive_mutex apply_mutex(boost::interprocess::open_or_create,
															   apply_mutex_name.c_str(),
															   details::default_ipc_permissions());
		boost::interprocess::scoped_lock<boost::interprocess::named_recursive_mutex> lock(apply_mutex);
		std::cout << ">> invoked image conversion\n";

		Magick::Image image{};

		// Determine if Warning exceptions are thrown.
		// Use is optional.  Set to true to block Warning exceptions.
		image.quiet(false);
		image.read(img_path);
		image.scale(Magick::Geometry(_cfg._scale_w, _cfg._scale_h));

		//output
		image.write(img_out_path);

		std::cout << ">> cached [" << img_path << ", " << img_out_path << "]\n";
		_cache.put(img_path, img_out_path);

		std::cout << ">> done\n";

	}

private:
	converter::settings _cfg{};
	ipc_cache_t<1000> _cache{ "svg2raster" };
};

//////////////////////////////////////////////////////////////////////////
