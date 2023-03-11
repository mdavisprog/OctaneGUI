#include "Frontend.h"
#include "OctaneGUI/OctaneGUI.h"

int main(int, char**)
{
    OctaneGUI::Application Application;
    Frontend::Initialize(Application);

    const std::string Contents = Application.FS().LoadContents("CodeEdit.json");

    std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
    Application.Initialize(Contents.c_str(), WindowControls);

    return Application.Run();
}
