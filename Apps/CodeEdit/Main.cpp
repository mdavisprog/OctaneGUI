#include "Frontend.h"
#include "OctaneGUI/OctaneGUI.h"

int main(int argc, char** argv)
{
    OctaneGUI::Application Application;
    Frontend::Initialize(Application);

    const std::string Contents = Application.FS().LoadContents("CodeEdit.json");

    std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
    Application
        .SetCommandLine(argc, argv)
        .Initialize(Contents.c_str(), WindowControls);

    const OctaneGUI::ControlList& ControlList = WindowControls["Main"];
    std::shared_ptr<OctaneGUI::TextEditor> Editor = ControlList.To<OctaneGUI::TextEditor>("Editor");
    Application.LS().AddServer(U"clangd", U"clangd", {U".h", U".cpp"});

    Application.FS().SetOnFileDialogResult([&](OctaneGUI::FileDialogType Type, const std::u32string& FileName) -> void
        {
            if (Type == OctaneGUI::FileDialogType::Open)
            {
                Editor->OpenFile(FileName.c_str());
            }
        });

    ControlList.To<OctaneGUI::MenuItem>("File.Open")->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            Application.FS().FileDialog(OctaneGUI::FileDialogType::Open);
        });
    ControlList.To<OctaneGUI::MenuItem>("File.Close")->SetOnPressed([&](OctaneGUI::TextSelectable) -> void
        {
            Editor->CloseFile();
        });
    ControlList.To<OctaneGUI::MenuItem>("File.Quit")->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
        {
            Application.Quit();
        });

    return Application.Run();
}
