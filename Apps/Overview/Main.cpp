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
#include "OctaneGUI/OctaneGUI.h"

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

void LoadTheme(OctaneGUI::Application& Application, const char* Name)
{
	if (Themes.find(Name) == Themes.end())
	{
		return;
	}

	const std::string& Path = Themes[Name];
	const std::string Buffer = GetContents(Path.c_str());
	Application.GetTheme()->Load(OctaneGUI::Json::Parse(Buffer.c_str()));
}

void SelectMenuItem(OctaneGUI::Application& Application, const std::string& Name)
{
	std::shared_ptr<OctaneGUI::Menu> Menu = Application.GetMainWindow()->GetMenuBar()->Item("Themes");

	std::vector<std::shared_ptr<OctaneGUI::MenuItem>> Options;
	Menu->GetMenuItems(Options);

	for (std::shared_ptr<OctaneGUI::MenuItem>& Option : Options)
	{
		const bool IsSelected = Name == OctaneGUI::Json::ToMultiByte(Option->GetText());
		Option->SetChecked(IsSelected);
	}
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

	std::string Theme = "Dark";
	for (int I = 0; I < argc; I++)
	{
		if (std::string(argv[I]) == "--theme" && I < argc - 1)
		{
			Theme = argv[I + 1];
		}
	}

	OctaneGUI::Application Application;
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	Application.Initialize(GetContents("Overview.json").c_str(), WindowControls);
	LoadTheme(Application, Theme.c_str());
	
	const OctaneGUI::ControlList& List = WindowControls["Main"];
	List.To<OctaneGUI::MenuItem>("File.Quit")->SetOnPressed([&](const OctaneGUI::TextSelectable&) -> void
	{
		Application.Quit();
	});

	std::shared_ptr<OctaneGUI::Menu> ThemesMenu = List.To<OctaneGUI::MenuItem>("Themes")->CreateMenu();
	for (const std::pair<std::string, std::string>& Theme : Themes)
	{
		const char* Name = Theme.first.c_str();
		ThemesMenu->AddItem(Name);
		ThemesMenu->GetItem(Name)->SetOnPressed([&](const OctaneGUI::TextSelectable& Item) -> void
		{
			const std::string Name = OctaneGUI::Json::ToMultiByte(Item.GetText());
			LoadTheme(Application, Name.c_str());
			SelectMenuItem(Application, Name);
		});
	}
	SelectMenuItem(Application, Theme);

	List.To<OctaneGUI::MenuItem>("Help.About")->SetOnPressed([&](const OctaneGUI::TextSelectable&) -> void
	{
		Application.DisplayWindow("About");
	});

	const OctaneGUI::ControlList& AboutList = WindowControls["About"];
	AboutList.To<OctaneGUI::Button>("OK")->SetOnClicked([](const OctaneGUI::Button& Button) -> void
	{
		Button.GetWindow()->RequestClose();
	});

	return Application.Run();
}
