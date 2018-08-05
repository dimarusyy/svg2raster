#dependencies

#top
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#boost
find_package(Boost REQUIRED system filesystem date_time unit_test_framework program_options)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_libraries(${Boost_LIBRARIES})
	message(STATUS "Found boost v.${Boost_VERSION}: [${Boost_INCLUDE_DIRS}] [${Boost_LIBRARIES}]")
else()
	message(FATAL_ERROR "Could not find boost")
endif()
