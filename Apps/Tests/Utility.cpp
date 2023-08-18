/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

#include <sstream>

namespace Tests
{
namespace Utility
{

void Load(OctaneGUI::Application& Application, const char* MenuBarJson, const char* BodyJson, OctaneGUI::ControlList& List, int WindowWidth, int WindowHeight)
{
    std::stringstream Stream;
    Stream << "{\"Width\": "
        << WindowWidth
        << ", \"Height\": "
        << WindowHeight
        << ", \"MenuBar\": {" << MenuBarJson << "}"
        << ", \"Body\": {\"Controls\": [" << BodyJson
        << "]}}";
    Application.GetMainWindow()->Load(Stream.str().c_str(), List);
    Application.GetMainWindow()->Update();
}

void Load(OctaneGUI::Application& Application, const char* BodyJson, OctaneGUI::ControlList& List)
{
    Load(Application, "", BodyJson, List, 1280, 720);
}

void Load(OctaneGUI::Application& Application, const char* MenuBarJson, const char* BodyJson, OctaneGUI::ControlList& List)
{
    Load(Application, MenuBarJson, BodyJson, List, 1280, 720);
}

void KeyEvent(OctaneGUI::Application& Application, OctaneGUI::Keyboard::Key Key)
{
    Application.GetMainWindow()->OnKeyPressed(Key);
    Application.GetMainWindow()->OnKeyReleased(Key);
}

void MouseMove(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position)
{
    Application.GetMainWindow()->OnMouseMove(Position);
}

void MousePress(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button)
{
    MouseMove(Application, Position);
    Application.GetMainWindow()->OnMousePressed(Position, Button);
}

void MouseRelease(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button)
{
    MouseMove(Application, Position);
    Application.GetMainWindow()->OnMouseReleased(Position, Button);
}

void MouseClick(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button)
{
    MouseMove(Application, Position);
    Application.GetMainWindow()->OnMousePressed(Position, Button);
    Application.GetMainWindow()->OnMouseReleased(Position, Button);
}

void TextEvent(OctaneGUI::Application& Application, const std::u32string& Text)
{
    for (const char32_t Ch : Text)
    {
        Application.GetMainWindow()->OnText((uint32_t)Ch);
    }
}

bool ContextMenu(OctaneGUI::Application& Application, const std::shared_ptr<OctaneGUI::Control>& Control)
{
    bool Opened { false };
    bool Selected { false };
    Control->SetOnCreateContextMenu([&](OctaneGUI::Control*, const std::shared_ptr<OctaneGUI::Menu>& ContextMenu) -> void
        {
            Opened = true;

            ContextMenu
                ->AddItem("Item 1", [&]() -> void
                    {
                        Selected = true;
                    });
        });
    
    MouseClick(Application, Control->GetAbsolutePosition(), OctaneGUI::Mouse::Button::Right);
    Application.Update();
    MouseClick(Application, Control->GetAbsolutePosition() + OctaneGUI::Vector2{8.0f, 8.0f}, OctaneGUI::Mouse::Button::Left);

    return Opened && Selected;
}

}
}
