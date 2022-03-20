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

#include <filesystem>
#include <fstream>
#include <string>

std::unordered_map<std::string, std::string> Themes;

std::string GetContents(const char* Filename)
{
	std::string Result;
	std::ifstream File;
	File.open(Filename);
	if (File.is_open())
	{
		File.seekg(0, std::ios::end);
		Result.resize(File.tellg());
		File.seekg(0, std::ios::beg);

		File.read(&Result[0], Result.size());
		File.close();
	}
	return Result;
}

int main(int argc, char **argv)
{
	if (std::filesystem::exists("./Themes"))
	{
		for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator("./Themes"))
		{
			const std::string Stem = Entry.path().stem().string().c_str();
			Themes[Stem] = Entry.path().string().c_str();
		}
	}

	OctaneUI::Application Application;
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneUI::ControlList> WindowControls;
	Application.Initialize(GetContents("Overview.json").c_str(), WindowControls);
	
	const OctaneUI::ControlList& List = WindowControls["Main"];
	List.To<OctaneUI::MenuItem>("File.Quit")->SetOnSelected([&](OctaneUI::MenuItem* Item) -> void
	{
		Application.Quit();
	});

	std::shared_ptr<OctaneUI::Menu> ThemesMenu = List.To<OctaneUI::MenuItem>("Themes")->CreateMenu();
	for (const std::pair<std::string, std::string>& Theme : Themes)
	{
		const char* Name = Theme.first.c_str();
		ThemesMenu->AddItem(Name);
		ThemesMenu->GetItem(Name)->SetOnSelected([&](OctaneUI::MenuItem* Item) -> void
		{
			const char* Name = Item->GetText();
			if (Themes.find(Name) != Themes.end())
			{
				const std::string& Path = Themes[Name];
				const std::string Buffer = GetContents(Path.c_str());
				Application.GetTheme()->Load(OctaneUI::Json::Parse(Buffer.c_str()));
			}
		});
	}

	List.To<OctaneUI::MenuItem>("Help.About")->SetOnSelected([&](OctaneUI::MenuItem* Item) -> void
	{
		Application.DisplayWindow("About");
	});

	List.To<OctaneUI::Button>("OKBtn")->SetOnPressed([&](const OctaneUI::Button& Button) -> void
	{
		printf("OK\n");
	});

	std::shared_ptr<OctaneUI::ListBox> ListBox = List.To<OctaneUI::ListBox>("List");
	ListBox->SetOnSelect([](int Index, std::weak_ptr<OctaneUI::Control>) -> void
	{
		printf("OnSelect: %d\n", Index);
	});

	for (int I = 0; I < 10; I++)
	{
		std::string Text = std::string("Item added through code: ") + std::to_string(I);
		ListBox->AddItem<OctaneUI::Text>()->SetText(Text.c_str());
	}

	const OctaneUI::ControlList& AboutList = WindowControls["About"];
	AboutList.To<OctaneUI::Button>("OK")->SetOnPressed([](const OctaneUI::Button& Button) -> void
	{
		Button.GetWindow()->RequestClose();
	});

	return Application.Run();
}
