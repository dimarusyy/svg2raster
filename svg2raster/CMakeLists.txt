cmake_minimum_required (VERSION 3.10)

# Add source to this project's executable.
set(SOURCE_FILES command_line_args.cpp  stdafx.cpp  svg2raster.cpp)
set(HEADER_FILES command_line_args.h  image_converter.h  ipc_cache.h  stdafx.h  targetver.h)
add_executable (svg2raster ${SOURCE_FILES} ${HEADER_FILES})

# GM
add_compile_options(${GM_INCLUDE_DIRS})
link_directories(${GM_LIBRARY_DIRS})
link_libraries(${GM_LIBS})


