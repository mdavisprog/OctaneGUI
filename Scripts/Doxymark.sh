#!/bin/bash

MANIFEST_PATH="--manifest-path=../Tools/doxymark/Cargo.toml"
DOC_PATH=../Documentation
XML_PATH=$DOC_PATH/xml
CLEAN=false

for Var in "$@"
do
    Var=$(echo $Var | tr '[:upper:]' '[:lower:]')
    case ${Var} in
        clean) CLEAN=true ;;
        *) break
    esac
done

pushd "$(dirname "${BASH_SOURCE[0]}")"

if [ "$CLEAN" = true ] ; then
	echo Cleaning...
	cargo clean $MANIFEST_PATH
fi

cargo run $MANIFEST_PATH -- --path $XML_PATH --output-dir $DOC_PATH

popd
