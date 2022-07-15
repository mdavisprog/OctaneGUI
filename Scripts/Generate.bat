@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD "%~dp0"

CALL Defines.bat

SET CONFIGURATION=Release
SET BUILD_TOOLS=OFF
SET SDL2=FALSE

:PARSE_ARGS
IF NOT "%1" == "" (
	IF /I "%1" == "Debug" SET CONFIGURATION=Debug
	IF /I "%1" == "Tools" SET BUILD_TOOLS=ON
	IF /I "%1" == "SDL2" SET SDL2=TRUE
	SHIFT
	GOTO :PARSE_ARGS
)

SET CMAKE_OPTIONS=

IF NOT "%GENERATOR%" == "" (
	SET CMAKE_OPTIONS=-G %GENERATOR%
)

SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -S %SOURCE_PATH% -B %BUILD_PATH% -DCMAKE_BUILD_TYPE=%CONFIGURATION%
SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DTOOLS=%BUILD_TOOLS%

IF "%SDL2%" == "TRUE" (
	IF "%SDL2_MODULE_PATH%" == "" (
		ECHO SDL2_MODULE_PATH batch variable is empty. Please provide a valid path to this variable to continue project generation using the SDL2 library.
		EXIT -1
	)
	IF "%SDL2_DIR%" == "" (
		ECHO SDL2_DIR batch variable is empty. Please provide a valid path to this variable to continue project generation using the SDL2 library.
	)
	SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DWINDOWING=SDL2 -DRENDERING=OpenGL -DCMAKE_MODULE_PATH=%SDL2_MODULE_PATH% -DSDL2_DIR=%SDL2_DIR%
) ELSE (
	IF "%SFML_DIR%" == "" (
		ECHO SFML_DIR batch variable is empty. Please provide a valid path to this variable to continue project generation using the SFML library.
		EXIT -1
	)
	SET CMAKE_OPTIONS=%CMAKE_OPTIONS% -DSFML_DIR=%SFML_DIR%
)

cmake %CMAKE_OPTIONS%

POPD
ENDLOCAL
