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

#include "ScrollableViewControl.h"
#include "../Json.h"
#include "ScrollableContainer.h"

namespace OctaneGUI
{

//
// ScrollableViewInteraction
//

ScrollableViewInteraction::ScrollableViewInteraction(Window* InWindow)
    : Control(InWindow)
{
}

ScrollableViewControl& ScrollableViewInteraction::ScrollableView() const
{
    return *static_cast<ScrollableViewControl*>(GetParent());
}

ScrollableViewInteraction& ScrollableViewInteraction::SetAlwaysFocus(bool AlwaysFocus)
{
    m_AlwaysFocus = AlwaysFocus;
    return *this;
}

bool ScrollableViewInteraction::AlwaysFocus() const
{
    return m_AlwaysFocus;
}

void ScrollableViewInteraction::OnMouseMove(const Vector2& Position)
{
    if (m_AlwaysFocus)
    {
        ScrollableView().OnMouseMove(Position);
    }
}

bool ScrollableViewInteraction::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    if (m_AlwaysFocus)
    {
        return ScrollableView().OnMousePressed(Position, Button, Count);
    }

    return false;
}

void ScrollableViewInteraction::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
    if (m_AlwaysFocus)
    {
        ScrollableView().OnMouseReleased(Position, Button);
    }
}

void ScrollableViewInteraction::OnMouseWheel(const Vector2& Delta)
{
    ScrollableView().Scrollable()->OnMouseWheel(Delta);
}

//
// ScrollableViewControl
//

ScrollableViewControl::ScrollableViewControl(Window* InWindow)
    : Container(InWindow)
{
    m_Scrollable = AddControl<ScrollableContainer>();
    m_Scrollable->SetOnInvalidate([this](std::shared_ptr<Control> Focus, InvalidateType Type) -> void
        {
            if (IsInLayout())
            {
                return;
            }

            if (Type == InvalidateType::Paint)
            {
                HandleInvalidate(Focus, Type);
            }
            else
            {
                Invalidate(Type);
            }
        });

    // The interaction should not have a size. This allows for the mouse to pass through to grab the control within the scrollable contents.
    // The GetControl function will check to make sure the Point is within this container itself.
    m_Interaction = AddControl<ScrollableViewInteraction>();
}

ScrollableViewControl& ScrollableViewControl::SetInteraction(const std::shared_ptr<ScrollableViewInteraction>& Interaction)
{
    if (m_Interaction)
    {
        RemoveControl(m_Interaction);
    }

    m_Interaction = Interaction;

    if (m_Interaction)
    {
        InsertControl(m_Interaction);
    }
    return *this;
}

const std::shared_ptr<ScrollableViewInteraction>& ScrollableViewControl::Interaction() const
{
    return m_Interaction;
}

const std::shared_ptr<ScrollableContainer>& ScrollableViewControl::Scrollable() const
{
    return m_Scrollable;
}

ScrollableViewControl& ScrollableViewControl::SetIgnoreOwnedControls(bool IgnoreOwnedControls)
{
    m_IgnoreOwnedControls = IgnoreOwnedControls;
    return *this;
}

ScrollableViewControl& ScrollableViewControl::SetPendingFocus(const std::weak_ptr<Control>& PendingFocus)
{
    m_PendingFocus = PendingFocus;
    InvalidateLayout();
    return *this;
}

std::weak_ptr<Control> ScrollableViewControl::GetControl(const Vector2& Point) const
{
    if (m_Scrollable->IsInScrollBar(Point) && !AlwaysFocusInteraction())
    {
        return m_Scrollable->GetControl(Point);
    }

    std::weak_ptr<Control> Result = Container::GetControl(Point);
    if (!Result.expired())
    {
        if (!m_IgnoreOwnedControls || !HasControl(Result.lock()))
        {
            return Result;
        }
    }

    if (Contains(Point) && m_Interaction)
    {
        return m_Interaction;
    }

    return std::weak_ptr<Control>();
}

void ScrollableViewControl::OnLoad(const Json& Root)
{
    Json Copy = Root;
    Copy["Controls"] = Json();

    Container::OnLoad(Copy);

    // Prevent the ID to be set for the scrollable container. The ScrollableViewControl should be the only container with the desired ID.
    Copy["ID"] = Json();
    Copy["Controls"] = std::move(Root["Controls"]);
    m_Scrollable->OnLoad(Copy);
}

void ScrollableViewControl::OnMouseMove(const Vector2& Position)
{
    m_Scrollable->OnMouseMove(Position);
}

bool ScrollableViewControl::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    return m_Scrollable->OnMousePressed(Position, Button, Count);
}

void ScrollableViewControl::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
    m_Scrollable->OnMouseReleased(Position, Button);
}

void ScrollableViewControl::OnLayoutComplete()
{
    if (!m_PendingFocus.expired())
    {
        Scrollable()->ScrollIntoView(m_PendingFocus.lock());
        m_PendingFocus.reset();
    }
}

bool ScrollableViewControl::AlwaysFocusInteraction() const
{
    if (!m_Interaction)
    {
        return false;
    }

    return m_Interaction->AlwaysFocus();
}

}
