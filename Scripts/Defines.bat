@ECHO OFF

SET SDL2=FALSE
SET SFML=TRUE
SET NINJA=FALSE
SET CONFIGURATION=Debug
SET BUILD_TOOLS=OFF
SET CLEAN=FALSE
SET WINDOWING=SFML
SET RENDERING=SFML
SET NO_APPS=OFF
SET HELP=FALSE

SET BIN_PATH=..\bin
SET BUILD_PATH=..\Build
SET SOURCE_PATH=..

:PARSE_ARGS
IF NOT "%1" == "" (
	IF /I "%1" == "Release" SET CONFIGURATION=Release
	IF /I "%1" == "Tools" SET BUILD_TOOLS=ON
	IF /I "%1" == "SDL2" SET SDL2=TRUE
	IF /I "%1" == "SFML" SET SFML=TRUE
	IF /I "%1" == "NINJA" SET NINJA=TRUE
	IF /I "%1" == "CLEAN" SET CLEAN=TRUE
	IF /I "%1" == "NOAPPS" SET NO_APPS=ON
	IF /I "%1" == "HELP" SET HELP=TRUE
	SHIFT
	GOTO :PARSE_ARGS
)

IF "%SDL2%" == "TRUE" (
	SET WINDOWING=SDL2
	SET RENDERING=OpenGL
	SET SFML=FALSE
)

IF "%SFML%" == "TRUE" (
	SET WINDOWING=SFML
	SET RENDERING=SFML
	SET SDL2=FALSE
)

IF "%NINJA%" == "TRUE" (
	SET GENERATOR=Ninja
)

IF "%HELP%" == "TRUE" (
	ECHO This is a list of arguments and a description for each:
	ECHO release        Sets the configuration to Release. The default is Debug.
	ECHO tools          Compiles with tools enabled. This can be compiled in any configuration.
	ECHO ninja          Use the ninja build system instead of the default (msbuild^). The path
	ECHO                to the ninja build system must be added to the PATH environment variable.
	ECHO clean          Cleans the intermediate files before generating and building. This forces
	ECHO                a full rebuild of the project.
	ECHO noapps         Only compiles the library.
	ECHO sfml           Builds the apps using the SFML library. The SFML_DIR variable must be set
	ECHO                for the generator to locate the library.
	ECHO sdl2           Builds the app using the SDL library. The SDL2 cmake and library paths must
	ECHO                be locatable by the generator through either the environment variables or the 
	ECHO                SDL2_DIR and SDL2_MODULE_PATH variables.
	ECHO help           Displays this help message.
	EXIT 0
)
