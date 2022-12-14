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
#include "../Icons.h"
#include "../Json.h"
#include "../String.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "ImageButton.h"
#include "MenuBar.h"
#include "Panel.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

class TitleBar : public Container
{
public:
    TitleBar(Window* InWindow)
        : Container(InWindow)
    {
        SetExpand(Expand::Width);

        m_Background = AddControl<Panel>();
        m_Background->SetExpand(Expand::Both);

        const std::shared_ptr<HorizontalContainer> Root = AddControl<HorizontalContainer>();
        Root->SetExpand(Expand::Width);

        m_Draggable = Root->AddControl<HorizontalContainer>();
        m_Draggable->SetExpand(Expand::Both);

        const std::shared_ptr<VerticalContainer> TextContainer = m_Draggable->AddControl<VerticalContainer>();
        TextContainer
            ->SetGrow(Grow::Center)
            ->SetExpand(Expand::Height);
        m_Title = TextContainer->AddControl<Text>();

        const std::shared_ptr<VerticalContainer> RightLayout = Root->AddControl<VerticalContainer>();
        RightLayout
            ->SetGrow(Grow::Center)
            ->SetExpand(Expand::Height);

        const std::shared_ptr<HorizontalContainer> Buttons = RightLayout->AddControl<HorizontalContainer>();
        Buttons
            ->SetGrow(Grow::End)
            ->SetExpand(Expand::Height);

        if (GetWindow()->CanMinimize())
        {
            m_Minimize = AddButton(Icons::Type::Minimize, Buttons);
            m_Minimize->SetOnClicked([this](Button&) -> void
                {
                    GetWindow()->Minimize();
                });
        }

        if (GetWindow()->IsResizable())
        {
            m_Maximize = AddButton(Icons::Type::Maximize, Buttons);
            m_Maximize->SetOnClicked([this](Button&) -> void
                {
                    GetWindow()->Maximize();
                });
            SetMaximized(GetWindow()->IsMaximized());
        }

        m_Close = AddButton(Icons::Type::Close, Buttons);
        m_Close->SetOnClicked([this](Button&) -> void
            {
                GetWindow()->RequestClose();
            });

        OnThemeLoaded();
    }

    TitleBar& SetTitle(const char32_t* Title)
    {
        m_Title->SetText(Title);
        return *this;
    }

    TitleBar& SetMaximized(bool Maximized)
    {
        if (!m_Maximize)
        {
            return *this;
        }

        const Icons::Type IconType = Maximized ? Icons::Type::Restore : Icons::Type::Maximize;
        const Rect UVs = GetWindow()->GetIcons()->GetUVs(IconType);
        m_Maximize->SetUVs(UVs);

        return *this;
    }

    TitleBar& SetFocused(bool Focused)
    {
        const Variant& BackgroundColor = Focused ? GetProperty(ThemeProperties::Window_Focused) : GetProperty(ThemeProperties::Panel);
        m_Background->SetProperty(ThemeProperties::Panel, BackgroundColor);

        const Variant& TextColor = Focused ? GetProperty(ThemeProperties::Window_Title_Focused) : GetProperty(ThemeProperties::Text);
        m_Title->SetProperty(ThemeProperties::Text, TextColor);

        Invalidate(InvalidateType::Paint);
        return *this;
    }

    bool IsDraggable(const Vector2& Position) const
    {
        return m_Draggable->Contains(Position);
    }

    virtual void OnThemeLoaded() override
    {
        Container::OnThemeLoaded();
        SetFocused(GetWindow()->Focused());
        SetSize({ 0.0f, Height() });
    }

private:
    float Height() const
    {
        return std::max<float>(m_Title->GetSize().Y, m_Close->GetSize().Y);
    }

    std::shared_ptr<ImageButton> AddButton(Icons::Type Type, const std::shared_ptr<Container>& Parent)
    {
        std::shared_ptr<ImageButton> Result = Parent->AddControl<ImageButton>();

        Result
            ->SetTexture(GetWindow()->GetIcons()->GetTexture())
            .SetUVs(GetWindow()->GetIcons()->GetUVs(Type))
            .SetProperty(ThemeProperties::Button, Color {})
            .SetExpand(Expand::Height);

        return Result;
    }

    std::shared_ptr<Panel> m_Background { nullptr };
    std::shared_ptr<Text> m_Title { nullptr };
    std::shared_ptr<BoxContainer> m_Draggable { nullptr };
    std::shared_ptr<ImageButton> m_Minimize { nullptr };
    std::shared_ptr<ImageButton> m_Maximize { nullptr };
    std::shared_ptr<ImageButton> m_Close { nullptr };
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

WindowContainer& WindowContainer::SetMaximized(bool Maximized)
{
    if (!m_TitleBar)
    {
        return *this;
    }

    m_TitleBar->SetMaximized(Maximized);
    return *this;
}

WindowContainer& WindowContainer::SetFocused(bool Focused)
{
    if (!m_TitleBar)
    {
        return *this;
    }

    m_TitleBar->SetFocused(Focused);
    return *this;
}

bool WindowContainer::IsInTitleBar(const Vector2& Position) const
{
    if (!m_TitleBar)
    {
        return false;
    }

    return m_TitleBar->IsDraggable(Position);
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
