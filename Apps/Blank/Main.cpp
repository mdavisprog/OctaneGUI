#include "Interface.h"
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
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	Application.Initialize(GetContents("Blank.json").c_str(), WindowControls);

	return Application.Run();
}
