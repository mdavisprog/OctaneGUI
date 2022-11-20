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
#include "../Assert.h"
#include "../Json.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "Separator.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

//
// SplitterInteraction
//

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
        if (m_Drag)
        {
            std::shared_ptr<Separator> Split = m_Hovered.lock();
            std::shared_ptr<Container> Item = m_Owner->GetContainer(Split);
            if (!Item)
            {
                return;
            }

            Vector2 Diff = Position - Item->GetAbsolutePosition() - m_DragOffset;
            Diff.X = Diff.X < 0.0f ? 0.0f : Diff.X;
            Diff.Y = Diff.Y < 0.0f ? 0.0f : Diff.Y;
            if (m_Owner->Fit())
            {
                const Vector2 AvailableSize { GetSize() - Item->GetPosition() };
                const Vector2 Max { AvailableSize.X - Split->GetSize().X, AvailableSize.Y - Split->GetSize().Y };
                Diff.X = Diff.X > Max.X ? Max.X : Diff.X;
                Diff.Y = Diff.Y > Max.Y ? Max.Y : Diff.Y;
            }

            Vector2 Size { Item->GetSize() };
            if (m_Owner->GetOrientation() == Orientation::Horizontal)
            {
                Size.Y = Diff.Y;
            }
            else
            {
                Size.X = Diff.X;
            }

            m_Owner->Resize(Item, Size);
        }
        else
        {
            m_Hovered = m_Owner->GetSeparator(Position);

            if (!m_Hovered.expired())
            {
                GetWindow()->SetMouseCursor(MouseCursor());
            }
            else
            {
                // TODO: Find a better way of reverting mouse cursor to arrow while still
                // allowing hovered control to perform its own mouse cursor setting i.e. TextInput controls.
                // Not a fan of the atomic lock of the weak pointer to shared pointer for checking
                // the control.
                const std::weak_ptr<Control>& Hovered = GetWindow()->Hovered();
                if (!Hovered.expired() && Hovered.lock().get() == this)
                {
                    GetWindow()->SetMouseCursor(Mouse::Cursor::Arrow);
                }
            }
        }
    }

    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override
    {
        if (!m_Hovered.expired())
        {
            m_Drag = true;
            m_DragOffset = Position - m_Hovered.lock()->GetAbsolutePosition();
            return true;
        }

        return false;
    }

    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override
    {
        m_Drag = false;

        if (m_Owner->GetSeparator(Position) == nullptr)
        {
            GetWindow()->SetMouseCursor(Mouse::Cursor::Arrow);
        }
    }

    virtual void OnMouseLeave() override
    {
        if (!m_Drag)
        {
            GetWindow()->SetMouseCursor(Mouse::Cursor::Arrow);
        }
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
    Vector2 m_DragOffset {};
    std::weak_ptr<Separator> m_Hovered {};
};

//
// Splitter
//

Splitter::Splitter(Window* InWindow)
    : Container(InWindow)
{
    m_Interaction = std::make_shared<SplitterInteraction>(InWindow, this);

    UpdateLayout();
}

Splitter& Splitter::SetOrientation(Orientation InOrientation)
{
    if (m_Orientation == InOrientation)
    {
        return *this;
    }

    m_Orientation = InOrientation;
    UpdateLayout();
    InvalidateLayout();
    return *this;
}

Orientation Splitter::GetOrientation() const
{
    return m_Orientation;
}

Splitter& Splitter::SetSplitterPosition(size_t Index, float Position)
{
    Assert(Index < m_Items.size(), "Invalid index %zu given. Maximum number of containers is %zu.", Index, m_Items.size());
    Item& Item_ = m_Items[Index];
    Item_.Position = Position;
    return *this;
}

Vector2 Splitter::SplitterSize() const
{
    if (m_Items.size() < 2)
    {
        return {};
    }

    return m_Items.front().Handle->GetSize();
}

Splitter& Splitter::SetFit(bool Fit)
{
    m_Fit = Fit;
    return *this;
}

bool Splitter::Fit() const
{
    return m_Fit;
}

const std::shared_ptr<Container>& Splitter::GetSplit(size_t Index) const
{
    Assert(Index >= 0 && Index < m_Items.size(), "Invalid index %zu given! Maximum number of containers is %zu.", Index, m_Items.size());
    return m_Items[Index].Data;
}

size_t Splitter::Count() const
{
    return m_Items.size();
}

const std::shared_ptr<Container>& Splitter::AddContainer()
{
    if (m_Items.empty())
    {
        m_Items.push_back({ CreateContainer(), nullptr });
    }
    else
    {
        Item& Back = m_Items.back();
        Back.Handle = m_Split->AddControl<Separator>();
        Back.Handle
            ->SetOrientation(m_Orientation)
            .SetProperty(ThemeProperties::Separator_Margins, 0.0f);
        m_Items.push_back({ CreateContainer(), nullptr });
    }

    m_Resize = true;

    return m_Items.back().Data;
}

Splitter& Splitter::AddContainers(int Count)
{
    while (Count > 0)
    {
        AddContainer();
        Count--;
    }

    return *this;
}

std::weak_ptr<Control> Splitter::GetControl(const Vector2& Point) const
{
    std::weak_ptr<Control> Result = Container::GetControl(Point);

    if (!Result.expired() && IsSeparator(Result.lock()))
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
    if (m_Resize)
    {
        Resize();
        m_Resize = false;
    }
}

void Splitter::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    SetOrientation(ToOrientation(Root["Orientation"].String()));
    SetFit(Root["Fit"].Boolean(Fit()));

    const Json& Containers = Root["Containers"];
    for (unsigned int I = 0; I < Containers.Count(); I++)
    {
        const Json& Sub = Containers[I];
        const std::shared_ptr<Container>& Split = AddContainer();
        Split->OnLoad(Sub);

        Item& Item_ = m_Items.back();
        Item_.Position = Sub["Position"].Number(-1.0f);
    }

    UpdateLayout();
}

void Splitter::UpdateLayout()
{
    ClearControls();
    InsertControl(m_Interaction);

    m_Split = nullptr;
    if (GetOrientation() == Orientation::Vertical)
    {
        m_Split = AddControl<HorizontalContainer>();
    }
    else
    {
        m_Split = AddControl<VerticalContainer>();
    }

    m_Split
        ->SetSpacing({ 0.0f, 0.0f })
        ->SetExpand(GetExpand());
    
    for (const Item& Item_ : m_Items)
    {
        if (Fit())
        {
            Item_.Data->SetExpand(GetOrientation() == Orientation::Horizontal ? Expand::Width : Expand::Height);
        }

        m_Split->InsertControl(Item_.Data);

        if (Item_.Handle)
        {
            Item_.Handle->SetOrientation(GetOrientation());
            m_Split->InsertControl(Item_.Handle);
        }
    }

    m_Resize = true;
}

void Splitter::Resize()
{
    if (Fit())
    {
        Vector2 Size {};
        Vector2 Available { AvailableSize() };
        size_t Index = 0;
        for (Item& Item_ : m_Items)
        {
            const Vector2 ItemSize = Item_.Data->GetSize();

            // Auto-resize.
            if (Item_.Position < 0.0f)
            {
                Item_.Position = 1.0f / (float)(m_Items.size() - Index);
            }

            // The final container will occupy the remaning size.
            if (Index == m_Items.size() - 1)
            {
                Item_.Position = 1.0f;
            }

            Size = Available * Item_.Position;

            if (m_Orientation == Orientation::Horizontal)
            {
                Item_.Data->SetSize({ ItemSize.X, Size.Y });
            }
            else
            {
                Item_.Data->SetSize({ Size.X, ItemSize.Y });
            }

            Available -= Size;
            Index++;
        }
    }
    else
    {
        for (const Item& Item_ : m_Items)
        {
            const Vector2 Size = Item_.Data->ChildrenSize();
            Item_.Data->SetSize(Size);
        }
    }
}

void Splitter::Resize(const std::shared_ptr<Container>& Target, const Vector2& Size)
{
    Target->SetSize(Size);

    if (Fit())
    {
        Vector2 Remaining { AvailableSize() };
        bool Reallocate = false;
        for (Item& Item_ : m_Items)
        {
            if (GetOrientation() == Orientation::Horizontal)
            {
                Item_.Position = Item_.Data->GetSize().Y / Remaining.Y;
            }
            else
            {
                Item_.Position = Item_.Data->GetSize().X / Remaining.X;
            }

            if (!Reallocate)
            {
                Reallocate = Item_.Data == Target;
            }
            else if (m_Items.back().Data == Item_.Data)
            {
                Item_.Position = 1.0f;
                Item_.Data->SetSize(Remaining);
            }

            Remaining -= Item_.Data->GetSize();
        }
    }

    Invalidate(InvalidateType::Both);
}

std::shared_ptr<Separator> Splitter::GetSeparator(const Vector2& Point) const
{
    for (const Item& Item_ : m_Items)
    {
        if (Item_.Handle && Item_.Handle->Contains(Point))
        {
            return Item_.Handle;
        }
    }

    return nullptr;
}

std::shared_ptr<Container> Splitter::GetContainer(const std::shared_ptr<Separator>& Handle) const
{
    for (const Item& Item_ : m_Items)
    {
        if (Item_.Handle && Item_.Handle == Handle)
        {
            return Item_.Data;
        }
    }

    return nullptr;
}

bool Splitter::IsSeparator(const std::shared_ptr<Control>& Handle) const
{
    for (const Item& Item_ : m_Items)
    {
        if (Item_.Handle == Handle)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<Container> Splitter::CreateContainer()
{
    std::shared_ptr<Container> Result = m_Split->AddControl<Container>();
    
    if (Fit())
    {
        Result->SetExpand(GetOrientation() == Orientation::Horizontal ? Expand::Width : Expand::Height);
    }

    return Result;
}

Vector2 Splitter::AvailableSize() const
{
    Vector2 Result { GetSize() };

    if (m_Items.size() >= 2)
    {
        const Vector2 SplitterSize { m_Items.front().Handle->GetSize() };
        Result -= SplitterSize * (float)(m_Items.size() - 1);
    }

    return Result;
}

}
