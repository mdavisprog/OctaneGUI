#!/bin/bash

SDL2=false
SFML=false
NINJA=false
CONFIGURATION=Release
TOOLS=OFF
WINDOWING=SDL2
RENDERING=OpenGL
CLEAN=false

BIN_PATH=../bin
BUILD_PATH=../Build
SOURCE_PATH=..

if [[ $OSTYPE == "darwin"* ]]; then
	RENDERING=Metal
fi

for Var in "$@"
do
    case ${Var,,} in
        debug) CONFIGURATION=Debug ;;
        tools) TOOLS=ON ;;
        sfml) SFML=true ;;
		sdl2) SDL2=true ;;
		ninja) NINJA=true ;;
		clean) CLEAN=true ;;
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
