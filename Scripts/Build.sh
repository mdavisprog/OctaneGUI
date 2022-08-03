#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"

source Defines.sh
source Generate.sh

if [ "$NINJA" = true ] ; then
    NINJA_VERSION=$(ninja --version)
    echo "Using ninja version $NINJA_VERSION"
    ninja -C $BUILD_PATH
else
    make -C $BUILD_PATH
fi

popd
