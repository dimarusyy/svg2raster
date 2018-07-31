#include "stdafx.h"

#include <svg2raster/ipc_cache.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cache_t_tests)

BOOST_AUTO_TEST_CASE(simple_test)
{
	cache_t<int, std::string, 2> cache;

	std::string s;
	auto rc = cache.get(1, s);
	BOOST_ASSERT_MSG(rc == false, "wrong value returned : should be false");
	BOOST_ASSERT_MSG(s.empty(), "value should be empty");

	cache.put(1, "hello1");
	rc = cache.get(1, s);
	BOOST_ASSERT_MSG(rc == true, "item not cached");
	BOOST_ASSERT_MSG(s == "hello1", "wrong value returned");

	cache.put(2, "hello2");
	rc = cache.get(2, s);
	BOOST_ASSERT_MSG(rc == true, "item not cached");
	BOOST_ASSERT_MSG(s == "hello2", "wrong value returned");

	cache.put(3, "hello3");
	rc = cache.get(3, s);
	BOOST_ASSERT_MSG(rc == true, "item not cached");
	BOOST_ASSERT_MSG(s == "hello3", "wrong value returned");

	rc = cache.get(1, s);
	BOOST_ASSERT_MSG(rc == false, "wrong value returned : should be false");
}

BOOST_AUTO_TEST_SUITE_END()
