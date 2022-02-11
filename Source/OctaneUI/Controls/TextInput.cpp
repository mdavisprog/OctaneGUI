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

#include "../Font.h"
#include "../Paint.h"
#include "../Theme.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneUI
{

TextInput::TextInput(Window* InWindow)
	: Control(InWindow)
	, m_Position(0)
	, m_Anchor(-1)
	, m_Focused(false)
	, m_Drag(false)
	, m_Offset()
{
	m_Text = std::make_shared<Text>(InWindow);
	m_Text->SetParent(this);

	SetSize(100.0f, GetTheme()->GetFont()->GetSize());
}

TextInput::~TextInput()
{
}

TextInput* TextInput::SetText(const char* InText)
{
	m_Text->SetText(InText);
	Invalidate();
	return this;
}

const char* TextInput::GetText() const
{
	return m_Text->GetText();
}

void TextInput::OnPaint(Paint& Brush) const
{
	std::shared_ptr<Theme> TheTheme = GetTheme();

	Rect Bounds = GetAbsoluteBounds();
	Brush.Rectangle(Bounds, TheTheme->GetColor(Theme::Colors::TextInput_Background));

	if (m_Focused)
	{
		Brush.RectangleOutline(Bounds, TheTheme->GetColor(Theme::Colors::TextInput_FocusedOutline));
	}

	Brush.PushClip(GetAbsoluteBounds(), m_Offset);
	m_Text->OnPaint(Brush);

	if (m_Focused)
	{
		const Vector2 Size = GetPositionLocation();

		const Vector2 Start = m_Text->GetAbsolutePosition() + Vector2(Size.X, -5.0f);
		const Vector2 End = Start + Vector2(0.0f, GetTheme()->GetFont()->GetSize() - 5.0f);
		Brush.Line(Start, End, TheTheme->GetColor(Theme::Colors::TextInput_Cursor));
	}

	if (m_Anchor != -1 && m_Anchor != m_Position)
	{
		uint32_t Min = std::min<uint32_t>(m_Anchor, m_Position);
		uint32_t Max = std::max<uint32_t>(m_Anchor, m_Position);

		const std::string Contents = m_Text->GetText();
		const std::string Selected = Contents.substr(Min, Max - Min);
		const std::string MinStr = Contents.substr(0, Min);

		const Vector2 MinSize = GetTheme()->GetFont()->Measure(MinStr);
		const Vector2 Size = GetTheme()->GetFont()->Measure(Selected);

		Rect SelectBounds;
		SelectBounds.Min = Vector2(MinSize.X, 0.0f);
		SelectBounds.Max = SelectBounds.Min + Vector2(Size.X + GetTheme()->GetFont()->GetSpaceSize().X, GetTheme()->GetFont()->GetSize());
		SelectBounds.Move(m_Text->GetAbsolutePosition());

		Brush.Rectangle(SelectBounds, TheTheme->GetColor(Theme::Colors::TextInput_Selection));
	}

	Brush.PopClip();
}

void TextInput::Update()
{
	const float Height = GetTheme()->GetFont()->GetSize();
	const Vector2 Size = GetSize();
	const Vector2 Position(3.0f, Size.Y * 0.5f - Height * 0.25f);
	m_Text->SetPosition(Position);
}

void TextInput::OnFocused()
{
	m_Focused = true;
	Invalidate();
}

void TextInput::OnUnfocused()
{
	m_Focused = false;
	Invalidate();
}

void TextInput::OnKeyPressed(Keyboard::Key Key)
{
	switch (Key)
	{
	case Keyboard::Key::Backspace: Delete(GetRangeOr(-1)); break;
	case Keyboard::Key::Delete: Delete(GetRangeOr(1)); break;
	case Keyboard::Key::Left: MovePosition(-1, IsShiftPressed()); break;
	case Keyboard::Key::Right: MovePosition(1, IsShiftPressed()); break;
	case Keyboard::Key::Home: MovePosition(-m_Text->Length(), IsShiftPressed()); break;
	case Keyboard::Key::End: MovePosition(m_Text->Length(), IsShiftPressed()); break;
	default: break;
	}
}

void TextInput::OnMouseMove(const Vector2& Position)
{
	if (m_Drag)
	{
		uint32_t Pos = GetPosition(Position);
		MovePosition(Pos - m_Position, true);
	}
}

bool TextInput::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	uint32_t Pos = GetPosition(Position);
	MovePosition(Pos - m_Position);
	m_Anchor = m_Position;
	m_Drag = true;

	return true;
}

void TextInput::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
	m_Drag = false;
}

void TextInput::OnText(uint32_t Code)
{
	if (Code == '\b')
	{
		return;
	}

	// Check for delete character.
	if (Code == 127)
	{
		return;
	}

	std::string Contents = m_Text->GetText();
	Contents.insert(Contents.begin() + m_Position, (int8_t)Code);
	SetText(Contents.c_str());
	MovePosition(1);
}

void TextInput::Delete(int32_t Range)
{
	int32_t Min = std::min<int32_t>(m_Position, m_Position + (int32_t)Range);
	Min = std::max<int32_t>(0, Min);

	int32_t Max = std::max<int32_t>(m_Position, m_Position + (int32_t)Range);
	Max = std::min<int32_t>(m_Text->Length(), Max);

	std::string Contents = m_Text->GetText();
	Contents.erase(Contents.begin() + (uint32_t)Min, Contents.begin() + (uint32_t)Max);

	SetText(Contents.c_str());

	// Only move the cursor if deleting characters to the left of the cursor.
	int32_t Move = std::min<int32_t>(Range, 0);
	MovePosition(Move);
}

void TextInput::MovePosition(int32_t Count, bool UseAnchor)
{
	if (UseAnchor)
	{
		if (m_Anchor == -1)
		{
			m_Anchor = m_Position;
		}
	}
	else
	{
		m_Anchor = -1;
	}

	if (Count < 0)
	{
		int32_t Result = (int32_t)m_Position + Count;

		if (Result < 0)
		{
			Count = -m_Position;
		}
	}

	m_Position += Count;
	m_Position = std::min<uint32_t>(m_Position, m_Text->Length());

	Vector2 Position = GetPositionLocation();
	Vector2 Max = m_Offset + Vector2(GetSize().X, 0.0f);

	if (Position.X < m_Offset.X)
	{
		m_Offset.X = Position.X;
	}
	else if (Position.X >= Max.X)
	{
		m_Offset.X = Position.X - GetSize().X + GetTheme()->GetFont()->GetSpaceSize().X;
	}

	const Vector2 TextPos = m_Text->GetPosition();
	m_Text->SetPosition(-m_Offset.X, TextPos.Y);
	Invalidate();
}

Vector2 TextInput::GetPositionLocation() const
{
	const std::string Sub = std::string(m_Text->GetText()).substr(0, m_Position);
	return GetTheme()->GetFont()->Measure(Sub);
}

uint32_t TextInput::GetPosition(const Vector2& Position) const
{
	Vector2 Offset;
	const std::string Contents = m_Text->GetText();
	uint32_t Pos = 0;
	for (char Ch : Contents)
	{
		const Vector2 Size = GetTheme()->GetFont()->Measure(Ch);
		Offset.X += Size.X;

		if (Position.X + m_Offset.X <= Offset.X)
		{
			break;
		}

		Pos++;
	}

	return Pos;
}

bool TextInput::IsShiftPressed() const
{
	return IsKeyPressed(Keyboard::Key::LeftShift) || IsKeyPressed(Keyboard::Key::RightShift);
}

int32_t TextInput::GetRangeOr(int32_t Value) const
{
	if (m_Anchor == -1)
	{
		return Value;
	}

	return m_Anchor - m_Position;
}

}
