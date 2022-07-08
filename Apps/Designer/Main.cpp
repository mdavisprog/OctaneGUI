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

	std::shared_ptr<OctaneGUI::TextEditor> Document { nullptr };
	std::shared_ptr<OctaneGUI::Text> StatusText { nullptr };
	std::shared_ptr<OctaneGUI::Panel> StatusBar { nullptr };
	std::shared_ptr<OctaneGUI::Window> PreviewWindow { nullptr };
	std::shared_ptr<OctaneGUI::WindowContainer> PreviewPane { nullptr };
	std::shared_ptr<OctaneGUI::Container> Editor { nullptr };
	std::shared_ptr<OctaneGUI::Splitter> Splitter { nullptr };

	std::shared_ptr<OctaneGUI::Window> MainWindow = Application.GetWindow("Main");
	std::shared_ptr<OctaneGUI::Timer> CompileTimer = MainWindow->CreateTimer(500, false, [&]() -> void
		{
			const std::string Contents = OctaneGUI::String::ToMultiByte(Document->GetText());
			bool IsError = false;
			OctaneGUI::Json Root = OctaneGUI::Json::Parse(Contents.c_str(), IsError);

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

				if (PreviewWindow->IsVisible())
				{
					const OctaneGUI::Json& Main = Root["Windows"]["Main"];

					if (!Main.IsNull())
					{
						PreviewWindow->SetTitle(Main["Title"].String());
						PreviewWindow->Clear();
						PreviewWindow->LoadContents(Main);
					}
				}

				if (PreviewPane)
				{
					const OctaneGUI::Json& Main = Root["Windows"]["Main"];

					PreviewPane->Clear();
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

	std::shared_ptr<OctaneGUI::MenuItem> QuitMenu = MainList.To<OctaneGUI::MenuItem>("File.Quit");
	QuitMenu->SetOnPressed([&](OctaneGUI::TextSelectable& Item) -> void
		{
			Application.Quit();
		});
	
	Editor = MainList.To<OctaneGUI::Container>("Editor");
	Splitter = MainList.To<OctaneGUI::Splitter>("Editor.Splitter");

	Document = MainList.To<OctaneGUI::TextEditor>("Editor.Splitter.Document");
	Document->SetOnTextChanged([&](std::shared_ptr<OctaneGUI::TextInput>) -> void
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
	PreviewWindow->SetOnClose([&](OctaneGUI::Window& Target) -> void
		{
			PreviewWindowMenu->SetChecked(false);
			Application.CloseWindow("PreviewWindow");
		});

	return Application.Run();
}
