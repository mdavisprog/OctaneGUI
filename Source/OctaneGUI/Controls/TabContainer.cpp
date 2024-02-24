/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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
#include "../Application.h"
#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "ImageButton.h"
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
private:
    class CloseButton;

public:
    typedef std::function<void(const Tab&)> OnPressedSignature;

    Tab(Window* InWindow)
        : Container(InWindow)
    {
        m_Margins = AddControl<MarginContainer>();
        m_Margins
            ->SetMargins({ 8.0f * RenderScale().X, 2.0f, 2.0f, 8.0f * RenderScale().X })
            .SetExpand(Expand::Both);

        m_Row = m_Margins->AddControl<HorizontalContainer>();
        m_Row
            ->SetGrow(Grow::Center)
            .SetExpand(Expand::Both);

        std::shared_ptr<VerticalContainer> Contents { m_Row->AddControl<VerticalContainer>() };
        Contents
            ->SetGrow(Grow::Center)
            .SetExpand(Expand::Height);

        m_Label = Contents->AddControl<Text>();
        m_Label->SetText(U"New Tab");

        m_CloseContainer = m_Row->AddControl<VerticalContainer>();
        m_CloseContainer
            ->SetGrow(Grow::Center)
            .SetExpand(Expand::Height);

        m_Close = m_CloseContainer->AddControl<CloseButton>();
        m_Close
            ->SetTab(this)
            .SetOnPressed([this](Button&) -> void
                {
                    if (m_OnClosed != nullptr)
                    {
                        // FIXME: m_OnClosed will remove the Tab object from the TabContainer,
                        // which will destroy this object. Need to prevent the button from
                        // invalidating itself and calling a callback function which will
                        // not exist. Would like a more general approach to cleaning up
                        // destroyed controls.
                        m_Close->SetOnInvalidate(nullptr);
                        m_OnClosed(*this);
                    }
                });

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

    Tab& SetSelected(bool Selected)
    {
        if (m_Selected != Selected)
        {
            m_Selected = Selected;
            Invalidate(InvalidateType::Paint);
        }

        return *this;
    }

    bool Selected() const
    {
        return m_Selected;
    }

    Tab& SetShowClose(bool Show)
    {
        if (Show)
        {
            if (!m_Row->HasControl(m_CloseContainer))
            {
                m_Row->InsertControl(m_CloseContainer);
            }
        }
        else
        {
            m_Row->RemoveControl(m_CloseContainer);
        }

        return *this;
    }

    Tab& SetOnPressed(OnPressedSignature&& Fn)
    {
        m_OnPressed = std::move(Fn);
        return *this;
    }

    Tab& SetOnClosed(OnPressedSignature&& Fn)
    {
        m_OnClosed = std::move(Fn);
        return *this;
    }

    virtual Vector2 DesiredSize() const override
    {
        const Vector2 Padding { GetProperty(ThemeProperties::Tab_Padding).Vector() };
        return m_Margins->DesiredSize() + Padding * 2.0f;
    }

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override
    {
        std::weak_ptr<Control> Result;

        if (Contains(Point))
        {
            Result = m_Interaction;
        }

        if (m_Close->Contains(Point))
        {
            Result = m_Close;
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
    class CloseButton : public ImageButton
    {
    public:
        CloseButton(Window* InWindow)
            : ImageButton(InWindow)
        {
            const float Height { GetProperty(ThemeProperties::FontSize).Float() * 0.75f };
            SetTexture(InWindow->App().GetIcons()->GetTexture());
            SetUVs(InWindow->App().GetIcons()->GetUVs(Icons::Type::Close));
            SetImageSize({ Height, Height });
            SetProperty(ThemeProperties::Button, Color {});
        }

        CloseButton& SetTab(Tab* InTab)
        {
            m_Tab = InTab;
            return *this;
        }

        virtual void OnMouseEnter() override
        {
            ImageButton::OnMouseEnter();
            m_Tab->OnMouseEnter();
        }

        virtual void OnMouseLeave() override
        {
            ImageButton::OnMouseLeave();
            m_Tab->OnMouseLeave();
        }

    private:
        Tab* m_Tab { nullptr };
    };

    std::shared_ptr<MarginContainer> m_Margins { nullptr };
    std::shared_ptr<HorizontalContainer> m_Row { nullptr };
    std::shared_ptr<Text> m_Label { nullptr };
    std::shared_ptr<VerticalContainer> m_CloseContainer { nullptr };
    std::shared_ptr<CloseButton> m_Close { nullptr };
    std::shared_ptr<Container> m_Container { nullptr };
    std::shared_ptr<Control> m_Interaction { nullptr };

    bool m_Hovered { false };
    bool m_Selected { false };

    OnPressedSignature m_OnPressed { nullptr };
    OnPressedSignature m_OnClosed { nullptr };
};

//
// TabContainer
//

TabContainer::TabContainer(Window* InWindow)
    : Container(InWindow)
{
    m_Contents = AddControl<VerticalContainer>();
    m_Tabs = m_Contents->AddControl<HorizontalContainer>();

    const float Height { GetProperty(ThemeProperties::FontSize).Float() };

    const std::shared_ptr<VerticalContainer> Aligner { m_Tabs->AddControl<VerticalContainer>() };
    Aligner
        ->SetGrow(Grow::Center)
        .SetExpand(Expand::Height);
    m_AddTab = Aligner;
    const std::shared_ptr<ImageButton> AddButton { Aligner->AddControl<ImageButton>() };
    AddButton
        ->SetTexture(InWindow->App().GetIcons()->GetTexture())
        .SetUVs(InWindow->App().GetIcons()->GetUVs(Icons::Type::Plus))
        .SetOnPressed([this](Button&) -> void
            {
                CreateTab(U"New Tab");
            })
        .SetProperty(ThemeProperties::Button, Color{})
        .SetSize({ Height, Height });

    if (!m_ShowAdd)
    {
        m_Tabs->RemoveControl(m_AddTab);
    }
}

std::shared_ptr<Container> TabContainer::AddTab(const char32_t* Label)
{
    const std::shared_ptr<Tab> Result { CreateTab(Label) };
    return Result->Contents();
}

TabContainer& TabContainer::SetShowAdd(bool ShowAdd)
{
    if (m_ShowAdd != ShowAdd)
    {
        m_ShowAdd = ShowAdd;

        if (m_ShowAdd)
        {
            m_Tabs->InsertControl(m_AddTab);
        }
        else
        {
            m_Tabs->RemoveControl(m_AddTab);
        }
    }

    return *this;
}

bool TabContainer::ShowAdd() const
{
    return m_ShowAdd;
}

TabContainer& TabContainer::SetShowClose(bool ShowClose)
{
    m_ShowClose = ShowClose;

    for (size_t I = 0; I < m_Tabs->NumControls(); I++)
    {
        if (m_ShowAdd && I == m_Tabs->NumControls() - 1)
        {
            continue;
        }

        const std::shared_ptr<Tab>& Item { std::static_pointer_cast<Tab>(m_Tabs->Get(I)) };
        Item->SetShowClose(ShowClose);
    }

    return *this;
}

bool TabContainer::ShowClose() const
{
    return m_ShowClose;
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

    SetShowAdd(Root["ShowAdd"].Boolean(ShowAdd()));
    SetShowClose(Root["ShowClose"].Boolean(ShowClose()));
}

std::shared_ptr<Tab> TabContainer::CreateTab(const char32_t* Label)
{
    const std::shared_ptr<Tab> Result { std::make_shared<Tab>(GetWindow()) };
    m_Tabs->InsertControl(Result, ShowAdd() ? (int)m_Tabs->NumControls() - 1 : -1);
    Result
        ->SetLabel(Label)
        .SetOnPressed([this](const Tab& Pressed) -> void
            {
                SetTab(Pressed.Contents());
            })
        .SetOnClosed([this](const Tab& Closed) -> void
            {
                RemoveTab(Closed.TShare<Tab>());
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

TabContainer& TabContainer::RemoveTab(const std::shared_ptr<Tab const>& Target)
{
    for (size_t I = 0; I < m_Tabs->NumControls(); I++)
    {
        const std::shared_ptr<Tab>& Item { std::static_pointer_cast<Tab>(m_Tabs->Get(I)) };

        if (Item == Target)
        {
            m_Contents->RemoveControl(Item->Contents());
            m_Tabs->RemoveControl(Item);

            const std::shared_ptr<Container> TabContents { m_Tab.lock() };
            if (Target->Contents() == TabContents && m_Tabs->NumControls() > 0)
            {
                I = I < m_Tabs->NumControls() ? I : m_Tabs->NumControls() - 1;
                if (m_ShowAdd)
                {
                    if (m_Tabs->NumControls() == 1)
                    {
                        I = m_Tabs->NumControls();
                    }
                    else if (I == m_Tabs->NumControls() - 1)
                    {
                        // Should have more than one control is this block.
                        I--;
                    }
                }

                if (I < m_Tabs->NumControls())
                {
                    const std::shared_ptr<Tab>& Current { std::static_pointer_cast<Tab>(m_Tabs->Get(I)) };
                    SetTab(Current->Contents());
                }
            }

            break;
        }
    }

    return *this;
}

}
