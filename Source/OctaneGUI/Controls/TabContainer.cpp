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

#include "TabContainer.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "MarginContainer.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

//
// Tab
//

class Tab : public Container
{
public:
    typedef std::function<void(const Tab&)> OnPressedSignature;

    Tab(Window* InWindow)
        : Container(InWindow)
    {
        m_Margins = AddControl<MarginContainer>();
        m_Margins
            ->SetMargins({ 8.0f * RenderScale().X, 2.0f, 2.0f, 4.0f })
            .SetExpand(Expand::Both);

        const std::shared_ptr<VerticalContainer> Contents { m_Margins->AddControl<VerticalContainer>() };
        Contents
            ->SetGrow(Grow::Center)
            .SetExpand(Expand::Both);

        m_Label = Contents->AddControl<Text>();
        m_Label->SetText(U"New Tab");

        m_Interaction = AddControl<Control>();
        m_Interaction->SetForwardMouseEvents(true);

        m_Container = std::make_shared<Container>(InWindow);
    }

    Tab& SetLabel(const char32_t* Label)
    {
        m_Label->SetText(Label);
        return *this;
    }

    const char32_t* Label() const
    {
        return m_Label->GetText();
    }

    const std::shared_ptr<Container>& Contents() const
    {
        return m_Container;
    }

    Tab& Tab::SetSelected(bool Selected)
    {
        if (m_Selected != Selected)
        {
            m_Selected = Selected;
            Invalidate(InvalidateType::Paint);
        }

        return *this;
    }

    bool Tab::Selected() const
    {
        return m_Selected;
    }

    Tab& Tab::SetOnPressed(OnPressedSignature&& Fn)
    {
        m_OnPressed = std::move(Fn);
        return *this;
    }

    virtual Vector2 DesiredSize() const override
    {
        const Vector2 Padding { GetProperty(ThemeProperties::Tab_Padding).Vector() };
        return m_Margins->DesiredSize() + Padding * 2.0f;
    }

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const
    {
        std::weak_ptr<Control> Result;

        if (Contains(Point))
        {
            Result = m_Interaction;
        }

        return Result;
    }

    virtual void OnPaint(Paint& Brush) const override
    {
        const Color Background { (m_Hovered || m_Selected) ? GetProperty(ThemeProperties::Tab_Selected).ToColor() : GetProperty(ThemeProperties::Tab).ToColor() };
        Brush.Rectangle(GetAbsoluteBounds(), Background);

        Container::OnPaint(Brush);
    }

    virtual void OnLoad(const Json& Root) override
    {
        const Json& Label { Root["Label"] };
        SetLabel(String::ToUTF32(Label.String()).data());

        Json Copy { Root };
        Json Controls { Json::Type::Object };
        Controls["Controls"] = std::move(Copy["Controls"]);
        m_Container->OnLoad(Controls);

        Container::OnLoad(Copy);
    }

    virtual bool OnMousePressed(const Vector2&, Mouse::Button, Mouse::Count) override
    {
        if (m_OnPressed != nullptr)
        {
            m_OnPressed(*this);
        }

        return false;
    }

    virtual void OnMouseEnter() override
    {
        m_Hovered = true;
        Invalidate(InvalidateType::Paint);
    }

    virtual void OnMouseLeave() override
    {
        m_Hovered = false;
        Invalidate(InvalidateType::Paint);
    }

private:
    std::shared_ptr<MarginContainer> m_Margins { nullptr };
    std::shared_ptr<Text> m_Label { nullptr };
    std::shared_ptr<Container> m_Container { nullptr };
    std::shared_ptr<Control> m_Interaction { nullptr };
    bool m_Hovered { false };
    bool m_Selected { false };
    OnPressedSignature m_OnPressed { nullptr };
};

//
// TabContainer
//

TabContainer::TabContainer(Window* InWindow)
    : Container(InWindow)
{
    m_Contents = AddControl<VerticalContainer>();
    m_Tabs = m_Contents->AddControl<HorizontalContainer>();
}

std::shared_ptr<Container> TabContainer::AddTab(const char32_t* Label)
{
    const std::shared_ptr<Tab> Result { CreateTab(Label) };
    return Result->Contents();
}

void TabContainer::OnLoad(const Json& Root)
{
    Json Copy { Root };
    Copy["Controls"] = Json();

    Container::OnLoad(Copy);

    const Json& Tabs { Root["Tabs"] };
    for (unsigned int I = 0; I < Tabs.Count(); I++)
    {
        const Json& Item { Tabs[I] };

        const std::shared_ptr<Tab> New { CreateTab(U"") };
        New->OnLoad(Item);

        if (I == 0)
        {
            SetTab(New->Contents());
        }
    }
}

std::shared_ptr<Tab> TabContainer::CreateTab(const char32_t* Label)
{
    const std::shared_ptr<Tab> Result { m_Tabs->AddControl<Tab>() };
    Result
        ->SetLabel(Label)
        .SetOnPressed([this](const Tab& Pressed) -> void
            {
                SetTab(Pressed.Contents());
            });
    return Result;
}

TabContainer& TabContainer::SetTab(const std::shared_ptr<Container>& Contents)
{
    const std::shared_ptr<Container> Current { m_Tab.lock() };

    if (Contents != Current)
    {
        SetTabSelected(Current, false);
        m_Contents->RemoveControl(Current);
        m_Tab = Contents;

        if (Contents != nullptr)
        {
            m_Contents->InsertControl(Contents);
            SetTabSelected(Contents, true);
        }
    }

    return *this;
}

TabContainer& TabContainer::SetTabSelected(const std::shared_ptr<Container>& Contents, bool Selected)
{
    if (Contents == nullptr)
    {
        return *this;
    }

    for (size_t I = 0; I < m_Tabs->NumControls(); I++)
    {
        const std::shared_ptr<Tab>& Item { std::static_pointer_cast<Tab>(m_Tabs->Get(I)) };

        if (Item->Contents() == Contents)
        {
            Item->SetSelected(Selected);
            break;
        }
    }

    return *this;
}

}
