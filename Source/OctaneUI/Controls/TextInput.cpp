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
	case Keyboard::Key::Left: MovePosition(0, -1, IsShiftPressed()); break;
	case Keyboard::Key::Right: MovePosition(0, 1, IsShiftPressed()); break;
	case Keyboard::Key::Up: MovePosition(-1, 0, IsShiftPressed()); break;
	case Keyboard::Key::Down: MovePosition(1, 0, IsShiftPressed()); break;
	case Keyboard::Key::Home: MoveHome(); break;
	case Keyboard::Key::End: MoveEnd(); break;
	case Keyboard::Key::Enter: OnText('\n'); break;
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

	if (m_Anchor.IsValid())
	{
		Delete(GetRangeOr(0));
	}

	std::string Contents = m_Text->GetText();
	Contents.insert(Contents.begin() + m_Position.Index(), (int8_t)Code);
	SetText(Contents.c_str());
	MovePosition(0, 1);
}

void TextInput::Delete(int32_t Range)
{
	uint32_t Index = m_Position.Index();

	int32_t Min = std::min<int32_t>(Index, Index + (int32_t)Range);
	Min = std::max<int32_t>(0, Min);

	int32_t Max = std::max<int32_t>(Index, Index + (int32_t)Range);
	Max = std::min<int32_t>(m_Text->Length(), Max);

	// Only move the cursor if deleting characters to the left of the cursor.
	// Move the position before updating the text object. This should place
	// the position to the correct index in the string buffer.
	int32_t Move = std::min<int32_t>(Range, 0);
	MovePosition(0, Move);

	// TODO: Maybe allow altering the contents in-place and repaint?
	std::string Contents = m_Text->GetText();
	Contents.erase(Contents.begin() + (uint32_t)Min, Contents.begin() + (uint32_t)Max);

	SetText(Contents.c_str());
}

void TextInput::MoveHome()
{
	int Index = LineStartIndex(m_Position.Index());
	Index = Index > 0 ? Index + 1 : Index;
	MovePosition(0, Index - m_Position.Index(), IsShiftPressed());
}

void TextInput::MoveEnd()
{
	MovePosition(0, LineEndIndex(m_Position.Index()) - m_Position.Index(), IsShiftPressed());
}

void TextInput::MovePosition(int32_t Line, int32_t Column, bool UseAnchor)
{
	// This function will calculate the new line and column along with the index
	// into the string buffer of the text object.

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

	const std::string& String = m_Text->GetString();
	int32_t NewIndex = m_Position.Index();
	int32_t LineIndex = LineStartIndex(m_Position.Index());
	int32_t NewLine = m_Position.Line();
	int32_t NewColumn = m_Position.Column();

	// First, figure out the line and the string index for the new line.
	// This is done by iterating each line and calculating the new offset until the desired number
	// of lines is reached.
	const bool LineBack = Line < 0;
	LineIndex = String[LineIndex] == '\n' ? LineIndex + 1 : LineIndex;
	for (int32_t I = 0; I < std::abs(Line) && NewLine >= 0; I++)
	{
		// Need to adjust the starting search position for finding the next newline character.
		// Want to avoid cases where the same index is returned.
		const uint32_t Start = LineBack ? std::max<int32_t>(LineIndex - 1, 0) : LineIndex;
		uint32_t Index = LineBack ? LineStartIndex(Start) : LineEndIndex(Start);
		Index = String[Index] == '\n' ? Index + 1 : Index;

		if (Index == String.size())
		{
			NewIndex = String.size();
			break;
		}

		NewLine = LineBack ? NewLine - 1 : NewLine + 1;
		NewIndex = Index;
		LineIndex = Index;
	}

	NewColumn = std::min<int32_t>(NewColumn, LineEndIndex(LineIndex) - LineIndex);
	NewIndex = LineIndex + NewColumn;

	// Apply any column movement. This will alter the current line based on if the
	// cursor moves past the beginning or end of a line. This is done by looping
	// and subtracting from the amount of column spaces to move until all moves
	// have been accounted for.
	const bool ColumnBack = Column < 0;
	Column = std::abs(Column);
	int32_t ColumnIndex = NewIndex + Column;
	while (Column != 0)
	{
		// Find the line character based on if the cursor is moving forward or backward.
		int LineSize = this->LineSize(NewIndex);
		uint32_t Index = ColumnBack ? LineStartIndex(NewIndex) : LineEndIndex(NewIndex);
		
		// Prevent the diff to exceed amount of columns to traverse.
		int Diff = std::min<int>(std::abs(NewIndex - (int)Index), std::abs(Column));
		if (Diff == 0)
		{
			// Search for newline characters will not result in a Diff, so apply
			// one to move past this character. This will force the line count to
			// update.
			if (String[NewIndex] == '\n')
			{
				Diff = 1;
			}
			else
			{
				// May be trying to move at the beginning or end.
				Column = 0;
			}
		}

		Column = std::max<int>(Column - Diff, 0);
		
		// Apply the current diff amount to the index for possible further searches.
		// Clamp to [0, Stirng.size]
		NewIndex = ColumnBack
			? std::max<int>(NewIndex - Diff, 0)
			: std::min<int>(NewIndex + Diff, String.size());
		
		// Set the new column index. This will move the column index to either the beginning
		// or end of a line if the column exceeds the line size.
		NewColumn = ColumnBack ? NewColumn - Diff : NewColumn + Diff;
		if (NewColumn < 0)
		{
			NewLine--;
			NewColumn = this->LineSize(Index);
		}
		else if (NewColumn > LineSize)
		{
			NewLine++;
			NewColumn = 0;
		}
	}

	NewLine = std::max<int32_t>(NewLine, 0);
	NewColumn = std::max<int>(NewColumn, 0);

	m_Position = { (uint32_t)NewLine, (uint32_t)NewColumn, (uint32_t)NewIndex };
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

	return m_Anchor.Index() - m_Position.Index();
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

uint32_t TextInput::LineEndIndex(uint32_t Index) const
{
	const std::string& String = m_Text->GetString();

	if (String[Index] == '\n')
	{
		return Index;
	}

	size_t Result = String.find('\n', Index);
	return Result == std::string::npos ? String.size() : Result;
}

uint32_t TextInput::LineSize(uint32_t Index) const
{
	const std::string& String = m_Text->GetString();
	uint32_t Start = LineStartIndex(Index);
	// The line should start at the character after the newline character.
	if (String[Start] == '\n')
	{
		Start++;
	}
	uint32_t End = LineEndIndex(Index);
	return std::abs((int)Start - (int)End);
}

}
