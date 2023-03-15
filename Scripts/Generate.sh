#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"

# If source path is not found, then the 'Defines.sh' may not have been run. Run it here.
if [[ -z $SOURCE_PATH ]] ; then
    source Defines.sh
fi

if [ "$CLEAN" = true ] ; then
    source Clean.sh
fi

if [[ ! -z $GENERATOR ]]; then
    CMAKE_OPTIONS="-G $GENERATOR"
fi

CMAKE_OPTIONS="$CMAKE_OPTIONS -S $SOURCE_PATH -B $BUILD_PATH -DCMAKE_BUILD_TYPE=$CONFIGURATION"
CMAKE_OPTIONS="$CMAKE_OPTIONS -DTOOLS=$TOOLS -DWINDOWING=$WINDOWING -DRENDERING=$RENDERING -DNO_APPS=$NO_APPS"
CMAKE_OPTIONS="$CMAKE_OPTIONS -DWITH_LSTALK=$WITH_LSTALK"

if [ "$SFML" = true ] ; then
    if [[ -z $SFML_DIR ]]; then
        echo "SFML_DIR variable is not defined. Please provide a valid path to the SFML installation before proceeding."
        exit -1
    fi
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DSFML_DIR=$SFML_DIR"
elif [ "$SDL2" = true ] ; then
    if [[ ! -z $SDL2_DIR ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DSDL2_DIR=$SDL2_DIR"
    fi
    if [[ ! -z $SDL2_MODULE_PATH ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_MODULE_PATH=$SDL2_MODULE_PATH"
    fi
fi

cmake $CMAKE_OPTIONS

popd
