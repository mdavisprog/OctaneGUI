#include "Frontend.h"
#include "Windowing/Windowing.h"
#include "OctaneGUI/OctaneGUI.h"

#include <fstream>
#include <string>

int main(int, char**)
{
    OctaneGUI::Application Application;
    Frontend::Initialize(Application);

    std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
    Application.Initialize(Application.FS().LoadContents("App.json").c_str(), WindowControls);

    std::shared_ptr<OctaneGUI::TextEditor> Document { nullptr };
    std::shared_ptr<OctaneGUI::Text> StatusText { nullptr };
    std::shared_ptr<OctaneGUI::Panel> StatusBar { nullptr };
    std::shared_ptr<OctaneGUI::Window> PreviewWindow { nullptr };
    std::shared_ptr<OctaneGUI::WindowContainer> PreviewPane { nullptr };
    std::shared_ptr<OctaneGUI::Container> Editor { nullptr };
    std::shared_ptr<OctaneGUI::Splitter> Splitter { nullptr };
    std::u32string OpenFileName {};

    std::shared_ptr<OctaneGUI::Window> MainWindow = Application.GetWindow("Main");
    std::shared_ptr<OctaneGUI::Timer> CompileTimer = MainWindow->CreateTimer(500, false, [&]() -> void
        {
            const std::string Contents = OctaneGUI::String::ToMultiByte(Document->GetText());
            bool IsError = false;
            const OctaneGUI::Json Root = OctaneGUI::Json::Parse(Contents.c_str(), IsError);

            Document->ClearLineColors();

            if (IsError)
            {
                size_t Line = (size_t)Root["Line"].Number();
                Document->SetLineColor(Line, OctaneGUI::Color(148, 0, 0, 255));
                StatusBar->SetProperty(OctaneGUI::ThemeProperties::Panel, OctaneGUI::Color(148, 0, 0, 255));
                StatusText->SetText(OctaneGUI::String::ToUTF32(Root["Error"].String()).c_str());
            }
            else
            {
                StatusBar->ClearProperty(OctaneGUI::ThemeProperties::Panel);
                StatusText->SetText(U"OK!");

                PreviewWindow->Clear();
                PreviewPane->Clear();

                if (!Root.Contains("Windows"))
                {
                    return;
                }

                const OctaneGUI::Json& Windows = Root["Windows"];
                
                if (!Windows.Contains("Main"))
                {
                    return;
                }

                const OctaneGUI::Json& Main = Windows["Main"];

                if (PreviewWindow->IsVisible())
                {
                    PreviewWindow->SetTitle(Main["Title"].String());
                    PreviewWindow->LoadContents(Main);
                }

                if (PreviewPane)
                {
                    PreviewPane->OnLoad(Main);
                }
            }
        });

    const OctaneGUI::ControlList& MainList = WindowControls["Main"];

    std::shared_ptr<OctaneGUI::MenuItem> PreviewWindowMenu = MainList.To<OctaneGUI::MenuItem>("File.PreviewWindow");
    PreviewWindowMenu->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            PreviewWindowMenu->SetChecked(!PreviewWindowMenu->IsChecked());

            if (PreviewWindowMenu->IsChecked() && !PreviewWindow->IsVisible())
            {
                Application.DisplayWindow("PreviewWindow");
                CompileTimer->Start();
            }
            else if (!PreviewWindowMenu->IsChecked() && PreviewWindow->IsVisible())
            {
                PreviewWindow->Close();
            }
        });
    
    std::shared_ptr<OctaneGUI::MenuItem> PreviewPaneMenu = MainList.To<OctaneGUI::MenuItem>("File.PreviewPane");
    PreviewPaneMenu
        ->SetChecked(true)
        .SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            PreviewPaneMenu->SetChecked(!PreviewPaneMenu->IsChecked());

            if (PreviewPaneMenu->IsChecked() && !Editor->HasControl(Splitter))
            {
                Editor->ClearControls();
                Editor->InsertControl(Splitter);
            }
            else if (!PreviewPaneMenu->IsChecked() && !Editor->HasControl(Document))
            {
                Editor->ClearControls();
                Editor->InsertControl(Document);
            }
        });
    
    std::shared_ptr<OctaneGUI::MenuItem> FontMenu = MainList.To<OctaneGUI::MenuItem>("Font");
    std::shared_ptr<OctaneGUI::TextInput> FontSizeMenu = MainList.To<OctaneGUI::TextInput>("Font.FontSize");

    FontMenu->SetOnOpenMenu([&](std::shared_ptr<OctaneGUI::MenuItem const>) -> void
        {
            const float FontSize = Document->LineHeight();
            FontSizeMenu->SetText(OctaneGUI::String::ToMultiByte(FontSize).c_str());
        });

    FontSizeMenu->SetOnConfirm([&](OctaneGUI::TextInput& Input) -> void
        {
            if (!FontSizeMenu->GetString().empty())
            {
                const float FontSize = OctaneGUI::String::ToFloat(FontSizeMenu->GetText());
                Document->SetFontSize(FontSize);
            }
            Input.GetWindow()->ClosePopup();
        });
    
    Application.FS().SetOnFileDialogResult([&](OctaneGUI::FileDialogType Type, const std::u32string& FileName) -> void
        {
            if (!FileName.empty())
            {
                OpenFileName = FileName;
                if (Type == OctaneGUI::FileDialogType::Open)
                {
                    const std::string Contents = Application.FS().LoadContents(OctaneGUI::String::ToMultiByte(FileName).c_str());
                    Document->SetText(Contents.c_str());
                }
                else
                {
                    Application.FS().WriteContents(FileName, Document->GetText());
                }
            }
        });
    
    const std::vector<OctaneGUI::FileDialogFilter> Filters {{{U"json"}, U"Json Files"}};
    MainList.To<OctaneGUI::MenuItem>("File.Open")->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            Application.FS().FileDialog(OctaneGUI::FileDialogType::Open, Filters);
        });
    
    MainList.To<OctaneGUI::MenuItem>("File.Save")->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            if (OpenFileName.empty())
            {
                Application.FS().FileDialog(OctaneGUI::FileDialogType::Save, Filters);
            }
            else
            {
                Application.FS().WriteContents(OpenFileName, Document->GetText());
            }
        });
    
    MainList.To<OctaneGUI::MenuItem>("File.SaveAs")->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            Application.FS().FileDialog(OctaneGUI::FileDialogType::Save, Filters);
        });

    std::shared_ptr<OctaneGUI::MenuItem> QuitMenu = MainList.To<OctaneGUI::MenuItem>("File.Quit");
    QuitMenu->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            Application.Quit();
        });
    
    Editor = MainList.To<OctaneGUI::Container>("Editor");
    Splitter = MainList.To<OctaneGUI::Splitter>("Editor.Splitter");

    Document = MainList.To<OctaneGUI::TextEditor>("Editor.Splitter.Document");
    Document->Highlighter().SetRules(OctaneGUI::Syntax::Rules::Get(U".json"));
    Document
        ->SetOnTextChanged([&](OctaneGUI::TextInput&) -> void
        {
            CompileTimer->Start();
        })
        .SetText(UR"({
    "Windows": {
        "Main": {"Title": "Untitled", "Width": 400, "Height": 200,
            "MenuBar": {"Items": []},
            "Body": {"Controls": []}
        }
    }
})");

    PreviewPane = MainList.To<OctaneGUI::WindowContainer>("Editor.Splitter.PreviewPane");

    StatusBar = MainList.To<OctaneGUI::Panel>("StatusBar");
    StatusText = MainList.To<OctaneGUI::Text>("Status");
    StatusText->SetText(U"New Document");

    PreviewWindow = Application.NewWindow("PreviewWindow", OctaneGUI::String::ToMultiByte(Document->GetText()).c_str());
    PreviewWindow->SetOnClose([&](OctaneGUI::Window&) -> void
        {
            PreviewWindowMenu->SetChecked(false);
            Application.CloseWindow("PreviewWindow");
        });

    return Application.Run();
}
