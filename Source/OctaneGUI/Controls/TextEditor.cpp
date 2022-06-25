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

#include "TextEditor.h"
#include "../Json.h"
#include "../Paint.h"

namespace OctaneGUI
{

TextEditor::TextEditor(Window* InWindow)
	: TextInput(InWindow)
{
	SetMulitline(true);

	SetOnModifyText([this](std::shared_ptr<TextInput>, const std::u32string& Pending) -> std::u32string
		{
			return ModifyText(Pending);
		});
	
	SetOnPrePaintText([this](std::shared_ptr<TextInput const> Input, Paint& Brush) -> void
		{
			PaintLineColors(Input, Brush);
		});
}

TextEditor& TextEditor::SetMatchIndent(bool MatchIndent)
{
	m_MatchIndent = MatchIndent;
	return *this;
}

bool TextEditor::MatchIndent() const
{
	return m_MatchIndent;
}

TextEditor& TextEditor::SetLineColor(const size_t Line, const Color& _Color)
{
	m_LineColors[Line] = _Color;
	return *this;
}

TextEditor& TextEditor::ClearLineColor(const size_t Line)
{
	m_LineColors.erase(Line);
	return *this;
}

TextEditor& TextEditor::ClearLineColors()
{
	m_LineColors.clear();
	return *this;
}

void TextEditor::OnLoad(const Json& Root)
{
	TextInput::OnLoad(Root);

	SetMatchIndent(Root["MatchIndent"].Boolean(m_MatchIndent));

	SetMulitline(true);
}

std::u32string TextEditor::ModifyText(const std::u32string& Pending) const
{
	if (Pending != U"\n" || !m_MatchIndent)
	{
		return Pending;
	}

	std::u32string Line { this->Line() };

	size_t TabCount = 0;
	size_t Pos = Line.find_first_of('\t');
	while (Pos == TabCount + 1)
	{
		TabCount++;
		Pos = Line.find_first_of('\t', Pos + 1);
	}

	std::u32string Result = Pending;
	if (TabCount > 0)
	{
		Result += std::u32string(TabCount, '\t');
	}

	return Result;
}

void TextEditor::PaintLineColors(std::shared_ptr<TextInput const>& Input, Paint& Brush) const
{
	for (const std::pair<size_t, Color>& Line : m_LineColors)
	{
		Rect Bounds { GetAbsoluteBounds() };
		Bounds.Min.Y += (Line.first - 1) * LineHeight();
		Bounds.Max.Y = Bounds.Min.Y + LineHeight();
		Brush.Rectangle(Bounds, Line.second);
	}
}

}
