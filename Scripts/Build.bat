@ECHO OFF

CALL "%~dp0"\Defines.bat %*

SETLOCAL ENABLEDELAYEDEXPANSION
PUSHD "%~dp0"

CALL VCVars.bat %*

IF NOT EXIST "%VCVARS%" (
	ECHO VCVars batch file "%VCVARS%" does not exist!. A valid Visual Studio installation was not found. Please verify a valid Visual Studio install exists before attempting to call the VCVars batch file.
	EXIT -1
)

CALL "%VCVARS%"
CALL Generate.bat %*

IF "%NINJA%" == "TRUE" (
	ninja --version
	ninja -C %BUILD_PATH%
) ELSE (
	msbuild %BUILD_PATH%\OctaneGUI.sln /p:Configuration=%CONFIGURATION%
)

POPD
ENDLOCAL
