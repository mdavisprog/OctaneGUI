# OctaneGUI
Latest update: [March 2022](https://github.com/mdavisprog/OctaneGUI/wiki/March22)

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
