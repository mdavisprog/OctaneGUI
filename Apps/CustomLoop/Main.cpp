#include "Frontend.h"
#include "OctaneGUI/OctaneGUI.h"

int main(int argc, char** argv)
{
    const char* Json = R"({
        "Theme": "Resources/Themes/Dark.json",
        "Windows": {
            "Main": {"Title": "Custom Loop", "Width": 1280, "Height": 720,
                "MenuBar": {},
                "Body": {"Controls": []}
            }
        }
    })";

    OctaneGUI::Application Application;
    Frontend::Initialize(Application);

    std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
    Application
        .SetCommandLine(argc, argv)
        .Initialize(Json, WindowControls);
    
    while (Application.IsRunning())
    {
        Application.RunFrame();
    }

    return 0;
}
