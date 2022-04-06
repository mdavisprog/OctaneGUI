cmake_minimum_required(VERSION 3.21)

message("Building configuration: ${CMAKE_BUILD_TYPE}")

project(OctaneGUI)

set(CMAKE_CXX_STANDARD 17)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

if(APPLE)
	set(SDL_INCLUDE "${PROJECT_SOURCE_DIR}/lib/SDL2.framework/Headers")
	find_library(
		SDL_LIBS
		NAME SDL2
		PATHS "${PROJECT_SOURCE_DIR}/lib"
	)

	set(FRONTEND_INCLUDE ${SDL_INCLUDE})
	set(FRONTEND_BIN_PATH ${SDL_LIBS}/..)
	set(FRONTEND_LIBS SDL_Frontend ${SDL_LIBS})
else()
	set(SFML_INCLUDE "${PROJECT_SOURCE_DIR}/lib/SFML/include")
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		message(FATAL_ERROR, "32-bit SFML libraries not supported.")
	else()
		if(WIN32)
			set(SFML_BIN_PATH "${PROJECT_SOURCE_DIR}/lib/SFML/bin/x64")
			set(SFML_LIBS_PATH "${PROJECT_SOURCE_DIR}/lib/SFML/lib/x64")
		else()
			set(SFML_BIN_PATH "${PROJECT_SOURCE_DIR}/lib/SFML/lib")
			set(SFML_LIBS_PATH "${PROJECT_SOURCE_DIR}/lib/SFML/lib")
		endif()
	endif()
	set(
		SFML_LIBS 
		sfml-graphics
		sfml-network
		sfml-system
		sfml-window
	)
	if(WIN32)
		set(
			SFML_LIBS
			${SFML_LIBS}
			sfml-main
		)
	endif()

	set(FRONTEND_INCLUDE ${SFML_INCLUDE})
	set(FRONTEND_LIBS_PATH ${SFML_LIBS_PATH})
	set(FRONTEND_BIN_PATH ${SFML_BIN_PATH})
	set(FRONTEND_LIBS SFML_Frontend ${SFML_LIBS})
endif()

set(FRONTEND_INCLUDE ${FRONTEND_INCLUDE} "${PROJECT_SOURCE_DIR}/Frontends")
set(OctaneGUI_INCLUDE "${PROJECT_SOURCE_DIR}/Source")

add_subdirectory(Source/OctaneGUI)
add_subdirectory(Frontends)
add_subdirectory(Apps)

unset(TOOLS CACHE)
