#!/bin/bash

SDL2=true
SFML=false
NINJA=false
CONFIGURATION=Release
TOOLS=OFF
WINDOWING=SDL2
RENDERING=OpenGL
CLEAN=false
NO_APPS=OFF

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
        debug) CONFIGURATION=Debug ;;
        tools) TOOLS=ON ;;
        sfml) SFML=true ;;
		sdl2) SDL2=true ;;
		ninja) NINJA=true ;;
		clean) CLEAN=true ;;
		noapps) NO_APPS=ON ;;
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
