set(GM_PREFIX graphicsmagick)

set(GM_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/graphicsmagick/${OS_PREFIX})
message(STATUS "GraphicsMagick path : ${GM_INSTALL_DIR}")

if(NOT MSVC)
	message(STATUS "Preparing GraphicsMagick for NON MSVC build")
	if(NOT EXISTS ${GM_INSTALL_DIR})
		ExternalProject_Add(${GM_PREFIX}
			PREFIX ${GM_PREFIX}
			HG_REPOSITORY http://hg.code.sf.net/p/graphicsmagick/code
			UPDATE_COMMAND ""
			CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${GM_PREFIX}/src/${GM_PREFIX}/configure --prefix=${GM_INSTALL_DIR}
			BUILD_COMMAND make all install
			BUILD_IN_SOURCE 1
			INSTALL_COMMAND ""
		)
	endif()

	#cppflags for GM
	execute_process(
		COMMAND ${GM_INSTALL_DIR}/bin/GraphicsMagick++-config --cppflags
		OUTPUT_VARIABLE GM_INCLUDE_DIRS)

	set(GM_INCLUDE_DIRS ${GM_INSTALL_DIR}/include/GraphicsMagick)

	#ldflags for GM
	execute_process(
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/graphicsmagick.sh ${GM_INSTALL_DIR}/bin/GraphicsMagick++-config --ldflags
		OUTPUT_VARIABLE GM_LIBRARY_DIRS)

	execute_process(
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/graphicsmagick.sh ${GM_INSTALL_DIR}/bin/GraphicsMagick++-config --libs
		OUTPUT_VARIABLE GM_LIBS)
	set(GM_LIBS ${GM_INSTALL_DIR}/lib/libGraphicsMagick++.a ${GM_INSTALL_DIR}/lib/libGraphicsMagick.a ${GM_LIBS} rt)
else()
	message(STATUS "Preparing GraphicsMagick for MSVC build")
	execute_process(
		COMMAND cmd /c C:/Program\ Files/7-Zip/7z.exe x "${CMAKE_SOURCE_DIR}/3rdParty.zip" -y > nul
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GM_EXTRACTING_STATUS
		OUTPUT_VARIABLE GM_EXTRACTING_OUTPUT)

	message(STATUS "Extracting GraphicksMagick : ${GM_EXTRACTING_STATUS} [${CMAKE_SOURCE_DIR}/3rdParty.zip]")

	set(GM_INCLUDE_DIRS ${GM_INSTALL_DIR}/include)
	include_directories(${GM_INCLUDE_DIRS})

	set(GM_LIBRARY_DIRS ${GM_INSTALL_DIR}/lib)
	link_directories(${GM_LIBRARY_DIRS})
endif()
