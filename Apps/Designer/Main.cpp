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

	const OctaneGUI::ControlList& MainList = WindowControls["Main"];
	std::shared_ptr<OctaneGUI::MenuItem> QuitMenu = MainList.To<OctaneGUI::MenuItem>("File.Quit");
	QuitMenu->SetOnPressed([&](OctaneGUI::TextSelectable& Item) -> void
		{
			Application.Quit();
		});
	
	std::shared_ptr<OctaneGUI::Text> StatusText = MainList.To<OctaneGUI::Text>("Status");
	StatusText->SetText(U"New Document");

	return Application.Run();
}
