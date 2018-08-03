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

#graphicsmagick
if(NOT MSVC)
	message(STATUS "Preparing GraphicsMagick for NON MSVC build")
	include(ExternalProject)
	ExternalProject_Add(graphicsmagick
		PREFIX ${CMAKE_SOURCE_DIR}/dependencies/graphicsmagick
		HG_REPOSITORY http://hg.code.sf.net/p/graphicsmagick/code
		UPDATE_COMMAND ""
		CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/dependencies/graphicsmagick/src/graphicsmagick/configure --prefix=${CMAKE_SOURCE_DIR}/3rdParty/graphicsmagick/linux
		BUILD_COMMAND make all install
		INSTALL_COMMAND ""
		LOG_DOWNLOAD 1
		LOG_CONFIGURE 1
		LOG_BUILD 1
		LOG_INSTALL 1
	)
	ExternalProject_Get_Property(graphicsmagick SOURCE_DIR)

	set(GM_DIR ${CMAKE_SOURCE_DIR}/3rdParty/graphicsmagick/linux)

	#cppflags for GM
	execute_process(
		COMMAND ${GM_DIR}/bin/GraphicsMagick++-config --cppflags
		OUTPUT_VARIABLE GM_CPPFLAGS)
	add_compile_options(${GM_CPPFLAGS})
	
	#ldflags for GM
	execute_process(
		COMMAND ${GM_DIR}/bin/GraphicsMagick++-config --ldflags
		OUTPUT_VARIABLE GM_LDFLAGS)
	execute_process(
		COMMAND ${GM_DIR}/bin/GraphicsMagick++-config --libs
		OUTPUT_VARIABLE GM_LIBS)
	string(REGEX REPLACE "\n$" "" GM_LDFLAGS "${GM_LDFLAGS}")
	string(REGEX REPLACE "\n$" "" GM_LIBS "${GM_LIBS}")

	#remove imagemagic libs as we need static linking
	string(REPLACE "-lGraphicsMagick++ " "" GM_LIBS ${GM_LIBS})
	string(REPLACE "-lGraphicsMagick " "" GM_LIBS ${GM_LIBS})
	string(REPLACE " -l" ";" GM_LIBS ${GM_LIBS})

	link_directories(" ${GM_LDFLAGS}")
	link_libraries(${GM_DIR}/lib/libGraphicsMagick++.a ${GM_DIR}/lib/libGraphicsMagick.a ${GM_LIBS} rt)
else()
	message(STATUS "Preparing GraphicsMagick for MSVC build")
	execute_process(
		COMMAND "C:\\Program Files\\7-Zip\\7z.exe -e ${CMAKE_SOURCE_DIR}/3rdParty.zip"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

	if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(GM_DIR ${CMAKE_SOURCE_DIR}/3rdParty/graphicsmagick/windows/x64)
	else()
		set(GM_DIR ${CMAKE_SOURCE_DIR}/3rdParty/graphicsmagick/windows/x86)  
	endif()

	include_directories(${GM_DIR}/include)
endif()
