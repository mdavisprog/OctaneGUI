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
	, m_Focused(false)
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

const char* TextInput::GetType() const
{
	return "TextInput";
}

void TextInput::OnPaint(Paint& Brush) const
{
	Rect Bounds = GetAbsoluteBounds();
	Brush.Rectangle(Bounds, Color(64, 64, 64, 255));

	if (m_Focused)
	{
		Brush.RectangleOutline(Bounds, Color(0, 0, 255, 255));
	}

	Brush.PushClip(GetAbsoluteBounds());
	m_Text->OnPaint(Brush);

	if (m_Focused)
	{
		const std::string Sub = std::string(m_Text->GetText()).substr(0, m_Position);
		const Vector2 Size = GetTheme()->GetFont()->Measure(Sub);

		const Vector2 Start = m_Text->GetAbsolutePosition() + Vector2(Size.X, 0.0f);
		const Vector2 End = Start + Vector2(0.0f, GetTheme()->GetFont()->GetSize());
		Brush.Line(Start, End, Color(255, 255, 255, 255));
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
	case Keyboard::Key::Backspace: Backspace(); break;
	case Keyboard::Key::Left: MovePosition(-1); break;
	case Keyboard::Key::Right: MovePosition(1); break;
	default: break;
	}
}

bool TextInput::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	return true;
}

void TextInput::OnText(uint32_t Code)
{
	if (Code == '\b')
	{
		return;
	}

	std::string Contents = m_Text->GetText();
	Contents.insert(Contents.begin() + m_Position, (int8_t)Code);
	m_Position++;
	SetText(Contents.c_str());
}

void TextInput::Backspace()
{
	if (m_Position == 0)
	{
		return;
	}

	std::string Contents = m_Text->GetText();
	Contents.erase(Contents.begin() + (m_Position - 1));
	m_Position--;
	SetText(Contents.c_str());
}

void TextInput::MovePosition(int32_t Count)
{
	if (Count < 0)
	{
		if (m_Position == 0)
		{
			Count = 0;
		}
	}

	m_Position += Count;
	m_Position = std::min<uint32_t>(m_Position, std::string(m_Text->GetText()).size());
	Invalidate();
}

}
