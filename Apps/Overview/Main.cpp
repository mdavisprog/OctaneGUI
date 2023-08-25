/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

int main(int argc, char **argv)
{
    if (std::filesystem::exists("./Resources/Themes"))
    {
        for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator("./Resources/Themes"))
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
    Frontend::Initialize(Application);

    std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
    Application
        .SetCommandLine(argc, argv)
        .Initialize(GetContents("Overview.json").c_str(), WindowControls);
    LoadTheme(Application, Theme.c_str());
    
    const OctaneGUI::ControlList& List = WindowControls["Main"];
    List.To<OctaneGUI::MenuItem>("File.OpenFile")->SetOnPressed([&](const OctaneGUI::TextSelectable&) -> void
        {
            Application.FS().FileDialog(OctaneGUI::FileDialogType::Open, {{{U"*"}, U"All Files"}, {{U"txt"}, U"Text Files"}});
        });

    List.To<OctaneGUI::MenuItem>("File.Quit")->SetOnPressed([&](const OctaneGUI::TextSelectable&) -> void
    {
        Application.Quit();
    });

    List.To<OctaneGUI::MenuItem>("Help.About")->SetOnPressed([&](const OctaneGUI::TextSelectable&) -> void
    {
        Application.DisplayWindow("About");
    });

    List.To<OctaneGUI::TextButton>("ButtonContextMenu")->SetOnCreateContextMenu([&](OctaneGUI::Control&, const std::shared_ptr<OctaneGUI::Menu>& ContextMenu) -> void
    {
        ContextMenu
            ->AddItem("Option 1", []() -> void
                {
                    printf("Selected Option 1\n");
                })
            .AddItem("Option 2", []() -> void
                {
                    printf("Selected Option 2\n");
                });
    });
    
    List.To<OctaneGUI::Button>("FlipSpinners")->SetOnClicked([List](OctaneGUI::Button&) -> void
    {
        List.To<OctaneGUI::Spinner>("SpinnerV")->SetOrientation(List.To<OctaneGUI::Spinner>("SpinnerV")->GetOppositeOrientation());
        List.To<OctaneGUI::Spinner>("SpinnerH")->SetOrientation(List.To<OctaneGUI::Spinner>("SpinnerH")->GetOppositeOrientation());
    });

    std::shared_ptr<OctaneGUI::ComboBox> ThemesComboBox = List.To<OctaneGUI::ComboBox>("ThemesComboBox");
    ThemesComboBox->SetOnSelected([&](const std::u32string& Selected) -> void
        {
            const std::string Name = OctaneGUI::String::ToMultiByte(Selected);
            LoadTheme(Application, Name.c_str());
        });
    for (const std::pair<std::string, std::string> Item : Themes)
    {
        ThemesComboBox->AddItem(Item.first.c_str());
    }
    ThemesComboBox->SetSelected(OctaneGUI::String::ToUTF32(Theme).c_str());

    std::shared_ptr<OctaneGUI::ListBox> ListBox = List.To<OctaneGUI::ListBox>("ListBox.ListBox");
    List.To<OctaneGUI::CheckBox>("ListBox.MultiSelect")->SetOnClicked([ListBox](OctaneGUI::Button&) -> void
        {
            ListBox->Deselect();
            ListBox->SetMultiSelect(!ListBox->MultiSelect());
        });
    for (int I = 0; I < 15; I++)
    {
        const std::string Text { std::string("Item ") + std::to_string(I) };
        ListBox->AddItem<OctaneGUI::Text>()->SetText(Text.c_str());
    }

    const OctaneGUI::ControlList& AboutList = WindowControls["About"];
    AboutList.To<OctaneGUI::Button>("OK")->SetOnClicked([](OctaneGUI::Button& Button) -> void
    {
        Button.GetWindow()->RequestClose();
    });

    return Application.Run();
}
