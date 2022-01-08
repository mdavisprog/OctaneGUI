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

#include "Interface.h"
#include "OctaneUI/OctaneUI.h"

int main(int argc, char **argv)
{
	OctaneUI::Application Application;
	SFML::Initialize(Application);
	Application.Initialize("OctaneUI");
	Application.LoadFont("Roboto-Regular.ttf");
	Application.GetMainWindow()->GetMenuBar()->AddItem("File")
		->AddItem("New", [&]() -> void 
		{
			Application.NewWindow("Test", 640, 480)
				->GetContainer()->AddText("Hello Test");
		})
		->AddItem("Open", [=]() -> void {printf("Open\n");})
		->AddSeparator()
		->AddItem("Quit", [=]() -> void {printf("Quit\n");})
		->GetItem("Open")->CreateMenu()
			->AddItem("Open File", [=]() -> void {printf("Open File\n");})
			->AddItem("Open Folder", [=]() -> void {printf("Open Folder\n");})
			->GetItem("Open File")->SetChecked(true);

	Application.GetMainWindow()->GetMenuBar()->AddItem("Help")
		->AddItem("About", [=]() -> void {printf("About\n");});
	
	std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->AddImage("info.png");
	Container->AddText("Hello World");
	Container->AddCheckbox("Checkbox");
	Container->AddTextInput();
	return Application.Run();
}
