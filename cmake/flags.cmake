macro(configure_msvc_runtime)
	if(MSVC)
		# Default to statically-linked runtime.
		if("${MSVC_RUNTIME}" STREQUAL "")
			set(MSVC_RUNTIME "static")
		endif()
		# Set compiler options.
		set(variables
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_MINSIZEREL
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_RELWITHDEBINFO
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_MINSIZEREL
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_RELWITHDEBINFO
		)
		if(${MSVC_RUNTIME} STREQUAL "static")
			message(STATUS "MSVC -> forcing use of statically-linked runtime.")
			foreach(variable ${variables})
				if(${variable} MATCHES "/MD")
					string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
				endif()
			endforeach()
		else()
		message(STATUS "MSVC -> forcing use of dynamically-linked runtime.")
		foreach(variable ${variables})
			if(${variable} MATCHES "/MT")
				string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
			endif()
		endforeach()
		endif()
	endif()
endmacro()

macro(print_compile_flags)
	set(variables
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
	)
	message(STATUS "Initial build flags:")
	foreach(variable ${variables})
		message(STATUS "  '${variable}': ${${variable}}")
	endforeach()
	message(STATUS "")
endmacro()

#set_target_properties(${TARGET} PROPERTIES
#    CXX_STANDARD 14
#    CXX_STANDARD_REQUIRED YES
#    CXX_EXTENSIONS NO
#)
if(NOT MSVC)
    SET(CMAKE_CXX_FLAGS "-Wall -std=c++14 ${CMAKE_CXX_FLAGS}")
endif()

if(WIN32)
	if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(OS_PREFIX windows.x86)
	else()
		set(OS_PREFIX windows.x64)
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OS_PREFIX linux.x64)
	else()
		set(OS_PREFIX linux.x86)
	endif()
endif()