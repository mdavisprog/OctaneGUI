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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"
#include "Utility.h"

namespace Tests
{

const char* MenuBarJson = 
R"("Items": [
	{"Text": "File", "ID": "File", "Items": [
		{"Text": "Quit", "ID": "Quit"}
	]},
	{"Text": "Help", "ID": "Help", "Items": [
		{"Text": "One", "ID": "One", "Items": [
			{"Text": "Two", "ID": "Two"}
		]}
	]}
])";

TEST_SUITE(MenuBar,

TEST_CASE(Open,
{
	OctaneGUI::ControlList List;
	Utility::Load(Application, MenuBarJson, "", List);

	const std::shared_ptr<OctaneGUI::MenuItem> FileMenu = List.To<OctaneGUI::MenuItem>("File");
	Utility::MouseClick(Application, FileMenu->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 5.0f});
	Application.Update();

	const std::shared_ptr<OctaneGUI::Window> Window = Application.GetMainWindow();
	return Window->IsPopupOpen();
})

TEST_CASE(CloseMenu,
{
	OctaneGUI::ControlList List;
	Utility::Load(Application, MenuBarJson, "", List);

	const std::shared_ptr<OctaneGUI::Window> Window = Application.GetMainWindow();
	const std::shared_ptr<OctaneGUI::MenuItem> FileMenu = List.To<OctaneGUI::MenuItem>("File");
	Utility::MouseClick(Application, FileMenu->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 5.0f});
	Application.Update();

	VERIFYF(Window->IsPopupOpen(), "File menu is not open!");
	Utility::MouseClick(Application, Window->GetSize() + OctaneGUI::Vector2{-5.0, -5.0f});

	return !Window->IsPopupOpen();
})

TEST_CASE(ClickItem,
{
	OctaneGUI::ControlList List;
	Utility::Load(Application, MenuBarJson, "", List);

	const std::shared_ptr<OctaneGUI::MenuItem> FileMenu = List.To<OctaneGUI::MenuItem>("File");
	Utility::MouseClick(Application, FileMenu->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 5.0f});
	Application.Update();

	bool Clicked = false;
	const std::shared_ptr<OctaneGUI::MenuItem> QuitMenu = List.To<OctaneGUI::MenuItem>("File.Quit");
	QuitMenu->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
		{
			Clicked = true;
		});
	Utility::MouseClick(Application, QuitMenu->GetAbsolutePosition() + OctaneGUI::Vector2(5.0f, 5.0f));
	Application.Update();
	
	const std::shared_ptr<OctaneGUI::Window> Window = Application.GetMainWindow();
	return Clicked && !Window->IsPopupOpen();
})

TEST_CASE(SubMenu,
{
	OctaneGUI::ControlList List;
	Utility::Load(Application, MenuBarJson, "", List);

	const std::shared_ptr<OctaneGUI::Window> Window = Application.GetMainWindow();
	const std::shared_ptr<OctaneGUI::MenuItem> HelpMenu = List.To<OctaneGUI::MenuItem>("Help");
	const std::shared_ptr<OctaneGUI::MenuItem> OneMenu = List.To<OctaneGUI::MenuItem>("Help.One");
	const std::shared_ptr<OctaneGUI::MenuItem> TwoMenu = List.To<OctaneGUI::MenuItem>("Help.One.Two");

	Utility::MouseClick(Application, HelpMenu->GetAbsolutePosition());
	Application.Update();

	VERIFYF(Window->IsPopupOpen(), "Help menu is not open!");

	bool IsSubMenuOpen = false;
	OneMenu->SetOnOpenMenu([&](std::shared_ptr<OctaneGUI::MenuItem const> MenumItem) -> void
		{
			IsSubMenuOpen = true;
		});

	Utility::MouseMove(Application, OneMenu->GetAbsolutePosition());
	Application.Update();

	VERIFYF(IsSubMenuOpen, "Menu for menu item 'One' is not open!");
	
	bool IsTwoMenuSelected = false;
	TwoMenu->SetOnPressed([&](OctaneGUI::TextSelectable&) -> void
		{
			IsTwoMenuSelected = true;
		});
	
	Utility::MouseClick(Application, OneMenu->GetAbsolutePosition() + OctaneGUI::Vector2{OneMenu->GetSize().X, 0.0f});
	Application.Update();
	
	VERIFYF(IsTwoMenuSelected, "The sub-menu was not clicked!");
	VERIFYF(!Window->IsPopupOpen(), "Menus are still open when they should not be!");

	return true;
})

)

}
