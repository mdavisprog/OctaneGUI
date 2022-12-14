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

namespace Tests
{

static const char* Json = R"({"Width": 1280, "Height": 720, "Body": {"Controls": [{"Type": "TextButton", "ID": "Button", "Text": {"Text": "Button"}}]}})";

TEST_SUITE(Button,

TEST_CASE(Press,
{
    OctaneGUI::ControlList List;
    Application.GetMainWindow()->Load(Json, List);
    Application.GetMainWindow()->Update();

    VERIFY(List.Contains("Button"))

    std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");

    bool Pressed = false;
    Button->SetOnPressed([&](OctaneGUI::Button&) -> void
    {
        Pressed = true;
    });

    OctaneGUI::Vector2 Position = Button->GetAbsoluteBounds().GetCenter();

    Application.GetMainWindow()->OnMouseMove(Position);
    Application.GetMainWindow()->OnMousePressed(Position, OctaneGUI::Mouse::Button::Left);
    VERIFY(Pressed)
    VERIFY(Button->IsPressed());

    return true;
})

TEST_CASE(Click,
{
    OctaneGUI::ControlList List;
    Application.GetMainWindow()->Load(Json, List);
    Application.GetMainWindow()->Update();

    VERIFY(List.Contains("Button"))

    std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");

    bool Clicked = false;
    Button->SetOnClicked([&](OctaneGUI::Button&) -> void
    {
        Clicked = true;
    });

    OctaneGUI::Vector2 Position = Button->GetAbsoluteBounds().GetCenter();

    Application.GetMainWindow()->OnMouseMove(Position);
    Application.GetMainWindow()->OnMousePressed(Position, OctaneGUI::Mouse::Button::Left);
    VERIFY(!Clicked)
    VERIFY(Button->IsPressed());

    Application.GetMainWindow()->OnMouseReleased(Position, OctaneGUI::Mouse::Button::Left);
    VERIFY(Clicked)

    return true;
})

TEST_CASE(Disabled,
{
    OctaneGUI::ControlList List;
    Application.GetMainWindow()->Load(Json, List);
    Application.GetMainWindow()->Update();

    bool Pressed = false;
    List.To<OctaneGUI::Button>("Button")->SetOnPressed([&](OctaneGUI::Button&) -> void
    {
        Pressed = true;
    })
    .SetDisabled(true);

    OctaneGUI::Vector2 Position = List.To<OctaneGUI::Button>("Button")->GetAbsoluteBounds().GetCenter();

    Application.GetMainWindow()->OnMouseMove(Position);
    Application.GetMainWindow()->OnMousePressed(Position, OctaneGUI::Mouse::Button::Left);
    Application.GetMainWindow()->OnMouseReleased(Position, OctaneGUI::Mouse::Button::Left);

    return !Pressed;
})

)

}
