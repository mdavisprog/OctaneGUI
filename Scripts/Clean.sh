#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"

if [[ -z $BUILD_PATH ]] ; then
	source Defines.sh
fi

if [[ -d "$BUILD_PATH" ]] ; then
	echo "Cleaning $BUILD_PATH"
	rm -rf $BUILD_PATH
fi

if [[ -d "$BIN_PATH" ]] ; then
	echo "Cleaning $BIN_PATH"
	rm -rf $BIN_PATH
fi

popd
