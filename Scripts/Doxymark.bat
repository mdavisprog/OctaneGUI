@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD "%~dp0"

SET MANIFEST_PATH=--manifest-path=..\Tools\doxymark\Cargo.toml
SET DOC_PATH=..\Documentation
SET XML_PATH=%DOC_PATH%\xml
SET CLEAN=FALSE

:PARSE_ARGS
IF NOT "%1" == "" (
    IF /I "%1" == "Clean" SET CLEAN=TRUE
    SHIFT
    GOTO :PARSE_ARGS
)

IF "%CLEAN%" == "TRUE" (
    ECHO Cleaning...
    cargo clean %MANIFEST_PATH%
)

cargo run %MANIFEST_PATH% -- --path %XML_PATH% --output-dir %DOC_PATH%

POPD
ENDLOCAL
