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
#include "../Json.h"
#include "../Paint.h"
#include "../Theme.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneUI
{

TextInput::TextPosition::TextPosition()
{
}

TextInput::TextPosition::TextPosition(uint32_t Line, uint32_t Column, uint32_t Index)
	: m_Line(Line)
	, m_Column(Column)
	, m_Index(Index)
{
}

bool TextInput::TextPosition::operator==(const TextInput::TextPosition& Other) const
{
	return m_Line == Other.m_Line && m_Column == Other.m_Column && m_Index == Other.m_Index;
}

bool TextInput::TextPosition::operator!=(const TextInput::TextPosition& Other) const
{
	return m_Line != Other.m_Line || m_Column != Other.m_Column || m_Index != Other.m_Index;
}

bool TextInput::TextPosition::operator<(const TextInput::TextPosition& Other) const
{
	return m_Line < Other.m_Line || (m_Line == Other.m_Line && m_Column < Other.m_Column);
}

void TextInput::TextPosition::Invalidate()
{
	m_Line = -1;
	m_Column = -1;
	m_Index = -1;
}

bool TextInput::TextPosition::IsValid() const
{
	return m_Line != -1 && m_Column != -1 && m_Index != -1;
}

bool TextInput::TextPosition::IsValidIndex() const
{
	return m_Index != -1;
}

void TextInput::TextPosition::SetLine(uint32_t Line)
{
	m_Line = Line;
}

uint32_t TextInput::TextPosition::Line() const
{
	return m_Line;
}

void TextInput::TextPosition::SetColumn(uint32_t Column)
{
	m_Column = Column;
}

uint32_t TextInput::TextPosition::Column() const
{
	return m_Column;
}

void TextInput::TextPosition::SetIndex(uint32_t Index)
{
	m_Index = Index;
}

uint32_t TextInput::TextPosition::Index() const
{
	return m_Index;
}

TextInput::TextInput(Window* InWindow)
	: Control(InWindow)
{
	m_Text = std::make_shared<Text>(InWindow);
	m_Text->SetParent(this);
	m_Text->SetPosition({3.0f, 0.0f});

	SetSize({100.0f, GetTheme()->GetFont()->Size()});
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
	const float LineHeight = TheTheme->GetFont()->Size();

	Rect Bounds = GetAbsoluteBounds();
	Brush.Rectangle(Bounds, TheTheme->GetColor(Theme::Colors::TextInput_Background));

	if (m_Focused)
	{
		Brush.RectangleOutline(Bounds, TheTheme->GetColor(Theme::Colors::TextInput_FocusedOutline));
	}

	Brush.PushClip(GetAbsoluteBounds());
	m_Text->OnPaint(Brush);

	if (m_Focused)
	{
		const Vector2 Size = GetPositionLocation(m_Position);
		const Vector2 Start = m_Text->GetAbsolutePosition() + Vector2(std::max<float>(Size.X, 2.0f), Size.Y);
		const Vector2 End = Start + Vector2(0.0f, LineHeight);
		Brush.Line(Start, End, TheTheme->GetColor(Theme::Colors::TextInput_Cursor));
	}

	if (m_Anchor.IsValid() && m_Anchor != m_Position)
	{
		const TextPosition Min = m_Anchor < m_Position ? m_Anchor : m_Position;
		const TextPosition Max = m_Anchor < m_Position ? m_Position : m_Anchor;

		const std::string& String = m_Text->GetString();
		if (Min.Line() == Max.Line())
		{
			const Vector2 MinPos = GetPositionLocation(Min);
			const Vector2 MaxPos = GetPositionLocation(Max);

			const Rect SelectBounds = {
				m_Text->GetAbsolutePosition() + MinPos,
				m_Text->GetAbsolutePosition() + MaxPos + Vector2(0.0f, LineHeight)
			};

			Brush.Rectangle(SelectBounds, TheTheme->GetColor(Theme::Colors::TextInput_Selection));
		}
		else
		{
			uint32_t Index = Min.Index();
			for (uint32_t Line = Min.Line(); Line <= Max.Line(); Line++)
			{
				if (Line == Min.Line())
				{
					const std::string Sub = String.substr(Min.Index(), LineEndIndex(Min.Index()) - Min.Index());
					const Vector2 Size = TheTheme->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation(Min);
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position,
						m_Text->GetAbsolutePosition() + Position + Vector2(Size.X, LineHeight)
					};
					Brush.Rectangle(SelectBounds, TheTheme->GetColor(Theme::Colors::TextInput_Selection));
				}
				else if (Line == Max.Line())
				{
					const uint32_t Start = LineStartIndex(Max.Index());
					const std::string Sub = String.substr(Start, Max.Index() - Start);
					const Vector2 Size = TheTheme->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation(Max);
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position - Vector2(Size.X, 0.0f),
						m_Text->GetAbsolutePosition() + Position + Vector2(0.0f, LineHeight)
					};
					Brush.Rectangle(SelectBounds, TheTheme->GetColor(Theme::Colors::TextInput_Selection));
				}
				else
				{
					const std::string Sub = String.substr(Index, LineEndIndex(Index) - Index);
					const Vector2 Size = TheTheme->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation({Line, 0, Index});
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position,
						m_Text->GetAbsolutePosition() + Position + Vector2(Size.X, LineHeight)
					};
					Brush.Rectangle(SelectBounds, TheTheme->GetColor(Theme::Colors::TextInput_Selection));
				}

				Index = LineEndIndex(Index) + 1;
			}
		}
	}

	Brush.PopClip();
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

void TextInput::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	m_Multiline = Root["Multiline"].Boolean();
	if (m_Multiline)
	{
		SetSize({200.0f, 200.0f});
	}

	m_Text->OnLoad(Root["Text"]);
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
		m_Position = GetPosition(Position);
		Invalidate();
	}
}

bool TextInput::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	m_Position = GetPosition(Position);
	m_Anchor = m_Position;
	m_Drag = true;
	Invalidate();

	return true;
}

void TextInput::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
	if (m_Anchor == m_Position)
	{
		m_Anchor.Invalidate();
	}

	m_Drag = false;
}

void TextInput::OnText(uint32_t Code)
{
	if (!std::isalnum(Code) && Code != '\n')
	{
		return;
	}

	std::string Contents = m_Text->GetText();
	Contents.insert(Contents.begin() + m_Position.Column(), (int8_t)Code);
	SetText(Contents.c_str());
	MovePosition(1);
}

void TextInput::Delete(int32_t Range)
{
	uint32_t Column = m_Position.Column();

	int32_t Min = std::min<int32_t>(Column, Column + (int32_t)Range);
	Min = std::max<int32_t>(0, Min);

	int32_t Max = std::max<int32_t>(Column, Column + (int32_t)Range);
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
		if (!m_Anchor.IsValid())
		{
			m_Anchor = m_Position;
		}
	}
	else
	{
		m_Anchor.Invalidate();
	}

	if (Count < 0)
	{
		int32_t Result = (int32_t)m_Position.Column() + Count;

		if (Result < 0)
		{
			Count = -m_Position.Column();
		}
	}

	uint32_t Column = m_Position.Column() + Count;
	Column = std::min<uint32_t>(Column, m_Text->Length());
	m_Position.SetColumn(Column);

	Vector2 Offset = -m_Text->GetPosition();
	Vector2 Position = GetPositionLocation(m_Position);
	Vector2 Max = Offset + Vector2(GetSize().X, 0.0f);

	if (Position.X < Offset.X)
	{
		Offset.X = Position.X - GetTheme()->GetFont()->SpaceSize().X;
	}
	else if (Position.X >= Max.X)
	{
		Offset.X = Position.X - GetSize().X + GetTheme()->GetFont()->SpaceSize().X;
	}

	const Vector2 TextPos = m_Text->GetPosition();
	m_Text->SetPosition({-Offset.X, TextPos.Y});
	Invalidate();
}

Vector2 TextInput::GetPositionLocation(const TextPosition& Position) const
{
	if (!m_Position.IsValid())
	{
		return {0.0f, 0.0f};
	}

	const std::string& String = m_Text->GetString();
	uint32_t Start = LineStartIndex(Position.Index());

	const std::string Sub = String.substr(Start, Position.Index() - Start);
	return {GetTheme()->GetFont()->Measure(Sub).X, Position.Line() * GetTheme()->GetFont()->Size()};
}

TextInput::TextPosition TextInput::GetPosition(const Vector2& Position) const
{
	const float LineHeight = GetTheme()->GetFont()->Size();
	const std::string& String = m_Text->GetString();

	// Transform into local space.
	const Vector2 LocalPosition = Position - GetAbsolutePosition();
	// TODO: Take into account any scrolling once contained within a ScrollableContainer.

	// Find the starting index based on what line the position is on.
	size_t StartIndex = 0;
	size_t Index = 0;
	uint32_t Line = 0;
	uint32_t Column = 0;
	Vector2 Offset = {0.0f, LineHeight};
	while (StartIndex != std::string::npos)
	{
		if (Offset.Y > LocalPosition.Y)
		{
			Index = StartIndex;
			break;
		}

		size_t Find = String.find('\n', StartIndex);
		if (Find != std::string::npos)
		{
			Line++;
			StartIndex = Find + 1;
			Offset.Y += LineHeight;
		}
		else
		{
			// Reached the end of the string. Mark the column to be the end
			// of the final line and make the index be the size of the string.
			Column = String.size() - StartIndex;
			Index = String.size();
			break;
		}
	}

	// Find the character on the line that is after the given position.
	for (; Index < String.size(); Index++, Column++)
	{
		const char Ch = String[Index];
		if (Ch == '\n')
		{
			break;
		}

		const Vector2 Size = GetTheme()->GetFont()->Measure(Ch);
		Offset.X += Size.X;

		if (Position.X + m_Text->GetPosition().X <= GetAbsolutePosition().X + Offset.X)
		{
			break;
		}
	}

	return {Line, Column, (uint32_t)Index};
}

bool TextInput::IsShiftPressed() const
{
	return IsKeyPressed(Keyboard::Key::LeftShift) || IsKeyPressed(Keyboard::Key::RightShift);
}

int32_t TextInput::GetRangeOr(int32_t Value) const
{
	if (!m_Anchor.IsValid())
	{
		return Value;
	}

	return m_Anchor.Column() - m_Position.Column();
}

uint32_t TextInput::LineStartIndex(uint32_t Index) const
{
	const std::string& String = m_Text->GetString();

	// The index may already be on a newline character. Start the search at the character
	// before this one.
	const uint32_t Offset = String[Index] == '\n' ? Index - 1 : Index;
	size_t Result = String.rfind('\n', Offset);
	return Result == std::string::npos ? 0 : Result;
}
}
