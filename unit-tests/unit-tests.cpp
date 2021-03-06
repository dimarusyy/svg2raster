#include "stdafx.h"

#include <svg2raster/ipc_cache.h>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <chrono>
#include <future>

BOOST_AUTO_TEST_SUITE(cache_t_tests)

BOOST_AUTO_TEST_CASE(ipc_cache_test_1)
{
	auto rt = []()
	{
		for (int i = 0; i < 1; i++)
		{
			ipc_cache_t<2> cache{ "ipc_cache_test_1.shared.memory" };

			cache.put("path1", "hello1");
			cache.put("path2", "hello2");

			auto s = cache.get("path1");
			BOOST_ASSERT_MSG(s.get() == "hello1", "wrong value returned");

			s = cache.get("path2");
			BOOST_ASSERT_MSG(s.get() == "hello2", "wrong value returned");

			s = cache.get("path3");
			BOOST_ASSERT_MSG(s == boost::none, "value should be empty");

			cache.put("path2", "hello22");
			s = cache.get("path2");
			BOOST_ASSERT_MSG(s.get() == "hello22", "value should be empty");

// 			std::cout << "thread [" << std::this_thread::get_id() << "], iteration [" << i << "]" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	};

	rt();

	auto f1 = std::async(std::launch::async, rt);
	auto f2 = std::async(std::launch::async, rt);

	f1.wait();
	f2.wait();
}

BOOST_AUTO_TEST_SUITE_END()
