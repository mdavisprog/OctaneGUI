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
#include "ScrollBar.h"

namespace OctaneUI
{

ScrollBar::ScrollBar(Window* InWindow, Orientation InOrientation)
	: Control(InWindow)
	, m_Orientation(InOrientation)
{
}

ScrollBar* ScrollBar::SetHandleSize(float HandleSize)
{
	// TODO: Not really a fan of how this is done. Look into something better.
	m_HandleSize = 0.0f;
	if (HandleSize != 0.0f)
	{
		const float Min = GetTheme()->GetConstant(Theme::FloatConstants::ScrollBar_HandleMinSize);
		m_HandleSize = std::max<float>(Min, HandleSize);
	}

	return this;
}

ScrollBar* ScrollBar::SetOnDrag(onScrollBarSignature Fn)
{
	m_OnDrag = Fn;
	return this;
}

float ScrollBar::Offset() const
{
	return m_Offset;
}

float ScrollBar::OffsetPct() const
{
	if (m_HandleSize <= 0.0f)
	{
		return 0.0f;
	}

	float Max = m_Orientation == Orientation::Horizontal ? GetSize().X : GetSize().Y;
	return m_Offset / (Max - m_HandleSize);
}

bool ScrollBar::HasHandle() const
{
	return m_HandleSize > 0.0f;
}

void ScrollBar::OnPaint(Paint& Brush) const
{
	if (m_HandleSize > 0.0f)
	{
		Brush.Rectangle(GetAbsoluteBounds(), GetTheme()->GetColor(Theme::Colors::ScrollBar));
		Brush.Rectangle(HandleBounds(), (m_Hovered || m_Drag) ? GetTheme()->GetColor(Theme::Colors::ScrollBar_HandleHovered) :GetTheme()->GetColor(Theme::Colors::ScrollBar_Handle));
	}
}

void ScrollBar::OnMouseMove(const Vector2& Position)
{
	bool Hovered = HandleBounds().Contains(Position);
	
	if (Hovered != m_Hovered)
	{
		m_Hovered = Hovered;
		Invalidate();
	}

	if (m_Drag)
	{
		float Max = 0.0f;
		if (m_Orientation == Orientation::Horizontal)
		{
			m_Offset += Position.X - m_DragAnchor.X;
			Max = GetSize().X;
		}
		else
		{
			m_Offset += Position.Y - m_DragAnchor.Y;
			Max = GetSize().Y;
		}

		m_DragAnchor = Position;

		m_Offset = std::max<float>(m_Offset, 0.0f);
		m_Offset = std::min<float>(m_Offset, Max - m_HandleSize);

		if (m_OnDrag)
		{
			m_OnDrag(this);
		}

		Invalidate();
	}
}

bool ScrollBar::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	if (m_Hovered)
	{
		m_Drag = true;
		m_DragAnchor = Position;
		return true;
	}

	return false;
}

void ScrollBar::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
	m_Drag = false;
	Invalidate();
}

void ScrollBar::OnMouseLeave()
{
	m_Hovered = false;
	Invalidate();
}

Rect ScrollBar::HandleBounds() const
{
	Rect Result;

	const Vector2 Size = GetSize();
	if (m_Orientation == Orientation::Horizontal)
	{
		const Vector2 Position = GetAbsolutePosition() + Vector2(m_Offset, 0.0f);
		Result = Rect(Position, Position + Vector2(m_HandleSize, Size.Y));
	}
	else
	{
		const Vector2 Position = GetAbsolutePosition() + Vector2(0.0f, m_Offset);
		Result = Rect(Position, Position + Vector2(Size.X , m_HandleSize));
	}

	return Result;
}

}
