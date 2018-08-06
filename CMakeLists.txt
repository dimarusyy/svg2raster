# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# required modules
include(ExternalProject)

include("cmake/paths.cmake")
include("cmake/dependencies.cmake")
include("cmake/flags.cmake")
include("cmake/graphicsmagick.cmake")

# do static msvc rutnime build
if(MSVC)
	configure_msvc_runtime()
endif()
#print_compile_flags()

# include root directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# sub-directories
add_subdirectory ("svg2raster")
add_subdirectory ("unit-tests")
