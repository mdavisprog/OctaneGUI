# OctaneGUI
Latest update: [November 2022](https://github.com/mdavisprog/OctaneGUI/wiki/November22)

<img src=https://github.com/mdavisprog/OctaneGUI/wiki/Images/September22/Window_High_Res.png width=50%>

![Inspector](https://github.com/mdavisprog/OctaneGUI/wiki/Images/May22/Inspector.gif)

![Designer](https://github.com/mdavisprog/OctaneGUI/wiki/Images/July22/Designer_Preview_Panel.gif)

* [Overview](#Overview)
* [Support](#Support)
	* [Windowing](#Windowing)
	* [Rendering](#Rendering)
* [Build](#Build)
	* [Repository](#Repository)

# Overview

OctaneGUI is a renderer agnostic multi-window multi-platform UI library for C++. The main goal of this library is to easily create applications that is performant on all devices. It is architected in a way to allow for easy integration to other frameworks that may have its own rendering pipeline. This project will provide multiple integrations to widely used windowing and rendering libraries such as SDL, SFML, OpenGL, and Metal among others.

OctaneGUI uses an event-driven paradigm for the UI system. The application and all of its controls and layouts can be defined through a JSON string. The C++ code can then register events for controls defined in the JSON string through IDs. Below is an example of a Hello World application with a button for user interaction.

```C++
#include "Interface.h"
#include "OctaneGUI/OctaneGUI.h"

int main(int argc, char** argv)
{
	const char* Stream = R"({
	"Theme": "./Themes/Dark.json",
	"Windows": {
		"Main": {"Title": "Hello", "Width": 300, "Height": 200, "Body": {"Controls": [
			{"ID": "Container", "Type": "VerticalContainer", "Controls": [
				{"ID": "Button", "Type": "TextButton", "Text": {"Text": "Hello Friends"}}
			]}
		]}}
	}
})";

	OctaneGUI::Application Application;
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	Application.Initialize(Stream, WindowControls);

	const OctaneGUI::ControlList& List = WindowControls["Main"];
	List.To<OctaneGUI::Button>("Container.Button")->SetOnClicked([&](const OctaneGUI::Button&) -> void
	{
		static bool DidClick = false;
		if (!DidClick)
		{
			List.To<OctaneGUI::Container>("Container")->AddControl<OctaneGUI::Text>()->SetText("Welcome to the program :^)!");
			DidClick = true;
		}
	});

	return Application.Run();
}
```

![Hello Sample](https://github.com/mdavisprog/OctaneGUI/wiki/Hello.gif)

# Support

The following tables displays what platforms are supported and which windowing and rendering libraries can be used for each platform. This information is for users who wish to build the applications in this repository.

## Windowing

|Platform|SDL2|SFML|
|:---:|:---:|:---:|
|Windows|X|X|
|MacOS|X||
|Linux|X|X|

## Rendering

|Platform|Metal|OpenGL|SFML|
|:---:|:---:|:---:|:---:|
|Windows||X|X|
|MacOS|X|||
|Linux||X|X|

# Build

Below are instructions for how to build this project out of the repository itself or integrate it into other projects.

## Prerequisites

Before beginning the process of building the project, some tools will be needed. Below is a list of the tools that will need to be installed:

* [CMake](https://cmake.org/download/) - This is a tool that helps with generating projects. It is a common tool used for many open source projects.
* Compiler - A C++ compiler is needed to compile the library and its apps. This is platform specific and the compiler for each platform is listed below.
	* [Visual Studio](https://visualstudio.microsoft.com/) - This is the compiler used on Windows. A Community version is offered for free which provides the compiler along with the IDE. The IDE is not required as the build scripts will find the proper tools needed to compile.
		* Visual Studio Build Tools - This is a download offered that will only install the compiler and other tools needed to compile the project without the IDE.
	* GCC - This is the compiler used on Linux. Please refer to your Linux distribution for proper instructions on how to install these tools.
	* XCode - This project has used 'Command Line Tools for Xcode' to build on Macs.
* (Optional) [Ninja](https://ninja-build.org/) - This is a light-weight build system that is supported on all platforms. If this is not used, then 'msbuild'/Visual Studio is used on Windows and 'Make' is used on other platforms.
* An installation of either the [SDL2](https://libsdl.org/) or [SFML](https://www.sfml-dev.org/) development libraries.

## Scripts

The repository provides shell scripts and batch files to make building the project and its applications easier. These files can be found in the 'Scripts' folder. The 'Build' script is the main script to run when building this project. This can be run from any directory and accpets a number of arguments. Below is the list of arguments and a desription for each:

* release - Sets the configuration to Release. The default is Debug.
* tools - Compiles with tools enabled. This can be compiled in any configuration.
* ninja - Use the ninja build system instead of the default (make or msbuild). The path to the ninja build system must be added to the PATH environment variable.
* clean - Cleans the intermediate files before generating and building. This forces a full rebuild of the project.
* noapps - Only compiles the library.
* sfml - Builds the apps using the SFML library. The SFML_DIR variable must be set for the generator to locate the library.
* sdl2 - Builds the app using the SDL library. The SDL2 cmake and library paths must be locatable by the generator through either the environment variables or the SDL2_DIR and SDL2_MODULE_PATH variables.
* help - Displays this help message.

On Windows, a VCVars.bat file is called which will load Visual Studio tools into the current environment. This will attempt to load Visual Studio 2019 first, then 2017. More versions of Visual Studio will be added in the future.

The path to CMake/Ninja will need to be set in the PATH environment variable on your system. This may be set during the installation of these tools. The PATH can also be set in your own script before calling the 'Build' script. Below are example scripts that can be used to build the project.

```batch
@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET PATH=%PATH%;"PATH\TO\CMake\bin\"
SET PATH=%PATH%;"PATH\TO\Ninja\"
SET SFML_DIR=PATH\TO\SFML\lib\cmake\SFML
SET SDL2_MODULE_PATH="PATH\TO\SDL2\cmake"
SET SDL2_DIR="PATH\TO\SDL2"

CALL Scripts\Build.bat %*

ENDLOCAL
```

```bash
#!/bin/bash
PATH="$PATH:/PATH/TO/CMake/bin:/PATH/TO/Ninja"
source Scripts/Build.sh
```

## Integration

TODO
