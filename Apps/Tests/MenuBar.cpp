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

#include <cmath>

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

TEST_CASE(Position,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, MenuBarJson, "", List);

    const std::shared_ptr<OctaneGUI::Window> Window = Application.GetMainWindow();
    const std::shared_ptr<OctaneGUI::MenuItem> FileMenu = List.To<OctaneGUI::MenuItem>("File");
    const std::shared_ptr<OctaneGUI::MenuItem> HelpMenu = List.To<OctaneGUI::MenuItem>("Help");

    const OctaneGUI::Vector2 FilePosition = FileMenu->GetAbsolutePosition();
    Utility::MouseClick(Application, FilePosition);
    Application.Update();

    const OctaneGUI::Vector2 FileMenuPos = FileMenu->GetMenu()->GetAbsolutePosition();
    VERIFYF(FilePosition.X == FileMenuPos.X, "File menu's X position '%.2f' does not match the label '%.2f'!", FileMenuPos.X, FilePosition.X);
    VERIFYF(FileMenuPos.Y == FileMenu->GetSize().Y, "File menu's Y position '%.2f' is not below the menu bar '%.2f'!", FileMenuPos.Y, FileMenu->GetSize().Y);

    const OctaneGUI::Vector2 HelpPosition = HelpMenu->GetAbsolutePosition();
    Utility::MouseMove(Application, HelpPosition);
    Application.Update();

    const OctaneGUI::Vector2 HelpMenuPos = HelpMenu->GetMenu()->GetAbsolutePosition();
    VERIFYF(HelpPosition.X == HelpMenuPos.X, "Help menu's X position '%.2f' does not match the label '%.2f'!", HelpMenuPos.X, HelpPosition.X);
    VERIFYF(HelpMenuPos.Y == HelpMenu->GetSize().Y, "File menu's Y position '%.2f' is not below the menu bar '%.2f'!", HelpMenuPos.Y, HelpMenu->GetSize().Y);

    return true;
})

TEST_CASE(SubMenuPosition,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, MenuBarJson, "", List);

    const std::shared_ptr<OctaneGUI::MenuItem> HelpMenu = List.To<OctaneGUI::MenuItem>("Help");
    const std::shared_ptr<OctaneGUI::MenuItem> OneMenu = List.To<OctaneGUI::MenuItem>("Help.One");

    Utility::MouseClick(Application, HelpMenu->GetAbsolutePosition());
    Application.Update();

    const OctaneGUI::Vector2 Margins = HelpMenu->GetMenu()->Margins();
    const OctaneGUI::Vector2 OnePosition = OneMenu->GetAbsolutePosition();
    Utility::MouseMove(Application, OnePosition);
    Application.Update();

    const OctaneGUI::Vector2 OneMenuPos = OneMenu->GetMenu()->GetAbsolutePosition() + OctaneGUI::Vector2(0.0f, Margins.Y);
    const float Expected = std::roundf(OnePosition.X + OneMenu->GetSize().X);
    VERIFYF(Expected == OneMenuPos.X, "One's menu X position '%.2f' is not at the expected position '%.2f'!", OneMenuPos.X, Expected);
    VERIFYF(OnePosition.Y == OneMenuPos.Y, "One's menu Y position '%.2f' is not at the expected position '%.2f'!", OneMenuPos.Y, OnePosition.Y);

    return true;
})

)

}
