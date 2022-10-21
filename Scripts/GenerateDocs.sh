#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"
pushd ..

doxygen Doxygen

popd
popd
