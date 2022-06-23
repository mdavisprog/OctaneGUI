#include "Frontend.h"
#include "OctaneGUI/OctaneGUI.h"

#include <fstream>
#include <string>

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
	OctaneGUI::Application Application;
	Frontend::Initialize(Application);

	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	Application.Initialize(GetContents("App.json").c_str(), WindowControls);

	std::shared_ptr<OctaneGUI::TextInput> Document { nullptr };
	std::shared_ptr<OctaneGUI::Text> StatusText { nullptr };

	std::shared_ptr<OctaneGUI::Window> MainWindow = Application.GetWindow("Main");
	std::shared_ptr<OctaneGUI::Timer> CompileTimer = MainWindow->CreateTimer(500, false, [&]() -> void
		{
			const std::string Contents = OctaneGUI::Json::ToMultiByte(Document->GetText());
			bool IsError = false;
			OctaneGUI::Json Root = OctaneGUI::Json::Parse(Contents.c_str(), IsError);

			if (IsError)
			{
				StatusText->SetText(OctaneGUI::Json::ToUTF32(Root["Error"].String()).c_str());
			}
			else
			{
				StatusText->SetText(U"OK!");
			}
		});

	const OctaneGUI::ControlList& MainList = WindowControls["Main"];
	std::shared_ptr<OctaneGUI::MenuItem> QuitMenu = MainList.To<OctaneGUI::MenuItem>("File.Quit");
	QuitMenu->SetOnPressed([&](OctaneGUI::TextSelectable& Item) -> void
		{
			Application.Quit();
		});
	
	Document = MainList.To<OctaneGUI::TextInput>("Document");
	Document->SetOnTextChanged([&](std::shared_ptr<OctaneGUI::TextInput>) -> void
		{
			CompileTimer->Start();
		})
		.SetText(UR"({
	"Windows": {
		"Main": {
			"Title": "Untitled",
			"Width": 960,
			"Height": 540
		}
	}
})");
	
	StatusText = MainList.To<OctaneGUI::Text>("Status");
	StatusText->SetText(U"New Document");

	return Application.Run();
}
