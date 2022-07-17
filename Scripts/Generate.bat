@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD "%~dp0"

IF "%SOURCE_PATH%" == "" (
	CALL Defines.bat %*
)

IF "%CLEAN%" == "TRUE" (
	CALL Clean.bat
)

SET CMAKE_OPTIONS=

IF NOT "%GENERATOR%" == "" (
	SET CMAKE_OPTIONS=-G %GENERATOR%
)

SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -S %SOURCE_PATH% -B %BUILD_PATH% -DCMAKE_BUILD_TYPE=%CONFIGURATION%
SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DTOOLS=%BUILD_TOOLS% -DWINDOWING=%WINDOWING% -DRENDERING=%RENDERING%

IF "%SDL2%" == "TRUE" (
	IF "%SDL2_MODULE_PATH%" == "" (
		ECHO SDL2_MODULE_PATH batch variable is empty. Please provide a valid path to this variable to continue project generation using the SDL2 library.
		EXIT -1
	)
	IF "%SDL2_DIR%" == "" (
		ECHO SDL2_DIR batch variable is empty. Please provide a valid path to this variable to continue project generation using the SDL2 library.
	)
	SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DCMAKE_MODULE_PATH=%SDL2_MODULE_PATH% -DSDL2_DIR=%SDL2_DIR%
	GOTO :GENERATE
)

IF "%SFML%" == "TRUE" (
	IF "%SFML_DIR%" == "" (
		ECHO SFML_DIR batch variable is empty. Please provide a valid path to this variable to continue project generation using the SFML library.
		EXIT -1
	)
	SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DSFML_DIR=%SFML_DIR%
	GOTO :GENERATE
)

:GENERATE
cmake %CMAKE_OPTIONS%

POPD
ENDLOCAL
