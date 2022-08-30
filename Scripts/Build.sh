#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"

source Defines.sh
source Generate.sh

if [ "$NINJA" = true ] ; then
    NINJA_VERSION=$(ninja --version)
    echo "Using ninja version $NINJA_VERSION"
    ninja -C $BUILD_PATH
elif [ "$XCODE" = true ] ; then
    xcodebuild -configuration $CONFIGURATION -scheme ALL_BUILD -project "$BUILD_PATH/OctaneGUI.xcodeproj"
else
    make -C $BUILD_PATH
fi

popd
