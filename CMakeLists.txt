cmake_minimum_required(VERSION 3.21)

message("Building configuration: ${CMAKE_BUILD_TYPE}")

project(OctaneGUI)

set(CMAKE_CXX_STANDARD 17)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

set(FRONTEND_INCLUDE ${FRONTEND_INCLUDE} "${PROJECT_SOURCE_DIR}/Frontends")
set(OctaneGUI_INCLUDE "${PROJECT_SOURCE_DIR}/Source")

add_subdirectory(Source/OctaneGUI)
add_subdirectory(Frontends)
add_subdirectory(Apps)

unset(TOOLS CACHE)
