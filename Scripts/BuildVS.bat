@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION
PUSHD "%~dp0"

SET VCVARS=
CALL VCVars.bat %*

IF NOT EXIST "%VCVARS%" (
	ECHO VCVars batch file "%VCVARS%" does not exist!. A valid Visual Studio installation was not found. Please verify a valid Visual Studio install exists before attempting to call the VCVars batch file.
	EXIT -1
)

CALL "%VCVARS%"
CALL Generate.bat %*
CALL Defines.bat

SET CONFIGURATION=Release
:PARSE_ARGS
IF NOT "%1" == "" (
	IF /I "%1" == "Debug" SET CONFIGURATION=Debug
	SHIFT
	GOTO :PARSE_ARGS
)

msbuild %BUILD_PATH%\OctaneGUI.sln /p:Configuration=%CONFIGURATION%

POPD
ENDLOCAL
