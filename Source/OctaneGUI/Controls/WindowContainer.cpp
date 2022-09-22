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

#include "WindowContainer.h"
#include "../Json.h"
#include "../String.h"
#include "../Window.h"
#include "ControlList.h"
#include "HorizontalContainer.h"
#include "MenuBar.h"
#include "Panel.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

class TitleBarInteraction : public Control
{
public:
    TitleBarInteraction(Window* InWindow)
        : Control(InWindow)
    {
    }

    virtual void OnMouseMove(const Vector2& Position) override
    {
        if (m_Move)
        {
            const Vector2 Delta = Position - m_LastPosition;
            const Vector2 New = GetWindow()->GetPosition() + Delta;
            GetWindow()->SetPosition(New);
        }

        m_LastPosition = Position;
    }

    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override
    {
        m_Move = true;
        return true;
    }

    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override
    {
        m_Move = false;
    }

private:
    bool m_Move { false };
    Vector2 m_LastPosition {};
};

class TitleBar : public Container
{
public:
    TitleBar(Window* InWindow)
        : Container(InWindow)
    {
        SetExpand(Expand::Width);

        AddControl<Panel>()->SetExpand(Expand::Both);

        const std::shared_ptr<HorizontalContainer> Layout = AddControl<HorizontalContainer>();
        m_Title = Layout->AddControl<Text>();

        AddControl<TitleBarInteraction>()->SetExpand(Expand::Both);

        SetSize({ 0.0f, m_Title->LineHeight() });
    }

    TitleBar& SetTitle(const char32_t* Title)
    {
        m_Title->SetText(Title);
        return *this;
    }

    virtual void OnThemeLoaded() override
    {
        Container::OnThemeLoaded();
        SetSize({ 0.0f, m_Title->LineHeight() });
    }

private:
    std::shared_ptr<Text> m_Title { nullptr };
};

WindowContainer::WindowContainer(Window* InWindow)
    : VerticalContainer(InWindow)
{
    SetExpand(Expand::Both);
    SetSpacing({ 0.0f, 0.0f });

    m_MenuBar = AddControl<MenuBar>();
    m_Body = AddControl<Container>();
    m_Body->SetExpand(Expand::Both);
}

WindowContainer& WindowContainer::Clear()
{
    m_MenuBar->ClearMenuItems();
    m_Body->ClearControls();
    return *this;
}

WindowContainer& WindowContainer::CloseMenuBar()
{
    m_MenuBar->Close();
    return *this;
}

WindowContainer& WindowContainer::ShowTitleBar(bool Show)
{
    if (Show)
    {
        if (!m_TitleBar)
        {
            m_TitleBar = std::make_shared<TitleBar>(GetWindow());
            InsertControl(m_TitleBar, 0);
            SetTitle(String::ToUTF32(GetWindow()->GetTitle()).c_str());
        }
    }
    else if (m_TitleBar)
    {
        RemoveControl(m_TitleBar);
        m_TitleBar = nullptr;
    }

    return *this;
}

WindowContainer& WindowContainer::SetTitle(const char32_t* Title)
{
    if (m_TitleBar)
    {
        m_TitleBar->SetTitle(Title);
    }

    return *this;
}

bool WindowContainer::IsInTitleBar(const Vector2& Position) const
{
    if (!m_TitleBar)
    {
        return false;
    }

    return m_TitleBar->Contains(Position);
}

const std::shared_ptr<Container>& WindowContainer::Body() const
{
    return m_Body;
}

const std::shared_ptr<MenuBar>& WindowContainer::GetMenuBar() const
{
    return m_MenuBar;
}

std::weak_ptr<Control> WindowContainer::GetControl(const Vector2& Point) const
{
    std::weak_ptr<Control> Result;

    if (m_TitleBar)
    {
        Result = m_TitleBar->GetControl(Point);
    }
    
    if (Result.expired())
    {
        Result = m_MenuBar->GetControl(Point);
    }

    if (Result.expired())
    {
        Result = m_Body->GetControl(Point);
    }

    return Result;
}

void WindowContainer::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    const Json& MB = Root["MenuBar"];
    const Json& Body = Root["Body"];

    m_MenuBar->OnLoad(MB);
    m_Body->OnLoad(Body);
    m_Body->SetExpand(Expand::Both);

    Invalidate(InvalidateType::Both);
}

}
