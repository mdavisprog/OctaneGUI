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
	m_Scrollable->SetOnInvalidate([this](Control* Focus, InvalidateType Type) -> void
		{
			if (Type == InvalidateType::Paint)
			{
				HandleInvalidate(Focus, Type);
			}
			else
			{
				Invalidate(this, Type);
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

std::weak_ptr<Control> ScrollableViewControl::GetControl(const Vector2& Point) const
{
	if (m_Scrollable->IsInScrollBar(Point))
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

	m_Scrollable->OnLoad(Root);
}

}
