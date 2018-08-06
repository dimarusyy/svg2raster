cmake_minimum_required (VERSION 3.10)

include_directories(${GM_INCLUDE_DIRS})
link_directories(${GM_LIBRARY_DIRS})
if(NOT MSVC)
	link_libraries(${GM_LIBS} pthread rt)
endif()

# Add source to this project's executable.
set(SOURCE_FILES command_line_args.cpp  stdafx.cpp  svg2raster.cpp)
set(HEADER_FILES command_line_args.h  image_converter.h  ipc_cache.h  stdafx.h  targetver.h)
add_executable (svg2raster ${SOURCE_FILES} ${HEADER_FILES})


