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

#include "../Paint.h"
#include "ScrollableContainer.h"
#include "ScrollBar.h"

namespace OctaneUI
{

ScrollableContainer::ScrollableContainer(Window* InWindow)
	: Container(InWindow)
{
	SetExpand(Expand::Both);

	m_HorizontalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Horizontal);
	m_HorizontalSB->SetOnDrag([this](ScrollBar*) -> void
	{
		const float SBSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();
		const float Size = m_ContentSize.X - GetSize().X + (m_VerticalSB->HasHandle() ? SBSize : 0.0f);
		SetOffset({m_HorizontalSB->OffsetPct() * Size, m_VerticalSB->Offset()}, false);
		InvalidateLayout();
	});
	InsertControl(m_HorizontalSB);

	m_VerticalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Vertical);
	m_VerticalSB->SetOnDrag([this](ScrollBar*) -> void
	{
		const float SBSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();
		const float Size = m_ContentSize.Y - GetSize().Y + (m_HorizontalSB->HasHandle() ? SBSize : 0.0f);
		SetOffset({m_HorizontalSB->Offset(), m_VerticalSB->OffsetPct() * Size}, false);
		InvalidateLayout();
	});
	InsertControl(m_VerticalSB);
}

bool ScrollableContainer::IsInScrollBar(const Vector2& Point) const
{
	if (m_HorizontalSB->HasHandle() && m_HorizontalSB->Contains(Point))
	{
		return true;
	}

	if (m_VerticalSB->HasHandle() && m_VerticalSB->Contains(Point))
	{
		return true;
	}

	return false;
}

bool ScrollableContainer::IsScrollBarVisible(const std::shared_ptr<Control>& Item) const
{
	if (m_HorizontalSB == Item)
	{
		return m_HorizontalSB->HasHandle();
	}

	if (m_VerticalSB == Item)
	{
		return m_VerticalSB->HasHandle();
	}

	return false;
}

Vector2 ScrollableContainer::ContentSize() const
{
	return m_ContentSize;
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

Vector2 ScrollableContainer::GetScrollableSize() const
{
	const float SBSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();
	return {
		GetSize().X - (m_VerticalSB->HasHandle() ? SBSize : 0.0f),
		GetSize().Y - (m_HorizontalSB->HasHandle() ? SBSize : 0.0f)
	};
}

std::weak_ptr<Control> ScrollableContainer::GetControl(const Vector2& Point) const
{
	if (m_HorizontalSB->HasHandle() && m_HorizontalSB->Contains(Point))
	{
		return m_HorizontalSB;
	}

	if (m_VerticalSB->HasHandle() && m_VerticalSB->Contains(Point))
	{
		return m_VerticalSB;
	}

	// This still may return a scrollbar. Parent containers can check for scrollbar visibility
	// with IsScrollBarVisible function.
	// TODO: May need to transform the point before passing up to parent.
	return Container::GetControl(Point);
}

void ScrollableContainer::Update()
{
	// Not a fan of doing this twice. Once here, and once in PlaceControls.
	// Would be nice for PlaceControls to use m_ContentSize.
	std::vector<std::shared_ptr<Control>> AllControls;
	GetControls(AllControls);

	m_ContentSize = GetContentSize(AllControls);
}

void ScrollableContainer::OnPaint(Paint& Brush) const
{
	// Need to translate from scrolled space.
	Brush.PushClip(TranslatedBounds());

	for (const std::shared_ptr<Control>& Item : Controls())
	{
		if (Item != m_HorizontalSB && Item != m_VerticalSB)
		{
			Item->OnPaint(Brush);
		}
	}

	Brush.PopClip();

	m_HorizontalSB->OnPaint(Brush);
	m_VerticalSB->OnPaint(Brush);
}

void ScrollableContainer::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);
	SetExpand(Expand::Both);
}

void ScrollableContainer::OnMouseMove(const Vector2& Position)
{
	m_HorizontalSB->OnMouseMove(Position);
	m_VerticalSB->OnMouseMove(Position);
}

bool ScrollableContainer::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	if (m_HorizontalSB->OnMousePressed(Position, Button))
	{
		return true;
	}

	if (m_VerticalSB->OnMousePressed(Position, Button))
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

void ScrollableContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Container::PlaceControls(Controls);

	std::vector<std::shared_ptr<Control>> AllControls;
	GetControls(AllControls);

	const Vector2 ContentSize = GetContentSize(AllControls);
	const Vector2 Size = GetSize();
	const Vector2 Overflow = ContentSize - Size;
	const float SBSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();

	m_HorizontalSB
		->SetHandleSize(Overflow.X > 0.0f ? Size.X - Overflow.X : 0.0f)
		.SetPosition({-GetPosition().X, -GetPosition().Y + Size.Y - SBSize})
		->SetSize({Size.X, SBSize});

	m_VerticalSB
		->SetHandleSize(Overflow.Y > 0.0f ? Size.Y - Overflow.Y : 0.0f)
		.SetPosition({-GetPosition().X + Size.X - SBSize, -GetPosition().Y})
		->SetSize({SBSize, Size.Y - (m_HorizontalSB->HasHandle() ? SBSize : 0.0f)});
}

Rect ScrollableContainer::TranslatedBounds() const
{
	const Vector2 Position = GetAbsolutePosition() - GetPosition();
	return {Position, Position + GetSize()};
}

Vector2 ScrollableContainer::GetContentSize(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Vector2 Result;

	for (const std::shared_ptr<Control>& Item : Controls)
	{
		if (Item != m_HorizontalSB && Item != m_VerticalSB)
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
	const float SBSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();
	return {
		std::max<float>(m_ContentSize.X - GetSize().X + (m_VerticalSB->HasHandle() ? SBSize : 0.0f), 0.0f),
		std::max<float>(m_ContentSize.Y - GetSize().Y + (m_HorizontalSB->HasHandle() ? SBSize : 0.0f), 0.0f)
	};
}

void ScrollableContainer::SetOffset(const Vector2& Offset, bool UpdateScrollBar)
{
	const Vector2 Overflow = GetOverflow();
	const Vector2 Position = {
		std::max<float>(-Overflow.X, std::min<float>(-Offset.X, 0.0f)),
		std::max<float>(-Overflow.Y, std::min<float>(-Offset.Y, 0.0f))
	};
	SetPosition({Position.X, Position.Y});

	if (UpdateScrollBar)
	{
		const Vector2 ScrollOffset = {
			-Position.X / (Overflow.X > 0.0f ? Overflow.X : 1.0f),
			-Position.Y / (Overflow.Y > 0.0f ? Overflow.Y : 1.0f)
		};
		m_HorizontalSB->SetOffset(ScrollOffset.X * m_HorizontalSB->GetAvailableScrollSize());
		m_VerticalSB->SetOffset(ScrollOffset.Y * m_VerticalSB->GetAvailableScrollSize());
	}
}

}
