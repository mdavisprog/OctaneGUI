# OctaneGUI
Latest update: [May 2022](https://github.com/mdavisprog/OctaneGUI/wiki/May22)

<img src=https://github.com/mdavisprog/OctaneGUI/wiki/Images/May22/Overview_Classic.png width=50%>

![Inspector](https://github.com/mdavisprog/OctaneGUI/wiki/Images/May22/Inspector.gif)

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
	const char* Stream = 
	"{"
		"\"Theme\": \"./Themes/Dark.json\","
		"\"Windows\": {"
			"\"Main\": {"
				"\"Title\": \"Hello\","
				"\"Width\": 300,"
				"\"Height\": 200,"
				"\"Body\": {"
					"\"Controls\": ["
						"{"
							"\"ID\": \"Container\","
							"\"Type\": \"VerticalContainer\","
							"\"Controls\": ["
								"{"
									"\"ID\": \"Button\","
									"\"Type\": \"TextButton\","
									"\"Text\": {"
										"\"Text\": \"Hello Friends\""
									"}"
								"}"
							"]"
						"}"
					"]"
				"}"
			"}"
		"}"
	"}";

	OctaneGUI::Application Application;
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	Application.Initialize(Stream, WindowControls);

	const OctaneGUI::ControlList& List = WindowControls["Main"];
	List.To<OctaneGUI::Button>("Container.Button")->SetOnPressed([&]()
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

## Repository

Before setting up the build process, some external libraries are needed. Refer to the [table](#Support) to see which libraries are desired for the target platform. The project uses CMake to generate projects and can be downloaded [here](https://cmake.org/download/).

* SDL2
	* The development packages can be downloaded from [here](https://libsdl.org/download-2.0.php).
	* The downloaded package doesn't come with any CMake files, however, there is a great resource provided [here](https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/).
* SFML
	* The development packages can be downloaded from [here](https://www.sfml-dev.org/download/sfml/2.5.1/). Some compiler versions may not be available here, which may require building from source with the latest compiler.

Use the following CMake command to genereate the project while replacing any values denoted in curly braces {}.

```
cmake -S {PROJECT_PATH} -DCMAKE_BUILD_TYPE={CONFIGURATION} -DTOOLS={TOOLS_VALUE} -DWINDOWING={WINDOWING_LIBRARY} -DRENDERING={RENDERING_LIBRARY} -DCMAKE_MODULE_PATH={LIBRARY_CMAKE_LOCATION} {LIBRARY_SPECIFIC_OPTIONS}
```

* PROJECT_PATH - The location of the repository.
* CONFIGURATION - This should either be Release or Debug. The default value is Release.
* TOOLS_VALUE - Should be ON or OFF. This determines if the 'Tools' directory is included when compiling the OctaneGUI library. The default value is OFF.
* WINDOWING_LIBRARY - Can be one of SDL2 or SFML. The default on Mac is SDL2 and Windows/Linux is SFML.
* RENDERING_LIBRARY - Can be one of SFML, OpenGL, or Metal. The default on Mac is Metal and Windows/Linux is SFML.
* LIBRARY_CMAKE_LOCATION - This should be the location of the associated cmake file for the desired library. This may not be needed on some systems as CMake will attempt to find the appropriate cmake files in system directories.
* LIBRARY_SPECIFIC_OPTIONS - This is an option that is required by the desired library. This may be the the location of the libraries headers and linkable libraries through the SDL2_DIR or SFML_DIR variables.

This command will generate the files needed to build the project for the desired build system such as Visual Studio or Make.

Once the project is built, several binaries are generated in the bin folder. These are the applications found in the 'Apps' folder. Dependencies such as dynamic link libraries may need to be copied into the bin folder to run these applications.

In the future, a 'Scripts' folder may be introduced to provide example batch or bash scripts that can be tailored for use to custom build systems.

### Integration

TODO
