/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Frontend.h"
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
	Frontend::Initialize(Application);

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
