@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD "%~dp0"

SET GENERATOR=Ninja

SET VCVARS=
CALL VCVars.bat %*

IF NOT EXIST "%VCVARS%" (
	ECHO VCVars batch file "%VCVARS%" does not exist!. A valid Visual Studio installation was not found. Please verify a valid Visual Studio install exists before attempting to call the VCVars batch file.
	EXIT -1
)

CALL "%VCVARS%"
CALL Generate.bat %*
CALL Defines.bat

ninja --version
ninja -C %BUILD_PATH%

POPD
ENDLOCAL
