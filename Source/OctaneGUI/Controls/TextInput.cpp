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

#include "TextInput.h"
#include "../Application.h"
#include "../Defines.h"
#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "../TextSpan.h"
#include "../Theme.h"
#include "../Timer.h"
#include "../Window.h"
#include "MarginContainer.h"
#include "ScrollableContainer.h"
#include "Syntax/Highlighter.h"
#include "Text.h"

#define MARGIN 2.0f

namespace OctaneGUI
{

class TextInputInteraction : public ScrollableViewInteraction
{
	CLASS(TextInputInteraction)

public:
	TextInputInteraction(Window* InWindow, TextInput* Input)
		: ScrollableViewInteraction(InWindow)
		, m_Input(Input)
	{
		SetExpand(Expand::Both);
		SetForwardKeyEvents(true);
		SetAlwaysFocus(true);
	}

	virtual void OnFocused() override
	{
		m_Input->Focus();
	}

	virtual void OnUnfocused() override
	{
		m_Input->Unfocus();
	}

	virtual bool OnKeyPressed(Keyboard::Key Key) override
	{
		if (Control::OnKeyPressed(Key))
		{
			return true;
		}

		switch (Key)
		{
		case Keyboard::Key::V:
		{
			if (m_Input->IsCtrlPressed())
			{
				m_Input->AddText(ClipboardContents());
			}
			return true;
		}
		case Keyboard::Key::Backspace: m_Input->Delete(m_Input->GetRangeOr(-1)); return true;
		case Keyboard::Key::Delete: m_Input->Delete(m_Input->GetRangeOr(1)); return true;
		case Keyboard::Key::Left: m_Input->MovePosition(0, -1, m_Input->IsShiftPressed(), m_Input->ShouldSkipWords()); return true;
		case Keyboard::Key::Right: m_Input->MovePosition(0, 1, m_Input->IsShiftPressed(), m_Input->ShouldSkipWords()); return true;
		case Keyboard::Key::Up: m_Input->MovePosition(-1, 0, m_Input->IsShiftPressed()); return true;
		case Keyboard::Key::Down: m_Input->MovePosition(1, 0, m_Input->IsShiftPressed()); return true;
		case Keyboard::Key::Home: m_Input->MoveHome(); return true;
		case Keyboard::Key::End: m_Input->MoveEnd(); return true;
		case Keyboard::Key::Enter: m_Input->EnterPressed(); return true;
		case Keyboard::Key::Tab: m_Input->AddText('\t'); return true;
		default: break;
		}

		return false;
	}

	virtual void OnMouseMove(const Vector2& Position) override
	{
		ScrollableViewInteraction::OnMouseMove(Position);
		m_Input->MouseMove(Position);
	}

	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override
	{
		bool Handled = ScrollableViewInteraction::OnMousePressed(Position, Button, Count);
		if (Handled)
		{
			return Handled;
		}

		return m_Input->MousePressed(Position, Button, Count);
	}

	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override
	{
		ScrollableViewInteraction::OnMouseReleased(Position, Button);
		m_Input->MouseReleased(Position, Button);
	}

	virtual void OnText(uint32_t Code) override
	{
		m_Input->AddText(Code);
	}

private:
	std::u32string ClipboardContents() const
	{
		return GetWindow()->App().ClipboardContents();
	}

	TextInput* m_Input { nullptr };
};

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
	: ScrollableViewControl(InWindow)
{
	std::shared_ptr<MarginContainer> Margins = Scrollable()->AddControl<MarginContainer>();
	Margins
		->SetMargins({ MARGIN, 0.0, MARGIN, 0.0f })
		.SetExpand(Expand::None);
	m_Text = Margins->AddControl<Text>();

	SetInteraction(std::make_shared<TextInputInteraction>(InWindow, this));

	SetSize({ 100.0f, m_Text->LineHeight() });

	m_BlinkTimer = InWindow->CreateTimer(500, true, [this]() -> void
		{
			m_DrawCursor = !m_DrawCursor;
			Invalidate();
		});

	Scrollable()->SetOnScroll([this](const Vector2& Delta) -> void
		{
			if (Delta.Y != 0.0f)
			{
				UpdateVisibleLines();
				UpdateSpans();
			}
		});
}

TextInput::~TextInput()
{
}

TextInput& TextInput::SetText(const char* InText)
{
	SetText(String::ToUTF32(InText).c_str());
	return *this;
}

TextInput& TextInput::SetText(const char32_t* InText)
{
	InternalSetText(InText);
	m_Anchor.Invalidate();
	m_Position = { 0, 0, 0 };
	// TODO: Should the scroll offset be reset to zero?
	m_FirstVisibleLine.Invalidate();
	UpdateVisibleLines();
	UpdateSpans();
	Invalidate();
	return *this;
}

const char32_t* TextInput::GetText() const
{
	return m_Text->GetText();
}

const std::u32string& TextInput::GetString() const
{
	return m_Text->GetString();
}

const std::u32string_view TextInput::Line() const
{
	if (!m_Position.IsValid())
	{
		return U"";
	}

	uint32_t Start = LineStartIndex(m_Position.Index());
	uint32_t End = LineEndIndex(m_Position.Index());

	return std::u32string_view(&m_Text->GetString()[Start], End - Start);
}

const std::u32string_view TextInput::VisibleText() const
{
	if (!m_FirstVisibleLine.IsValid() || !m_LastVisibleLine.IsValid())
	{
		return U"";
	}

	uint32_t Start = m_FirstVisibleLine.Index();
	uint32_t End = m_LastVisibleLine.Index();

	return std::u32string_view(&m_Text->GetString()[Start], End - Start);
}

char32_t TextInput::Left() const
{
	const std::u32string& Contents = m_Text->GetString();

	const size_t Index = m_Position.Index() > 0 ? m_Position.Index() - 1 : 0;
	if (Index >= Contents.length())
	{
		return U'\0';
	}

	return Contents[Index];
}

char32_t TextInput::Right() const
{
	const std::u32string& Contents = m_Text->GetString();

	const size_t Index = m_Position.Index();
	if (Index >= Contents.length())
	{
		return U'\0';
	}

	return Contents[Index];
}

size_t TextInput::LineNumber() const
{
	return (size_t)m_Position.Line();
}

size_t TextInput::Column() const
{
	return (size_t)m_Position.Column();
}

size_t TextInput::Index() const
{
	return (size_t)m_Position.Index();
}

size_t TextInput::FirstVisibleIndex() const
{
	return (size_t)m_FirstVisibleLine.Index();
}

size_t TextInput::LastVisibleIndex() const
{
	return (size_t)m_LastVisibleLine.Index();
}

TextInput& TextInput::SetReadOnly(bool Value)
{
	if (m_ReadOnly == Value)
	{
		return *this;
	}

	m_ReadOnly = Value;
	Invalidate();
	return *this;
}

bool TextInput::ReadOnly() const
{
	return m_ReadOnly;
}

TextInput& TextInput::SetMulitline(bool Multiline)
{
	m_Multiline = Multiline;
	Scrollable()->SetHorizontalSBEnabled(m_Multiline).SetVerticalSBEnabled(m_Multiline);
	return *this;
}

bool TextInput::Multiline() const
{
	return m_Multiline;
}

TextInput& TextInput::SetHighlighter(const std::shared_ptr<Syntax::Highlighter>& Value)
{
	m_Highlighter = Value;

	if (m_Highlighter)
	{
		m_Highlighter->SetInput(TShare<TextInput>());
	}

	return *this;
}

Color TextInput::TextColor() const
{
	return GetProperty(ThemeProperties::Text).ToColor();
}

TextInput& TextInput::SetNumbersOnly(bool NumbersOnly)
{
	m_NumbersOnly = NumbersOnly;
	return *this;
}

bool TextInput::NumbersOnly() const
{
	return m_NumbersOnly;
}

void TextInput::Focus()
{
	if (!m_Position.IsValid())
	{
		m_Position = { 0, 0, 0 };
	}

	m_Focused = true;
	ResetCursorTimer();
	Invalidate();
}

void TextInput::Unfocus()
{
	m_Focused = false;
	m_BlinkTimer->Stop();
	Invalidate();
}

TextInput& TextInput::SetOnTextChanged(OnTextInputSignature&& Fn)
{
	m_OnTextChanged = std::move(Fn);
	return *this;
}

TextInput& TextInput::SetOnConfirm(OnTextInputSignature&& Fn)
{
	m_OnConfirm = std::move(Fn);
	return *this;
}

TextInput& TextInput::SetOnModifyText(OnModifyTextSignature&& Fn)
{
	m_OnModifyText = std::move(Fn);
	return *this;
}

TextInput& TextInput::SetOnPrePaintText(OnPaintSignature&& Fn)
{
	m_OnPrePaintText = std::move(Fn);
	return *this;
}

TextInput& TextInput::SetFontSize(float FontSize)
{
	if (FontSize == m_Text->LineHeight())
	{
		return *this;
	}

	m_Text->SetFontSize(FontSize);
	InvalidateLayout();
	return *this;
}

float TextInput::LineHeight() const
{
	return m_Text->LineHeight();
}

void TextInput::OnPaint(Paint& Brush) const
{
	std::shared_ptr<Theme> TheTheme = GetTheme();
	const float LineHeight = m_Text->LineHeight();

	Rect Bounds = GetAbsoluteBounds();

	if (GetProperty(ThemeProperties::TextInput_3D).Bool())
	{
		Brush.Rectangle3D(
			Bounds,
			GetProperty(ThemeProperties::TextInput_Background).ToColor(),
			GetProperty(ThemeProperties::Button_Highlight_3D).ToColor(),
			GetProperty(ThemeProperties::Button_Shadow_3D).ToColor(),
			true);
	}
	else
	{
		Brush.Rectangle(Bounds, GetProperty(ThemeProperties::TextInput_Background).ToColor());
	}

	if (m_Focused)
	{
		Brush.RectangleOutline(Bounds, GetProperty(ThemeProperties::TextInput_FocusedOutline).ToColor());
	}

	Brush.PushClip(GetAbsoluteBounds());

	if (m_OnPrePaintText)
	{
		m_OnPrePaintText(TShare<TextInput>(), Brush);
	}

	if (m_Anchor.IsValid() && m_Anchor != m_Position)
	{
		const TextPosition Min = m_Anchor < m_Position ? m_Anchor : m_Position;
		const TextPosition Max = m_Anchor < m_Position ? m_Position : m_Anchor;

		const std::u32string& String = m_Text->GetString();
		if (Min.Line() == Max.Line())
		{
			const Vector2 MinPos = GetPositionLocation(Min);
			const Vector2 MaxPos = GetPositionLocation(Max);

			const Rect SelectBounds = {
				m_Text->GetAbsolutePosition() + MinPos,
				m_Text->GetAbsolutePosition() + MaxPos + Vector2(0.0f, LineHeight)
			};

			Brush.Rectangle(SelectBounds, GetProperty(ThemeProperties::TextInput_Selection).ToColor());
		}
		else
		{
			uint32_t Index = Min.Index();
			for (uint32_t Line = Min.Line(); Line <= Max.Line(); Line++)
			{
				if (Line == Min.Line())
				{
					const std::u32string Sub = String.substr(Min.Index(), LineEndIndex(Min.Index()) - Min.Index());
					const Vector2 Size = m_Text->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation(Min);
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position,
						m_Text->GetAbsolutePosition() + Position + Vector2(Size.X, LineHeight)
					};
					Brush.Rectangle(SelectBounds, GetProperty(ThemeProperties::TextInput_Selection).ToColor());
				}
				else if (Line == Max.Line())
				{
					const uint32_t Start = LineStartIndex(Max.Index());
					const std::u32string Sub = String.substr(Start, Max.Index() - Start);
					const Vector2 Size = m_Text->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation(Max);
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position - Vector2(Size.X, 0.0f),
						m_Text->GetAbsolutePosition() + Position + Vector2(0.0f, LineHeight)
					};
					Brush.Rectangle(SelectBounds, GetProperty(ThemeProperties::TextInput_Selection).ToColor());
				}
				else
				{
					const std::u32string Sub = String.substr(Index, LineEndIndex(Index) - Index);
					const Vector2 Size = m_Text->GetFont()->Measure(Sub);
					const Vector2 Position = GetPositionLocation({ Line, 0, Index });
					const Rect SelectBounds = {
						m_Text->GetAbsolutePosition() + Position,
						m_Text->GetAbsolutePosition() + Position + Vector2(Size.X, LineHeight)
					};
					Brush.Rectangle(SelectBounds, GetProperty(ThemeProperties::TextInput_Selection).ToColor());
				}

				Index = LineEndIndex(Index) + 1;
			}
		}
	}

	if (m_Focused && m_DrawCursor)
	{
		const Vector2 Size = GetPositionLocation(m_Position);
		const Vector2 Start = m_Text->GetAbsolutePosition() + Vector2(std::max<float>(Size.X, 2.0f), Size.Y);
		const Vector2 End = Start + Vector2(0.0f, LineHeight);
		Brush.Line(Start, End, GetProperty(ThemeProperties::TextInput_Cursor).ToColor());
	}

	Brush.PopClip();

	Container::OnPaint(Brush);
}

void TextInput::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);

	m_NumbersOnly = Root["NumbersOnly"].Boolean(m_NumbersOnly);
	SetMulitline(Root["Multiline"].Boolean());
	if (m_Multiline)
	{
		if (Root["Size"].IsNull())
		{
			SetSize({ 200.0f, 200.0f });
		}
	}

	m_Text->OnLoad(Root["Text"]);
}

void TextInput::OnSave(Json& Root) const
{
	ScrollableViewControl::OnSave(Root);

	Json Position(Json::Type::Array);
	Position.Push((float)m_Position.Line());
	Position.Push((float)m_Position.Column());
	Position.Push((float)m_Position.Index());
	Root["Position"] = std::move(Position);

	Json Anchor(Json::Type::Array);
	Anchor.Push((float)m_Anchor.Line());
	Anchor.Push((float)m_Anchor.Column());
	Anchor.Push((float)m_Anchor.Index());
	Root["Anchor"] = std::move(Anchor);

	Root["Multiline"] = m_Multiline;
	Root["ReadOnly"] = m_ReadOnly;
	Root["BlinkTimer"] = (float)m_BlinkTimer->Interval();
}

void TextInput::OnResized()
{
	ScrollableViewControl::OnResized();

	m_FirstVisibleLine.Invalidate();
	UpdateVisibleLines();
	UpdateSpans();
}

void TextInput::OnThemeLoaded()
{
	ScrollableViewControl::OnThemeLoaded();

	UpdateSpans();
}

void TextInput::MovePosition(int32_t Line, int32_t Column, bool UseAnchor, bool SkipWords)
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

	const std::u32string& String = m_Text->GetString();

	if (SkipWords)
	{
		const std::u32string Search { U" \t\n" };
		const bool Reverse = Column < 0;
		size_t Start = Reverse ? m_Position.Index() : m_Position.Index() + 1;
		size_t Pos = Reverse
			? String::FindFirstOfReverse(String, Search, Start)
			: String.find_first_of(Search, Start);
		// Check for end and any consecutive skippable characters.
		while (Pos != std::string::npos && Pos == Start)
		{
			Start = Reverse ? Pos - 1 : Pos + 1;
			Pos = Reverse
				? String::FindFirstOfReverse(String, Search, Start)
				: String.find_first_of(Search, Start);
		}

		if (Pos == std::string::npos)
		{
			Column = Reverse ? 0 : String.length();
		}
		else
		{
			Column = Pos - m_Position.Index();
		}
	}

	// Special case to handle moving to the beginnging of the string if trying to go before the top line.
	if (Line < 0 && m_Position.Line() == 0)
	{
		SetPosition(0, 0, 0);
		return;
	}

	// Prevent any update if trying to go before the beginning or moveing past the end.
	if ((Line < 0 && m_Position.Line() == 0) || (Column < 0 && m_Position.Index() == 0) || (Column > 0 && m_Position.Index() == String.size()))
	{
		return;
	}

	int32_t NewIndex = m_Position.Index();
	int32_t LineIndex = LineStartIndex(m_Position.Index());
	int32_t NewLine = m_Position.Line();
	int32_t NewColumn = m_Position.Column();

	// First, figure out the line and the string index for the new line.
	// This is done by iterating each line and calculating the new offset until the desired number
	// of lines is reached.
	const bool LineBack = Line < 0;
	// No need to move the index forward if the current index is zero.
	if (m_Position.Index() > 0)
	{
		LineIndex = String[LineIndex] == '\n' ? LineIndex + 1 : LineIndex;
	}

	for (int32_t I = 0; I < std::abs(Line) && NewLine >= 0; I++)
	{
		// Need to adjust the starting search position for finding the next newline character.
		// Want to avoid cases where the same index is returned.
		const uint32_t Start = LineBack ? std::max<int32_t>(LineIndex - 1, 0) : LineIndex;
		uint32_t Index = LineBack ? LineStartIndex(Start) : LineEndIndex(Start);

		if (Index == String.size())
		{
			NewIndex = String.size();
			break;
		}
		// Catching an edge case here where the first line could end up being a single newline.
		// If this is the case, Index will actually end up being a newline because of the above
		// logic pushing the index forward if it is on a newline character. This should occur
		// for normal newlines in the middle of the buffer, but not at the beginning.
		else if (Start == 0 && LineBack)
		{
			Index = 0;
		}
		else
		{
			Index = String[Index] == '\n' ? Index + 1 : Index;
		}

		NewLine = LineBack ? NewLine - 1 : NewLine + 1;
		NewIndex = Index;
		LineIndex = Index;
	}

	NewColumn = std::min<int32_t>(NewColumn, LineSize(LineIndex));
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

	SetPosition((uint32_t)NewLine, (uint32_t)NewColumn, (uint32_t)NewIndex);
	ResetCursorTimer();
}

void TextInput::TextAdded(const std::u32string& Contents)
{
}

void TextInput::TextDeleted(const std::u32string_view& Contents)
{
}

void TextInput::MouseMove(const Vector2& Position)
{
	if (m_Drag)
	{
		m_Position = GetPosition(Position);
		Invalidate();
		UpdateSpans();
	}
}

bool TextInput::MousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
	m_Position = GetPosition(Position);
	m_Anchor = m_Position;
	m_Drag = true;
	// Remove any multi-selected spans.
	UpdateSpans();
	ResetCursorTimer();
	Invalidate();

	if (Count == Mouse::Count::Double)
	{
		SelectWord();
	}
	else if (Count == Mouse::Count::Triple)
	{
		SelectLine();
	}

	return true;
}

void TextInput::MouseReleased(const Vector2& Position, Mouse::Button Button)
{
	if (m_Anchor == m_Position)
	{
		m_Anchor.Invalidate();
	}

	m_Drag = false;
}

void TextInput::AddText(uint32_t Code)
{
	if (!std::isalnum(Code) && Code != '\n' && Code != ' ' && Code != '\t' && !std::ispunct(Code))
	{
		return;
	}

	std::u32string Value;
	Value += Code;
	AddText(Value);
}

void Remove(std::u32string& Contents, char32_t Character)
{
	size_t Length = Contents.length();

	size_t Offset = Contents.find(Character);
	while (Offset != std::string::npos)
	{
		Contents.erase(Offset, 1);
		Offset = Contents.find(Character, Offset);
	}
}

void TextInput::AddText(const std::u32string& Contents)
{
	if (m_ReadOnly)
	{
		return;
	}

	if (m_NumbersOnly)
	{
		if (Contents.find_first_not_of(U"0123456789.-") != std::string::npos)
		{
			return;
		}

		if (Contents.find_first_of(U'.') != std::string::npos && GetString().find_first_of(U'.') != std::string::npos)
		{
			return;
		}

		if (Contents.find_first_of(U'-') != std::string::npos)
		{
			// Already exists.
			if (GetString().find_first_of(U'-') != std::string::npos)
			{
				return;
			}

			// The negative sign is only allowed as the first character.
			if (m_Position.Index() != 0)
			{
				return;
			}
		}
	}

	if (!m_Position.IsValid())
	{
		m_Position = { 0, 0, 0 };
	}

	if (m_Anchor.IsValid())
	{
		Delete(GetRangeOr(0));
	}

	std::u32string Pending = std::move(Contents);
	if (m_OnModifyText)
	{
		Pending = m_OnModifyText(TShare<TextInput>(), Pending);
	}

	std::u32string Stripped = std::move(Pending);
	Remove(Stripped, '\r');

	if (!m_Multiline)
	{
		Remove(Stripped, '\n');
	}

	const uint32_t Length = (uint32_t)Stripped.length();
	std::u32string Current = m_Text->GetText();
	Current.insert(Current.begin() + m_Position.Index(), Stripped.begin(), Stripped.end());
	InternalSetText(Current.c_str());
	Scrollable()->Update();

	// Need to update the last visible line index as it has changed to the length of the string changing.
	const uint32_t Index = std::min<uint32_t>(m_LastVisibleLine.Index() + Length, m_Text->Length());
	m_LastVisibleLine = { m_LastVisibleLine.Line(), LineEndIndex(Index) - LineStartIndex(Index), Index };

	MovePosition(0, Length);

	TextAdded(Stripped);
}

void TextInput::EnterPressed()
{
	if (m_Multiline)
	{
		AddText('\n');
	}
	else
	{
		if (m_OnConfirm)
		{
			m_OnConfirm(TShare<TextInput>());
		}
	}
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
	std::u32string Contents = m_Text->GetText();
	TextDeleted({ &Contents[Min], (size_t)(Max - Min) });
	Contents.erase(Contents.begin() + (uint32_t)Min, Contents.begin() + (uint32_t)Max);

	InternalSetText(Contents.c_str());
	Scrollable()->Update();

	// Force update the visible lines
	m_FirstVisibleLine.Invalidate();
	UpdateVisibleLines();
	UpdateSpans();
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

void TextInput::SetPosition(uint32_t Line, uint32_t Column, uint32_t Index)
{
	m_Position = { Line, Column, Index };
	ScrollIntoView();
	UpdateSpans();
	Invalidate();
}

Vector2 TextInput::GetPositionLocation(const TextPosition& Position, bool OffsetFirstLine) const
{
	if (!m_Position.IsValid())
	{
		return { 0.0f, 0.0f };
	}

	const std::u32string& String = m_Text->GetString();
	uint32_t Start = LineStartIndex(Position.Index());

	const uint32_t Line = Position.Line() - (OffsetFirstLine ? m_FirstVisibleLine.Line() : 0);
	const std::u32string Sub = String.substr(Start, Position.Index() - Start);
	return { m_Text->GetFont()->Measure(Sub).X, Line * m_Text->LineHeight() };
}

TextInput::TextPosition TextInput::GetPosition(const Vector2& Position) const
{
	const float LineHeight = m_Text->LineHeight();
	const std::u32string& String = m_Text->GetString();

	// Transform into local space.
	const Vector2 LocalPosition = Position - Scrollable()->GetAbsolutePosition();
	// The text control is offset by a margin container so this needs to be taken into account.
	const Vector2 TextOffset { MARGIN, MARGIN };

	// Find the starting index based on what line the position is on.
	size_t StartIndex = m_FirstVisibleLine.IsValid() ? m_FirstVisibleLine.Index() : 0;
	size_t Index = 0;
	uint32_t Line = m_FirstVisibleLine.IsValid() ? m_FirstVisibleLine.Line() : 0;
	uint32_t Column = 0;
	Vector2 Offset = { 0.0f, LineHeight * (Line > 0 ? Line + 1 : 1) };
	while (StartIndex != std::string::npos)
	{
		if (Offset.Y > LocalPosition.Y + TextOffset.Y)
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

		const Vector2 Size = m_Text->GetFont()->Measure(Ch);
		Offset.X += Size.X;

		if (Position.X - Scrollable()->GetPosition().X - TextOffset.X <= GetAbsolutePosition().X + Offset.X)
		{
			break;
		}
	}

	return { Line, Column, (uint32_t)Index };
}

bool TextInput::IsShiftPressed() const
{
	return IsKeyPressed(Keyboard::Key::LeftShift) || IsKeyPressed(Keyboard::Key::RightShift);
}

bool TextInput::IsCtrlPressed() const
{
	return IsKeyPressed(Keyboard::Key::LeftControl) || IsKeyPressed(Keyboard::Key::RightControl);
}

bool TextInput::IsAltPressed() const
{
	return IsKeyPressed(Keyboard::Key::LeftAlt) || IsKeyPressed(Keyboard::Key::RightAlt);
}

bool TextInput::ShouldSkipWords() const
{
#ifdef APPLE
	return IsAltPressed();
#else
	return IsCtrlPressed();
#endif
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
	const std::u32string& String = m_Text->GetString();

	// The index may already be on a newline character. Start the search at the character
	// before this one.
	const uint32_t Offset = String[Index] == '\n' ? std::max<int>(Index - 1, 0) : Index;
	size_t Result = String.rfind('\n', Offset);
	return Result == std::string::npos ? 0 : Result;
}

uint32_t TextInput::LineEndIndex(uint32_t Index) const
{
	const std::u32string& String = m_Text->GetString();

	if (String[Index] == '\n')
	{
		return Index;
	}

	size_t Result = String.find('\n', Index);
	return Result == std::string::npos ? String.size() : Result;
}

uint32_t TextInput::LineSize(uint32_t Index) const
{
	const std::u32string& String = m_Text->GetString();
	uint32_t Start = LineStartIndex(Index);
	// The line should start at the character after the newline character.
	if (String[Start] == '\n')
	{
		Start++;
	}
	uint32_t End = LineEndIndex(Index);
	return std::max<int>((int)End - (int)Start, 0);
}

void TextInput::ScrollIntoView()
{
	const float LineHeight = m_Text->LineHeight();
	const Vector2 TextOffset { MARGIN, MARGIN };
	const Vector2 Position = GetPositionLocation(m_Position, false) + Scrollable()->GetPosition() + TextOffset;
	const Vector2 Size = Scrollable()->GetScrollableSize();
	Vector2 Offset;

	if (Position.X < 0.0f)
	{
		Offset.X = Position.X - 2.0f - TextOffset.X;
	}
	if (Position.Y < 0.0f)
	{
		Offset.Y = Position.Y - TextOffset.Y;
	}

	if (Position.X > Size.X)
	{
		Offset.X = Position.X - Size.X + TextOffset.X;
	}
	if (Position.Y + LineHeight > Size.Y)
	{
		Offset.Y = Position.Y + LineHeight - Size.Y + TextOffset.Y;
	}

	Scrollable()->AddOffset(Offset);
}

void TextInput::UpdateSpans()
{
	if (m_Highlighter)
	{
		std::vector<TextSpan> Spans = m_Highlighter->GetSpans(VisibleText());
		if (!Spans.empty())
		{
			for (TextSpan& Span : Spans)
			{
				Span.Start += m_FirstVisibleLine.Index();
				Span.End += m_FirstVisibleLine.Index();
			}

			m_Text->ClearSpans();
			m_Text->PushSpans(Spans);
			return;
		}
	}

	m_Text->ClearSpans();

	if (!m_Anchor.IsValid())
	{
		SetVisibleLineSpan();
		return;
	}

	if (m_Anchor != m_Position)
	{
		TextPosition Min = m_Anchor < m_Position ? m_Anchor : m_Position;
		TextPosition Max = m_Anchor < m_Position ? m_Position : m_Anchor;

		const uint32_t First = m_FirstVisibleLine.IsValid() ? m_FirstVisibleLine.Index() : 0;
		const uint32_t Last = m_LastVisibleLine.IsValid() ? m_LastVisibleLine.Index() : m_Text->Length();

		uint32_t MinIndex = std::max<uint32_t>(First, Min.Index());
		uint32_t MaxIndex = std::min<uint32_t>(Last, Max.Index());

		if (MinIndex > First)
		{
			m_Text->PushSpan({ First, MinIndex, GetProperty(ThemeProperties::Text).ToColor() });
		}

		if (MinIndex < MaxIndex)
		{
			m_Text->PushSpan({ MinIndex, MaxIndex, GetProperty(ThemeProperties::TextSelectable_Text_Hovered).ToColor() });
		}

		if (MaxIndex < Last)
		{
			m_Text->PushSpan({ Max.Index(), Last, GetProperty(ThemeProperties::Text).ToColor() });
		}
	}
	else
	{
		SetVisibleLineSpan();
	}
}

void TextInput::InternalSetText(const char32_t* InText)
{
	m_Text->SetText(InText);
	Invalidate();

	if (m_OnTextChanged)
	{
		m_OnTextChanged(TShare<TextInput>());
	}
}

void TextInput::ResetCursorTimer()
{
	m_BlinkTimer->Start();
	m_DrawCursor = true;
}

void TextInput::UpdateVisibleLines()
{
	if (!m_Multiline)
	{
		m_FirstVisibleLine = { 0, 0, 0 };
		return;
	}

	const Vector2 Position = Scrollable()->GetPosition();
	const float LineHeight = m_Text->GetFont()->Size();
	const float Y = -Position.Y;

	const uint32_t Line = Y / LineHeight;
	const uint32_t NumLines = GetSize().Y / LineHeight;
	uint32_t LastLine = Line + NumLines;

	if (m_FirstVisibleLine.Line() == Line)
	{
		return;
	}

	const std::u32string& String = m_Text->GetString();

	size_t Start = 0;
	size_t Index = 0;
	uint32_t Count = 0;

	// Find index of the first line.
	while (Count < Line)
	{
		Index = String.find('\n', Start);

		if (Index == std::string::npos)
		{
			break;
		}

		Index++;
		Start = Index;
		Count++;
	}

	Index = std::min<uint32_t>(Index, String.length());
	m_FirstVisibleLine = { Line, 0, (uint32_t)Index };

	// Find index of the last line.
	while (Count < LastLine)
	{
		Index = String.find('\n', Start);

		if (Index == std::string::npos)
		{
			break;
		}

		Index++;
		Start = Index;
		Count++;
	}

	if (Index == std::string::npos)
	{
		Index = String.length();
		LastLine = Count;
	}
	else
	{
		Index = LineEndIndex(Index);
	}

	Index = std::min<uint32_t>(Index, String.length());
	m_LastVisibleLine = { LastLine, (uint32_t)Index - LineStartIndex(Index), (uint32_t)Index };

	m_Text->SetPosition({ 0.0f, m_FirstVisibleLine.Line() * LineHeight });
}

void TextInput::SetVisibleLineSpan()
{
	if (!m_FirstVisibleLine.IsValid() || !m_LastVisibleLine.IsValid())
	{
		return;
	}

	m_Text->ClearSpans();
	m_Text->PushSpan({ m_FirstVisibleLine.Index(), m_LastVisibleLine.Index(), GetProperty(ThemeProperties::Text).ToColor() });
}

void TextInput::SelectWord()
{
	const char32_t* SpaceChars = U" \t\n\r";

	const bool IsSpace = std::isspace(Right());
	size_t Start = IsSpace
		? String::FirdFirstNotOfReverse(GetString(), SpaceChars, Index())
		: String::FindFirstOfReverse(GetString(), SpaceChars, Index());

	size_t End = IsSpace
		? GetString().find_first_not_of(SpaceChars, Index() + 1)
		: GetString().find_first_of(SpaceChars, Index() + 1);

	if (Start == std::string::npos)
	{
		Start = 0;
	}

	if (End == std::string::npos)
	{
		End = m_Text->Length();
	}

	MovePosition(0, Start - Index());
	m_Anchor = m_Position;
	MovePosition(0, End - Start, true);
}

void TextInput::SelectLine()
{
	const size_t Start = LineStartIndex(Index());
	const size_t End = LineEndIndex(Index());
	MovePosition(0, Start - Index());
	MovePosition(0, End - Index(), true);
}

}
