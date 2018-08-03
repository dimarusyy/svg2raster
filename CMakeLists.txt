# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

include("cmake/paths.cmake")
project ("svg2raster")

include("cmake/dependencies.cmake")
include("cmake/flags.cmake")

if(MSVC)
	configure_msvc_runtime()
endif()

print_compile_flags()


# Include sub-projects.
add_subdirectory ("svg2raster")
add_subdirectory ("unit-tests")

#dependencies
if(NOT MSVC)
	add_dependencies(svg2raster graphicsmagick)
endif()