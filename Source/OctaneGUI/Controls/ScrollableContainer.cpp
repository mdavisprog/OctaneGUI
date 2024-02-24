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

#include "ScrollableContainer.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Timer.h"
#include "../Window.h"
#include "ScrollBar.h"

namespace OctaneGUI
{

#define SCROLL_SPEED 6.0f

ScrollableContainer::ScrollableContainer(Window* InWindow)
    : Container(InWindow)
{
    SetExpand(Expand::Both);

    m_HorizontalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Horizontal);
    m_HorizontalSB
        ->SetOnScrollMin([this](const ScrollBar&) -> void
            {
                m_ScrollOffset = { -SCROLL_SPEED, 0.0f };
                AddOffset(m_ScrollOffset);
                m_ScrollTimer->Start();
            })
        .SetOnScrollMax([this](const ScrollBar&) -> void
            {
                m_ScrollOffset = { SCROLL_SPEED, 0.0f };
                AddOffset(m_ScrollOffset);
                m_ScrollTimer->Start();
            })
        .SetOnRelease([this](const ScrollBar&) -> void
            {
                m_ScrollTimer->Stop();
            })
        .Handle()
        ->SetOnDrag([this](const ScrollBar&) -> void
            {
                const float Size = GetOverflow().X;
                SetOffset({ m_HorizontalSB->Handle()->OffsetPct() * Size, -GetPosition().Y }, false);
                Invalidate(InvalidateType::Paint);
            });
    InsertControl(m_HorizontalSB);

    m_VerticalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Vertical);
    m_VerticalSB
        ->SetOnScrollMin([this](const ScrollBar&) -> void
            {
                m_ScrollOffset = { 0.0f, -SCROLL_SPEED };
                AddOffset(m_ScrollOffset);
                m_ScrollTimer->SetInterval(400);
                m_ScrollTimer->Start();
            })
        .SetOnScrollMax([this](const ScrollBar&) -> void
            {
                m_ScrollOffset = { 0.0f, SCROLL_SPEED };
                AddOffset(m_ScrollOffset);
                m_ScrollTimer->SetInterval(400);
                m_ScrollTimer->Start();
            })
        .SetOnRelease([this](const ScrollBar&) -> void
            {
                m_InitialScroll = true;
                m_ScrollTimer->Stop();
            })
        .Handle()
        ->SetOnDrag([this](const ScrollBar&) -> void
            {
                const float Size = GetOverflow().Y;
                SetOffset({ -GetPosition().X, m_VerticalSB->Handle()->OffsetPct() * Size }, false);
                Invalidate(InvalidateType::Paint);
            });
    InsertControl(m_VerticalSB);

    m_ScrollTimer = GetWindow()->CreateTimer(100, true, [this]() -> void
        {
            if (m_InitialScroll)
            {
                m_InitialScroll = false;
                m_ScrollTimer->SetInterval(100);
            }

            AddOffset(m_ScrollOffset);
        });
}

bool ScrollableContainer::IsInScrollBar(const Vector2& Point) const
{
    if (m_HorizontalSB->ShouldPaint() && m_HorizontalSB->Contains(Point))
    {
        return true;
    }

    if (m_VerticalSB->ShouldPaint() && m_VerticalSB->Contains(Point))
    {
        return true;
    }

    return false;
}

bool ScrollableContainer::IsScrollBarVisible(const std::shared_ptr<Control>& Item) const
{
    if (m_HorizontalSB->Handle() == Item)
    {
        return m_HorizontalSB->ShouldPaint();
    }

    if (m_VerticalSB->Handle() == Item)
    {
        return m_VerticalSB->ShouldPaint();
    }

    return false;
}

bool ScrollableContainer::IsScrolling() const
{
    if (m_HorizontalSB->Handle()->Drag())
    {
        return true;
    }

    if (m_VerticalSB->Handle()->Drag())
    {
        return true;
    }

    return false;
}

Vector2 ScrollableContainer::ContentSize() const
{
    return m_ContentSize;
}

Vector2 ScrollableContainer::Overflow() const
{
    return GetOverflow();
}

Vector2 ScrollableContainer::Offset() const
{
    return -GetPosition();
}

const std::shared_ptr<ScrollBar>& ScrollableContainer::HorizontalScrollBar() const
{
    return m_HorizontalSB;
}

const std::shared_ptr<ScrollBar>& ScrollableContainer::VerticalScrollBar() const
{
    return m_VerticalSB;
}

ScrollableContainer& ScrollableContainer::SetHorizontalSBEnabled(bool Enabled)
{
    m_HorizontalSB->SetEnabled(Enabled);
    return *this;
}

ScrollableContainer& ScrollableContainer::SetVerticalSBEnabled(bool Enabled)
{
    m_VerticalSB->SetEnabled(Enabled);
    return *this;
}

ScrollableContainer& ScrollableContainer::SetOffset(const Vector2& Offset)
{
    SetOffset(Offset, true);
    return *this;
}

ScrollableContainer& ScrollableContainer::AddOffset(const Vector2& Delta)
{
    SetOffset(Delta - GetPosition());
    return *this;
}

ScrollableContainer& ScrollableContainer::ScrollIntoView(const std::shared_ptr<Control>& Item)
{
    const Vector2 Offset = Item->GetAbsolutePosition() - GetAbsolutePosition();
    SetOffset(Offset);
    return *this;
}

Vector2 ScrollableContainer::GetScrollableSize() const
{
    const float SBSize = ScrollBarPropertySize();
    return {
        GetSize().X - (m_VerticalSB->ShouldPaint() ? SBSize : 0.0f),
        GetSize().Y - (m_HorizontalSB->ShouldPaint() ? SBSize : 0.0f)
    };
}

float ScrollableContainer::ScrollBarPropertySize() const
{
    // The property size does not care about orientation. It is a constant scaled by a
    // height factor of the rendering output.
    return m_VerticalSB->PropertySize();
}

ScrollableContainer& ScrollableContainer::SetScrollSpeed(const Vector2& ScrollSpeed)
{
    m_ScrollSpeed = ScrollSpeed;
    return *this;
}

Vector2 ScrollableContainer::ScrollSpeed() const
{
    return m_ScrollSpeed;
}

ScrollableContainer& ScrollableContainer::SetInfinite(bool Infinite)
{
    m_Infinite = Infinite;
    return *this;
}

bool ScrollableContainer::Infinite() const
{
    return m_Infinite;
}

ScrollableContainer& ScrollableContainer::SetOnScroll(OnScrollSignature&& Fn)
{
    m_OnScroll = std::move(Fn);
    return *this;
}

std::weak_ptr<Control> ScrollableContainer::GetControl(const Vector2& Point) const
{
    if (m_HorizontalSB->ShouldPaint() && m_HorizontalSB->Contains(Point))
    {
        return m_HorizontalSB->GetControl(Point);
    }

    if (m_VerticalSB->ShouldPaint() && m_VerticalSB->Contains(Point))
    {
        return m_VerticalSB->GetControl(Point);
    }

    const Rect Bounds = TranslatedBounds();
    if (!Bounds.Contains(Point))
    {
        return std::weak_ptr<Control>();
    }

    // This still may return a scrollbar. Parent containers can check for scrollbar visibility
    // with IsScrollBarVisible function.
    return Container::GetControl(Point);
}

void ScrollableContainer::Update()
{
    m_ContentSize = GetContentSize(Controls());
    UpdateScrollBarSizes();
    SetOffset(-GetPosition(), true);
}

void ScrollableContainer::OnPaint(Paint& Brush) const
{
    // Need to translate from scrolled space.
    Brush.PushClip(TranslatedBounds());

    for (const std::shared_ptr<Control>& Item : Controls())
    {
        if (Item != m_HorizontalSB && Item != m_VerticalSB)
        {
            if (!Brush.IsClipped(Item->GetAbsoluteBounds()))
            {
                Item->OnPaint(Brush);
            }
        }
    }

    Brush.PopClip();

    if (m_HorizontalSB->ShouldPaint())
    {
        m_HorizontalSB->OnPaint(Brush);
    }

    if (m_VerticalSB->ShouldPaint())
    {
        m_VerticalSB->OnPaint(Brush);
    }
}

void ScrollableContainer::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);
    SetExpand(Expand::Both);
}

void ScrollableContainer::OnSave(Json& Root) const
{
    Container::OnSave(Root);

    Root["ContentSize"] = Vector2::ToJson(m_ContentSize);
    Root["ScrollOffset"] = Vector2::ToJson(m_ScrollOffset);
}

void ScrollableContainer::OnMouseMove(const Vector2& Position)
{
    m_HorizontalSB->OnMouseMove(Position);
    m_VerticalSB->OnMouseMove(Position);
}

bool ScrollableContainer::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    if (m_HorizontalSB->OnMousePressed(Position, Button, Count))
    {
        return true;
    }

    if (m_VerticalSB->OnMousePressed(Position, Button, Count))
    {
        return true;
    }

    return false;
}

void ScrollableContainer::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
    m_HorizontalSB->OnMouseReleased(Position, Button);
    m_VerticalSB->OnMouseReleased(Position, Button);
}

void ScrollableContainer::OnMouseWheel(const Vector2& Delta)
{
    AddOffset(-Delta * m_ScrollSpeed);
}

void ScrollableContainer::OnResized()
{
    const Vector2 Position = GetPosition();
    m_ContentSize = GetContentSize(Controls());
    SetOffset(-Position, true);
    UpdateScrollBarSizes();
}

void ScrollableContainer::OnThemeLoaded()
{
    Container::OnThemeLoaded();

    UpdateScrollBarSizes();
    UpdateScrollBarPositions();
}

Rect ScrollableContainer::TranslatedBounds() const
{
    const Vector2 Position = GetAbsolutePosition() - GetPosition();
    return { Position, Position + GetSize() };
}

Vector2 ScrollableContainer::GetContentSize(const std::vector<std::shared_ptr<Control>>& Controls) const
{
    Vector2 Result;

    for (const std::shared_ptr<Control>& Item : Controls)
    {
        if (Item != m_HorizontalSB && Item != m_VerticalSB && !m_HorizontalSB->HasControl(Item) && !m_VerticalSB->HasControl(Item))
        {
            Vector2 Size = Item->GetSize();
            const std::shared_ptr<Container> ItemContainer = std::dynamic_pointer_cast<Container>(Item);
            if (ItemContainer)
            {
                Size = ItemContainer->DesiredSize();
            }

            Result.X = std::max<float>(Result.X, Size.X);
            Result.Y = std::max<float>(Result.Y, Size.Y);
        }
    }

    return Result;
}

Vector2 ScrollableContainer::GetOverflow() const
{
    const float SBSize = ScrollBarPropertySize();
    return {
        std::max<float>(m_ContentSize.X - GetSize().X + (m_VerticalSB->ShouldPaint() ? SBSize : 0.0f), 0.0f),
        std::max<float>(m_ContentSize.Y - GetSize().Y + (m_HorizontalSB->ShouldPaint() ? SBSize : 0.0f), 0.0f)
    };
}

void ScrollableContainer::SetOffset(const Vector2& Offset, bool UpdateSBHandles)
{
    const Vector2 Overflow = GetOverflow();
    const Vector2 Position = {
        std::max<float>(-Overflow.X, std::min<float>(-Offset.X, 0.0f)),
        std::max<float>(-Overflow.Y, std::min<float>(-Offset.Y, 0.0f))
    };

    if (!m_Infinite)
    {
        const Vector2 Delta = GetPosition() - Position;
        SetPosition(Position);

        if (m_OnScroll)
        {
            m_OnScroll(Delta);
        }
    }

    if (UpdateSBHandles)
    {
        const Vector2 ScrollOffset = {
            -Position.X / (Overflow.X > 0.0f ? Overflow.X : 1.0f),
            -Position.Y / (Overflow.Y > 0.0f ? Overflow.Y : 1.0f)
        };
        m_HorizontalSB->Handle()->SetOffset(ScrollOffset.X * m_HorizontalSB->Handle()->GetAvailableScrollSize());
        m_VerticalSB->Handle()->SetOffset(ScrollOffset.Y * m_VerticalSB->Handle()->GetAvailableScrollSize());
    }

    UpdateScrollBarPositions();
}

void ScrollableContainer::UpdateScrollBarSizes()
{
    const Vector2 Size = GetSize();
    const Vector2 Overflow = GetOverflow();
    const float SBSize = ScrollBarPropertySize();

    m_HorizontalSB->SetScrollBarSize({ Size.X, SBSize });
    m_HorizontalSB->Handle()->SetHandleSize(Overflow.X > 0.0f ? m_HorizontalSB->GetScrollBarSize().X - Overflow.X : 0.0f);

    m_VerticalSB->SetScrollBarSize({ SBSize, Size.Y - (m_HorizontalSB->ShouldPaint() ? SBSize : 0.0f) });
    m_VerticalSB->Handle()->SetHandleSize(Overflow.Y > 0.0f ? m_VerticalSB->GetScrollBarSize().Y - Overflow.Y : 0.0f);
}

void ScrollableContainer::UpdateScrollBarPositions()
{
    const Vector2 Size = GetSize();
    const float SBSize = ScrollBarPropertySize();

    m_HorizontalSB->SetPosition({ -GetPosition().X, -GetPosition().Y + Size.Y - SBSize });
    m_VerticalSB->SetPosition({ -GetPosition().X + Size.X - SBSize, -GetPosition().Y });
}

bool ScrollableContainer::IsScrollBarControl(Control* Focus) const
{
    return m_HorizontalSB.get() == Focus
        || m_HorizontalSB->Handle().get() == Focus
        || m_VerticalSB.get() == Focus
        || m_VerticalSB->Handle().get() == Focus;
}

}
