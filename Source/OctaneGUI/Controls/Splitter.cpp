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

#include "Splitter.h"
#include "../Json.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "Separator.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

class SplitterInteraction : public Control
{
    CLASS(SplitterInteraction)

public:
    SplitterInteraction(Window* InWindow, Splitter* Owner)
        : Control(InWindow)
        , m_Owner(Owner)
    {
        SetExpand(Expand::Both);
    }

    virtual void OnMouseMove(const Vector2& Position) override
    {
        bool UpdateMouseCursor = false;
        if (m_Drag)
        {
            const Vector2 Delta = Position - m_Anchor;
            if (m_Owner->GetOrientation() == Orientation::Vertical)
            {
                m_Owner->SetSplitterPosition(m_Owner->m_Separator->GetPosition().X + Delta.X);
            }
            else
            {
                m_Owner->SetSplitterPosition(m_Owner->m_Separator->GetPosition().Y + Delta.Y);
            }
            m_Owner->InvalidateLayout();
            UpdateMouseCursor = true;
        }
        else
        {
            UpdateMouseCursor = m_Owner->m_Separator->Contains(Position);
        }

        m_Anchor = Position;

        if (UpdateMouseCursor)
        {
            GetWindow()->SetMouseCursor(MouseCursor());
        }
    }

    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override
    {
        if (m_Owner->m_Separator->Contains(Position))
        {
            m_Drag = true;
            return true;
        }

        return false;
    }

    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override
    {
        m_Drag = false;

        if (!m_Owner->m_Separator->Contains(Position))
        {
            GetWindow()->SetMouseCursor(Mouse::Cursor::Arrow);
        }
    }

    virtual void OnMouseLeave() override
    {
        GetWindow()->SetMouseCursor(Mouse::Cursor::Arrow);
    }

private:
    Mouse::Cursor MouseCursor() const
    {
        return m_Owner->GetOrientation() == Orientation::Vertical
            ? Mouse::Cursor::SizeWE
            : Mouse::Cursor::SizeNS;
    }

    Splitter* m_Owner { nullptr };
    bool m_Drag { false };
    Vector2 m_Anchor {};
};

Splitter::Splitter(Window* InWindow)
    : Container(InWindow)
{
    m_First = std::make_shared<Container>(InWindow);
    m_Separator = std::make_shared<Separator>(InWindow);
    m_Second = std::make_shared<Container>(InWindow);
    m_Interaction = std::make_shared<SplitterInteraction>(InWindow, this);

    m_Separator->SetProperty(ThemeProperties::Separator_Margins, 0.0f);

    m_First->SetClip(true);
    m_Second->SetClip(true);

    UpdateLayout();
}

Splitter& Splitter::SetOrientation(Orientation InOrientation)
{
    if (m_Separator->GetOrientation() == InOrientation)
    {
        return *this;
    }

    m_Separator->SetOrientation(InOrientation);
    UpdateLayout();
    InvalidateLayout();
    return *this;
}

Orientation Splitter::GetOrientation() const
{
    return m_Separator->GetOrientation();
}

std::shared_ptr<Container> Splitter::First() const
{
    return m_First;
}

std::shared_ptr<Container> Splitter::Second() const
{
    return m_Second;
}

std::weak_ptr<Control> Splitter::GetControl(const Vector2& Point) const
{
    std::weak_ptr<Control> Result = Container::GetControl(Point);

    if (!Result.expired() && Result.lock() == m_Separator)
    {
        Result = m_Interaction;
    }

    return Result;
}

Vector2 Splitter::DesiredSize() const
{
    return m_Split->DesiredSize();
}

void Splitter::Update()
{
    if (m_UpdateLayout)
    {
        const float Position = m_Separator->GetOrientation() == Orientation::Vertical
            ? GetSize().X * 0.5f - m_Separator->GetSize().X * 0.5f
            : GetSize().Y * 0.5f - m_Separator->GetSize().Y * 0.5f;
        SetSplitterPosition(Position);
        m_UpdateLayout = false;
    }
}

void Splitter::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    SetOrientation(ToOrientation(Root["Orientation"].String()));
    m_First->OnLoad(Root["First"]);
    m_Second->OnLoad(Root["Second"]);
}

void Splitter::OnResized()
{
    float Position = 0.0f;

    if (m_Separator->GetOrientation() == Orientation::Vertical)
    {
        Position = m_First->GetSize().X;
    }
    else
    {
        Position = m_First->GetSize().Y;
    }

    SetSplitterPosition(Position);
}

void Splitter::UpdateLayout()
{
    ClearControls();

    m_Split = nullptr;
    if (m_Separator->GetOrientation() == Orientation::Vertical)
    {
        m_Split = AddControl<HorizontalContainer>();
        m_First->SetSize({ GetSize().X * 0.5f - m_Separator->GetSize().X * 0.5f, m_First->GetSize().Y });
    }
    else
    {
        m_Split = AddControl<VerticalContainer>();
        m_First->SetSize({ m_First->GetSize().X, m_First->GetSize().Y * 0.5f - m_Separator->GetSize().Y * 0.5f });
    }

    m_Split
        ->SetSpacing({ 0.0f, 0.0f })
        ->SetExpand(Expand::Both);

    m_Split->InsertControl(m_First);
    m_Split->InsertControl(m_Separator);
    m_Split->InsertControl(m_Second);
    m_Split->InsertControl(m_Interaction);

    m_UpdateLayout = true;
}

void Splitter::SetSplitterPosition(float Position)
{
    const Vector2 Size = GetSize();
    const Vector2 SplitterSize = m_Separator->GetSize();

    if (m_Separator->GetOrientation() == Orientation::Vertical)
    {
        float Width = std::max<float>(Position, 0.0f);
        Width = std::min<float>(Width, Size.X - SplitterSize.X);
        m_First->SetSize({ Width, Size.Y });
        m_Second->SetSize({ Size.X - Width - SplitterSize.X, Size.Y });
    }
    else
    {
        float Height = std::max<float>(Position, 0.0f);
        Height = std::min<float>(Height, Size.Y - SplitterSize.Y);
        m_First->SetSize({ Size.X, Height });
        m_Second->SetSize({ Size.X, Size.Y - Height - SplitterSize.Y });
    }
}

}
