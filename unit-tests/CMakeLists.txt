cmake_minimum_required (VERSION 3.10)

# Add source to this project's executable.
set(SOURCE_FILES stdafx.cpp unit-tests.cpp)
set(HEADER_FILES stdafx.h  targetver.h)
add_executable (unit-tests ${SOURCE_FILES} ${HEADER_FILES})


