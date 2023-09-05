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

#include "Mouse.h"
#include "../Application.h"
#include "../Controls/ControlList.h"
#include "../Controls/Panel.h"
#include "../Controls/Text.h"
#include "../Controls/VerticalContainer.h"
#include "../String.h"
#include "../Window.h"

namespace OctaneGUI
{
namespace Tools
{

//
// MouseContainer
//

class MouseContainer : public Container
{
public:
    MouseContainer(Window* Window_)
        : Container(Window_)
    {
        SetExpand(Expand::Both);

        AddControl<Panel>()->SetExpand(Expand::Both);

        const std::shared_ptr<Container> Contents = AddControl<VerticalContainer>();
        Contents->SetExpand(Expand::Width);

        m_Window = Contents->AddControl<Text>();
        m_Window->SetText("Window:");

        m_Position = Contents->AddControl<Text>();
        m_Position->SetText("Position:");

        m_Focused = Contents->AddControl<Text>();
        m_Focused->SetText("Focused:");

        m_Hovered = Contents->AddControl<Text>();
        m_Hovered->SetText("Hovered:");
    }

    MouseContainer& SetWindowTarget(const std::shared_ptr<Window>& Window_)
    {
        std::string Contents { "Window: " };
        Contents += Window_->GetTitle();
        m_Window->SetText(Contents.c_str());

        const std::shared_ptr<Control> Focused { Window_->Focus().lock() };
        Contents = "Focused: ";
        Contents += Focused != nullptr ? Focused->GetType() : "None";
        m_Focused->SetText(Contents.c_str());

        const std::shared_ptr<Control> Hovered { Window_->Hovered().lock() };
        Contents = "Hovered: ";
        Contents += Hovered != nullptr ? Hovered->GetType() : "None";
        m_Hovered->SetText(Contents.c_str());

        return *this;
    }

    MouseContainer& SetPosition(const Vector2& Position)
    {
        std::string Contents { "Position: " };
        Contents += std::to_string(Position.X) + " " + std::to_string(Position.Y);
        m_Position->SetText(Contents.c_str());
        return *this;
    }

private:
    std::shared_ptr<Text> m_Window { nullptr };
    std::shared_ptr<Text> m_Position { nullptr };
    std::shared_ptr<Text> m_Focused { nullptr };
    std::shared_ptr<Text> m_Hovered { nullptr };
};

//
// Mouse
//

Mouse::Mouse()
{
}

Mouse& Mouse::Show(Application& App)
{
#define ID "__TOOLS__.Mouse"

    if (!App.HasWindow(ID))
    {
        const char* Stream = R"({"Title": "Mouse", "Width": 400, "Height": 200})";

        ControlList List;
        const std::shared_ptr<Window> Window_ = App.NewWindow(ID, Stream, List);
        m_Container = Window_->GetContainer()->AddControl<MouseContainer>();
    }

    App.DisplayWindow(ID);

    return *this;
}

Mouse& Mouse::OnMove(const std::shared_ptr<Window>& Target, const Vector2& Position)
{
    return Update(Target, Position);
}

Mouse& Mouse::OnPressed(const std::shared_ptr<Window>& Target, const Vector2& Position, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count)
{
    return Update(Target, Position);
}

Mouse& Mouse::OnReleased(const std::shared_ptr<Window>& Target, const Vector2& Position, OctaneGUI::Mouse::Button)
{
    return Update(Target, Position);
}

Mouse& Mouse::Update(const std::shared_ptr<Window>& Target, const Vector2& Position)
{
    if (m_Container == nullptr)
    {
        return *this;
    }

    m_Container
        ->SetWindowTarget(Target)
        .SetPosition(Position);

    return *this;
}

}
}
