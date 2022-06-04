cmake_minimum_required(VERSION 3.21)

message("Building configuration: ${CMAKE_BUILD_TYPE}")

project(OctaneGUI)

set(CMAKE_CXX_STANDARD 17)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

# Default settings for each platform.
if(APPLE)
	set(WINDOWING SDL)
	set(RENDERING METAL)
else()
	set(WINDOWING SFML)
	set(RENDERING SFML)
endif()

message("Desired windowing library: ${WINDOWING}")
message("Desired rendering library: ${RENDERING}")

# Find the appropriate packages for each windowing/rendering libraries.
if(${WINDOWING} MATCHES SFML OR ${RENDERING} MATCHES SFML)
	message("Loading SFML package")
	find_package(SFML COMPONENTS system window graphics main REQUIRED)
endif()

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
endif()

set(FRONTEND_INCLUDE ${FRONTEND_INCLUDE} "${PROJECT_SOURCE_DIR}/Frontends")
set(OctaneGUI_INCLUDE "${PROJECT_SOURCE_DIR}/Source")

add_subdirectory(Source/OctaneGUI)
add_subdirectory(Frontends)
add_subdirectory(Apps)

unset(TOOLS CACHE)
