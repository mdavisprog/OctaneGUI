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

#include "ScrollBar.h"
#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ImageButton.h"

namespace OctaneGUI
{

ScrollBarHandle::ScrollBarHandle(Window* InWindow, ScrollBar* InScrollBar, Orientation InOrientation)
	: Control(InWindow)
	, m_ScrollBar(InScrollBar)
	, m_Orientation(InOrientation)
{
}

Orientation ScrollBarHandle::GetOrientation() const
{
	return m_Orientation;
}

ScrollBarHandle& ScrollBarHandle::SetHandleSize(float HandleSize)
{
	// TODO: Not really a fan of how this is done. Look into something better.
	m_HandleSize = 0.0f;
	if (HandleSize != 0.0f)
	{
		const float Min = GetProperty(ThemeProperties::ScrollBar_HandleMinSize).Float();
		m_HandleSize = std::max<float>(Min, HandleSize);
		ClampOffset();
	}

	return *this;
}

float ScrollBarHandle::HandleSize() const
{
	return m_HandleSize;
}

float ScrollBarHandle::GetAvailableScrollSize() const
{
	const float Max = m_Orientation == Orientation::Horizontal ? GetSize().X : GetSize().Y;
	return std::max<float>(0.0f, Max - m_HandleSize);
}

ScrollBarHandle& ScrollBarHandle::SetOnDrag(OnScrollBarSignature Fn)
{
	m_OnDrag = Fn;
	return *this;
}

ScrollBarHandle& ScrollBarHandle::SetOffset(float Offset)
{
	if (m_Offset != Offset)
	{
		m_Offset = Offset;
		ClampOffset();
		Invalidate();
	}

	return *this;
}

float ScrollBarHandle::Offset() const
{
	return m_Offset;
}

float ScrollBarHandle::OffsetPct() const
{
	if (m_HandleSize <= 0.0f)
	{
		return 0.0f;
	}

	float Max = m_Orientation == Orientation::Horizontal ? GetSize().X : GetSize().Y;
	return m_Offset / (Max - m_HandleSize);
}

bool ScrollBarHandle::HasHandle() const
{
	return m_HandleSize > 0.0f;
}

void ScrollBarHandle::OnPaint(Paint& Brush) const
{
	if (m_HandleSize > 0.0f)
	{
		if (m_HandleSize > 0.0f)
		{
			if (GetProperty(ThemeProperties::ScrollBar_3D).Bool())
			{
				Brush.Rectangle3D(
					HandleBounds(),
					GetProperty(ThemeProperties::ScrollBar_Handle).ToColor(),
					GetProperty(ThemeProperties::Button_Highlight_3D).ToColor(),
					GetProperty(ThemeProperties::Button_Shadow_3D).ToColor());
			}
			else
			{
				Brush.Rectangle(
					HandleBounds(),
					(m_Hovered || m_Drag) ? GetProperty(ThemeProperties::ScrollBar_HandleHovered).ToColor() : GetProperty(ThemeProperties::ScrollBar_Handle).ToColor());
			}
		}
	}
}

void ScrollBarHandle::OnSave(Json& Root) const
{
	Control::OnSave(Root);

	Root["Orientation"] = ToString(m_Orientation);
	Root["HandleSize"] = m_HandleSize;
	Root["Offset"] = m_Offset;
}

void ScrollBarHandle::OnMouseMove(const Vector2& Position)
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
			m_OnDrag(*m_ScrollBar);
		}
	}
}

bool ScrollBarHandle::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
	if (m_Hovered && Button == Mouse::Button::Left)
	{
		m_Drag = true;
		m_DragAnchor = Position;
		return true;
	}

	return false;
}

void ScrollBarHandle::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
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

void ScrollBarHandle::OnMouseLeave()
{
	m_Hovered = false;

	if (!m_Drag)
	{
		Invalidate();
	}
}

Rect ScrollBarHandle::HandleBounds() const
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
		Result = Rect(Position, Position + Vector2(Size.X, m_HandleSize));
	}

	return Result;
}

void ScrollBarHandle::ClampOffset()
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

ScrollBar::ScrollBar(Window* InWindow, Orientation InOrientation)
	: Container(InWindow)
{
	m_Handle = AddControl<ScrollBarHandle>(this, InOrientation);
}

const std::shared_ptr<ScrollBarHandle>& ScrollBar::Handle() const
{
	return m_Handle;
}

ScrollBar& ScrollBar::SetScrollBarSize(const Vector2& Size)
{
	SetSize(Size);

	Vector2 HandleSize = Size;
	if (m_Buttons)
	{
		const float ScrollBarSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();

		if (m_Handle->GetOrientation() == Orientation::Horizontal)
		{
			HandleSize.X -= ScrollBarSize * 2.0f;

			if (m_MaxButton)
			{
				m_MaxButton->SetPosition({ HandleSize.X + ScrollBarSize, 0.0f });
			}
		}
		else
		{
			HandleSize.Y -= ScrollBarSize * 2.0f;

			if (m_MaxButton)
			{
				m_MaxButton->SetPosition({ 0.0f, HandleSize.Y + ScrollBarSize });
			}
		}
	}

	m_Handle->SetSize(HandleSize);

	return *this;
}

Vector2 ScrollBar::GetScrollBarSize() const
{
	return m_Handle->GetSize();
}

ScrollBar& ScrollBar::SetAlwaysPaint(bool AlwaysPaint)
{
	m_AlwaysPaint = AlwaysPaint;
	return *this;
}

bool ScrollBar::ShouldPaint() const
{
	return (m_AlwaysPaint || m_Handle->HasHandle()) && m_Enabled;
}

ScrollBar& ScrollBar::SetEnabled(bool Enabled)
{
	m_Enabled = Enabled;
	return *this;
}

ScrollBar& ScrollBar::SetOnScrollMin(OnScrollBarSignature&& Fn)
{
	m_OnScrollMin = std::move(Fn);
	return *this;
}

ScrollBar& ScrollBar::SetOnScrollMax(OnScrollBarSignature&& Fn)
{
	m_OnScrollMax = std::move(Fn);
	return *this;
}

ScrollBar& ScrollBar::SetOnRelease(OnScrollBarSignature&& Fn)
{
	m_OnRelease = std::move(Fn);
	return *this;
}

void ScrollBar::Update()
{
	Container::Update();

	UpdateButtons();
}

void ScrollBar::OnPaint(Paint& Brush) const
{
	Brush.Rectangle(HandleBackgroundBounds(), GetProperty(ThemeProperties::ScrollBar).ToColor());

	Container::OnPaint(Brush);
}

void ScrollBar::OnSave(Json& Root) const
{
	Container::OnSave(Root);

	Root["AlwaysPaint"] = m_AlwaysPaint;
	Root["Buttons"] = m_Buttons;
	Root["Enabled"] = m_Enabled;
}

void HandleMouseMove(const std::shared_ptr<Control>& Item, const Vector2& Position)
{
	if (!Item)
	{
		return;
	}

	Item->OnMouseMove(Position);

	if (Item->Contains(Position))
	{
		Item->OnMouseEnter();
	}
	else
	{
		Item->OnMouseLeave();
	}
}

void ScrollBar::OnMouseMove(const Vector2& Position)
{
	if (!m_Enabled)
	{
		return;
	}

	HandleMouseMove(m_MinButton, Position);
	HandleMouseMove(m_MaxButton, Position);
	HandleMouseMove(m_Handle, Position);
}

bool ScrollBar::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
	if (!m_Enabled)
	{
		return false;
	}

	if (m_MinButton && m_MinButton->OnMousePressed(Position, Button, Count))
	{
		return true;
	}

	if (m_MaxButton && m_MaxButton->OnMousePressed(Position, Button, Count))
	{
		return true;
	}

	if (m_Handle && m_Handle->OnMousePressed(Position, Button, Count))
	{
		return true;
	}

	return false;
}

void ScrollBar::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
	if (!m_Enabled)
	{
		return;
	}

	if (m_MinButton)
	{
		m_MinButton->OnMouseReleased(Position, Button);
	}

	if (m_MaxButton)
	{
		m_MaxButton->OnMouseReleased(Position, Button);
	}

	if (m_Handle)
	{
		m_Handle->OnMouseReleased(Position, Button);
	}
}

void ScrollBar::OnThemeLoaded()
{
	Container::OnThemeLoaded();

	m_AlwaysPaint = GetProperty(ThemeProperties::ScrollBar_AlwaysPaint).Bool();
	m_Buttons = GetProperty(ThemeProperties::ScrollBar_Buttons).Bool();

	if (m_Buttons)
	{
		const float ScrollBarSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();

		if (!m_MinButton)
		{
			m_MinButton = AddControl<ImageButton>();
			m_MinButton
				->SetOnPressed([this](const Button&) -> void
					{
						if (m_OnScrollMin)
						{
							m_OnScrollMin(*this);
						}
					})
				.SetOnReleased([this](const Button&) -> void
					{
						if (m_OnRelease)
						{
							m_OnRelease(*this);
						}
					});
		}

		if (!m_MaxButton)
		{
			m_MaxButton = AddControl<ImageButton>();
			m_MaxButton
				->SetOnPressed([this](const Button&) -> void
					{
						if (m_OnScrollMax)
						{
							m_OnScrollMax(*this);
						}
					})
				.SetOnReleased([this](const Button&) -> void
					{
						if (m_OnRelease)
						{
							m_OnRelease(*this);
						}
					});
		}

		m_MinButton->SetTexture(GetWindow()->GetIcons()->GetTexture());
		m_MaxButton->SetTexture(GetWindow()->GetIcons()->GetTexture());

		if (m_Handle->GetOrientation() == Orientation::Horizontal)
		{
			m_Handle->SetPosition({ ScrollBarSize, 0.0f });
			m_MaxButton->SetPosition({ m_Handle->GetSize().X, 0.0f });

			m_MinButton->SetUVs(GetWindow()->GetIcons()->GetUVs(Icons::Type::ArrowLeft));
			m_MaxButton->SetUVs(GetWindow()->GetIcons()->GetUVs(Icons::Type::ArrowRight));
		}
		else
		{
			m_Handle->SetPosition({ 0.0f, ScrollBarSize });
			m_MaxButton->SetPosition({ 0.0f, m_Handle->GetSize().Y - ScrollBarSize });

			m_MinButton->SetUVs(GetWindow()->GetIcons()->GetUVs(Icons::Type::ArrowUp));
			m_MaxButton->SetUVs(GetWindow()->GetIcons()->GetUVs(Icons::Type::ArrowDown));
		}

		m_MinButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
		m_MaxButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());

		m_MinButton->SetSize({ ScrollBarSize, ScrollBarSize });
		m_MaxButton->SetSize({ ScrollBarSize, ScrollBarSize });

		UpdateButtons();
	}
	else
	{
		m_Handle->SetPosition({ 0.0f, 0.0f });

		RemoveControl(m_MinButton);
		RemoveControl(m_MaxButton);
		m_MinButton = nullptr;
		m_MaxButton = nullptr;
	}
}

Rect ScrollBar::HandleBackgroundBounds() const
{
	Rect Result = GetAbsoluteBounds();

	if (m_Buttons)
	{
		const float ScrollBarSize = GetProperty(ThemeProperties::ScrollBar_Size).Float();

		if (m_Handle->GetOrientation() == Orientation::Horizontal)
		{
			Result.Move({ ScrollBarSize, 0.0f });
			Result.SetSize({ Result.GetSize().X - ScrollBarSize * 2.0f, Result.GetSize().Y });
		}
		else
		{
			Result.Move({ 0.0f, ScrollBarSize });
			Result.SetSize({ Result.GetSize().X, Result.GetSize().Y - ScrollBarSize * 2.0f });
		}
	}

	return Result;
}

void ScrollBar::UpdateButtons()
{
	if (m_MinButton)
	{
		m_MinButton->SetDisabled(!m_Handle->HasHandle());
	}

	if (m_MaxButton)
	{
		m_MaxButton->SetDisabled(!m_Handle->HasHandle());
	}
}

}
