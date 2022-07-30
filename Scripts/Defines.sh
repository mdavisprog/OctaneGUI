#!/bin/bash

SDL2=true
SFML=false
NINJA=false
CONFIGURATION=Debug
TOOLS=OFF
WINDOWING=SDL2
RENDERING=OpenGL
CLEAN=false
NO_APPS=OFF
HELP=false

BIN_PATH=../bin
BUILD_PATH=../Build
SOURCE_PATH=..

if [[ $OSTYPE == "darwin"* ]]; then
	RENDERING=Metal
fi

for Var in "$@"
do
	Var=$(echo $Var | tr '[:upper:]' '[:lower:]')
	case ${Var} in
        release) CONFIGURATION=Release ;;
        tools) TOOLS=ON ;;
        sfml) SFML=true ;;
		sdl2) SDL2=true ;;
		ninja) NINJA=true ;;
		clean) CLEAN=true ;;
		noapps) NO_APPS=ON ;;
		help) HELP=true ;;
        *) break
    esac
done

if [ "$SDL2" = true ] ; then
	WINDOWING=SDL2
elif [ "$SFML" = true ] ; then
	WINDOWING=SFML
	RENDERING=SFML
fi

if [ "$NINJA" = true ] ; then
	GENERATOR=Ninja
fi

if [ "$HELP" = true ] ; then
	echo "This is a list of arguments and a description for each:"
	echo "release        Sets the configuration to Release. The default is Debug."
	echo "tools          Compiles with tools enabled. This can be compiled in any configuration."
	echo "ninja          Use the ninja build system instead of the default (make). The path"
	echo "               to the ninja build system must be added to the PATH environment variable."
	echo "clean          Cleans the intermediate files before generating and building. This forces"
	echo "               a full rebuild of the project."
	echo "noapps         Only compiles the library."
	echo "sfml           Builds the apps using the SFML library. The SFML_DIR variable must be set"
	echo "               for the generator to locate the library."
	echo "sdl2           Builds the app using the SDL library. The SDL2 cmake and library paths must"
	echo "               be locatable by the generator through either the environment variables or the "
	echo "               SDL2_DIR and SDL2_MODULE_PATH variables."
	echo "help           Displays this help message."
	exit 0
fi
