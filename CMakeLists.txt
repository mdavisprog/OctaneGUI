cmake_minimum_required(VERSION 3.21)

message("Building configuration: ${CMAKE_BUILD_TYPE}")

project(OctaneGUI)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(CMAKE_CXX_STANDARD 17)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

set(FRONTEND_INCLUDE ${FRONTEND_INCLUDE} "${PROJECT_SOURCE_DIR}/Frontends")
set(OctaneGUI_INCLUDE "${PROJECT_SOURCE_DIR}/Source")


include(ExternalProject)

# Build SFML
set(SFML_INSTALL_DIR ${CMAKE_BINARY_DIR}/third_party/SFML)
set(SFML_INCLUDE_DIR ${SFML_INSTALL_DIR}/include)
set(SFML_LIB_DIR ${SFML_INSTALL_DIR}/lib)
ExternalProject_Add(SFML
					#GIT_REPOSITORY "https://github.com/SFML/SFML.git"
					#GIT_TAG "2.6.x"
                    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/third_party/SFML
                    PREFIX      ${SFML_INSTALL_DIR}
                    INSTALL_DIR ${SFML_INSTALL_DIR}
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${SFML_INSTALL_DIR}
                                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                -DCMAKE_CXX_FLAGS=${EXTERNAL_PROJECT_CMAKE_CXX_FLAGS}
                   )
include_directories(BEFORE SYSTEM ${SFML_INCLUDE_DIR})
link_directories(${SFML_LIB_DIR})
set(SFML_DIR ${SFML_INSTALL_DIR})
set(SFML_DOC_DIR ${SFML_INSTALL_DIR}/doc)


if(TOOLS)
    message("Building Tools...")
    add_compile_definitions(TOOLS=1)
endif()

add_subdirectory(Source/OctaneGUI)
add_subdirectory(Frontends)
add_subdirectory(Apps)

unset(TOOLS CACHE)
