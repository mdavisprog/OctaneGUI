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
#include "ScrollBar.h"

namespace OctaneUI
{

ScrollBar::ScrollBar(Window* InWindow, Orientation InOrientation)
	: Control(InWindow)
	, m_Orientation(InOrientation)
{
}

ScrollBar& ScrollBar::SetHandleSize(float HandleSize)
{
	// TODO: Not really a fan of how this is done. Look into something better.
	m_HandleSize = 0.0f;
	if (HandleSize != 0.0f && m_Enabled)
	{
		const float Min = GetProperty(ThemeProperties::ScrollBar_HandleMinSize).Float();
		m_HandleSize = std::max<float>(Min, HandleSize);
		ClampOffset();
	}

	return *this;
}

float ScrollBar::HandleSize() const
{
	return m_HandleSize;
}

float ScrollBar::GetAvailableScrollSize() const
{
	const float Max = m_Orientation == Orientation::Horizontal ? GetSize().X : GetSize().Y;
	return std::max<float>(0.0f, Max - m_HandleSize);
}

ScrollBar& ScrollBar::SetOnDrag(OnScrollBarSignature Fn)
{
	m_OnDrag = Fn;
	return *this;
}

ScrollBar& ScrollBar::SetOffset(float Offset)
{
	if (m_Offset != Offset)
	{
		m_Offset = Offset;
		ClampOffset();
		Invalidate();
	}

	return *this;
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

ScrollBar& ScrollBar::SetEnabled(bool Enabled)
{
	if (m_Enabled != Enabled)
	{
		m_Enabled = Enabled;
		if (!m_Enabled)
		{
			m_HandleSize = 0.0f;
		}
		Invalidate();
	}
	return *this;
}

bool ScrollBar::IsEnabled() const
{
	return m_Enabled;
}

void ScrollBar::OnPaint(Paint& Brush) const
{
	if (m_HandleSize > 0.0f)
	{
		Brush.Rectangle(GetAbsoluteBounds(), GetProperty(ThemeProperties::ScrollBar).ToColor());
		Brush.Rectangle(
			HandleBounds(),
			(m_Hovered || m_Drag) ? GetProperty(ThemeProperties::ScrollBar_HandleHovered).ToColor() : GetProperty(ThemeProperties::ScrollBar_Handle).ToColor());
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
		float NewOffset = m_Offset;
		if (m_Orientation == Orientation::Horizontal)
		{
			NewOffset += Position.X - m_DragAnchor.X;
		}
		else
		{
			NewOffset += Position.Y - m_DragAnchor.Y;
		}

		m_DragAnchor = Position;

		SetOffset(NewOffset);

		if (m_OnDrag)
		{
			m_OnDrag(this);
		}
	}
}

bool ScrollBar::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	if (m_Hovered && Button == Mouse::Button::Left)
	{
		m_Drag = true;
		m_DragAnchor = Position;
		return true;
	}

	return false;
}

void ScrollBar::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
	if (Button != Mouse::Button::Left)
	{
		return;
	}

	if (!Contains(Position))
	{
		Invalidate();
	}

	m_Drag = false;
}

void ScrollBar::OnMouseLeave()
{
	m_Hovered = false;

	if (!m_Drag)
	{
		Invalidate();
	}
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

void ScrollBar::ClampOffset()
{
	const float Max = m_Orientation == Orientation::Horizontal ? GetSize().X : GetSize().Y;
	if (Max <= 0.0f)
	{
		m_Offset = 0.0f;
		return;
	}

	m_Offset = std::max<float>(m_Offset, 0.0f);
	m_Offset = std::min<float>(m_Offset, Max - m_HandleSize);
}

}
