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
#include "../Theme.h"
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
		const float SBSize = GetTheme()->GetConstant(Theme::FloatConstants::ScrollBar_Size);
		const float Size = m_ContentSize.X - GetSize().X + (m_VerticalSB->HasHandle() ? SBSize : 0.0f);
		SetPosition({-m_HorizontalSB->OffsetPct() * Size, GetPosition().Y});
		InvalidateLayout();
	});
	InsertControl(m_HorizontalSB);

	m_VerticalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Vertical);
	m_VerticalSB->SetOnDrag([this](ScrollBar*) -> void
	{
		const float SBSize = GetTheme()->GetConstant(Theme::FloatConstants::ScrollBar_Size);
		const float Size = m_ContentSize.Y - GetSize().Y + (m_HorizontalSB->HasHandle() ? SBSize : 0.0f);
		SetPosition({GetPosition().X, -m_VerticalSB->OffsetPct() * Size});
		InvalidateLayout();
	});
	InsertControl(m_VerticalSB);
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

	if (TranslatedBounds().Contains(Point))
	{
		return Container::GetControl(Point);
	}

	return std::weak_ptr<Control>();
}

void ScrollableContainer::Update()
{
	m_ContentSize = GetContentSize(Controls());
}

void ScrollableContainer::OnPaint(Paint& Brush) const
{
	std::vector<std::shared_ptr<Control>> Controls;
	GetControls(Controls);

	// Need to translate from scrolled space.
	Brush.PushClip(TranslatedBounds());

	for (const std::shared_ptr<Control>& Item : Controls)
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

void ScrollableContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Container::PlaceControls(Controls);

	const Vector2 ContentSize = GetContentSize(Controls);
	const Vector2 Size = GetSize();
	const Vector2 Overflow = ContentSize - Size;
	const float SBSize = GetTheme()->GetConstant(Theme::FloatConstants::ScrollBar_Size);

	m_HorizontalSB
		->SetHandleSize(Overflow.X > 0.0f ? Size.X - Overflow.X : 0.0f)
		->SetPosition({-GetPosition().X, -GetPosition().Y + Size.Y - SBSize})
		->SetSize({Size.X, SBSize});

	m_VerticalSB
		->SetHandleSize(Overflow.Y > 0.0f ? Size.Y - Overflow.Y : 0.0f)
		->SetPosition({-GetPosition().X + Size.X - SBSize, -GetPosition().Y})
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

}
