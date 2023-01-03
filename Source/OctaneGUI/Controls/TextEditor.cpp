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
#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "ScrollableContainer.h"

namespace OctaneGUI
{

TextEditor::TextEditor(Window* InWindow)
    : TextInput(InWindow)
{
    SetMultiline(true);

    SetOnModifyText([this](std::shared_ptr<TextInput>, const std::u32string& Pending) -> std::u32string
        {
            return ModifyText(Pending);
        });

    SetOnPrePaintText([this](std::shared_ptr<TextInput const> Input, Paint& Brush) -> void
        {
            PaintLineColors(Input, Brush);
        });
    
    AddWordDelimiters(U"\",");
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

    SetMultiline(true);
}

void TextEditor::TextAdded(const std::u32string& Contents)
{
    const bool Back = Contents == U"\"\""
        || Contents == U"''"
        || Contents == U"{}"
        || Contents == U"[]"
        || Contents == U"()";

    if (Back)
    {
        MovePosition(0, -1);
    }
}

std::u32string TextEditor::ModifyText(const std::u32string& Pending)
{
    if (Pending == U"\n")
    {
        return MatchIndent(Pending);
    }

    std::u32string Result = Pending;
    if (Pending == U"\"" || Pending == U"'")
    {
        Result = MatchCharacter(Pending, Pending[0]);

        if (Result.empty())
        {
            MovePosition(0, 1);
        }
    }

    if (Pending == U"{")
    {
        Result = Pending + U"}";
    }

    if (Pending == U"[")
    {
        Result = Pending + U"]";
    }

    if (Pending == U"(")
    {
        Result = Pending + U")";
    }

    if (Pending == U"}" || Pending == U"]" || Pending == U")")
    {
        if (Right() == Pending.front())
        {
            Result.clear();
            MovePosition(0, 1);
        }
    }

    if (InsertSpaces())
    {
        Result = ConvertTabs(Result);
    }

    return Result;
}

std::u32string TextEditor::MatchIndent(const std::u32string& Pending) const
{
    if (!m_MatchIndent)
    {
        return Pending;
    }

    std::u32string Line { this->Line() };

    size_t SpaceCount = 0;
    size_t TabCount = 0;
    size_t Pos = Line.find_first_not_of(U" \t\n");
    if (Pos != std::string::npos && Pos > 0)
    {
        for (size_t I = 0; I < Pos; I++)
        {
            if (Line[I] == U' ')
            {
                SpaceCount++;
            }
            else if (Line[I] == U'\t')
            {
                TabCount++;
            }
        }
    }

    TabCount += SpaceCount / Font::TabSize();

    std::u32string Result = Pending;
    if (TabCount > 0)
    {
        Result += std::u32string(TabCount, U'\t');
    }

    if (InsertSpaces())
    {
        Result = ConvertTabs(Result);
    }

    return Result;
}

std::u32string TextEditor::MatchCharacter(const std::u32string& Pending, char32_t Character) const
{
    if (String::Count(Line(), Character) % 2 == 0)
    {
        if (Right() == Character)
        {
            return U"";
        }

        return Pending + Character;
    }

    return Pending;
}

std::u32string TextEditor::ConvertTabs(const std::u32string& Pending) const
{
    std::u32string Result;

    size_t Start = 0;
    size_t Pos = Pending.find(U'\t');
    while (Pos != std::string::npos)
    {
        Result += Pending.substr(Start, Pos - Start);
        Result += std::u32string(Font::TabSize(), U' ');
        Start = Pos + 1;
        Pos = Pending.find(U'\t', Start);
    }

    if (Start < Pos)
    {
        Result += Pending.substr(Start, Pending.length() - Start);
    }

    return Result;
}

void TextEditor::PaintLineColors(std::shared_ptr<TextInput const>&, Paint& Brush) const
{
    for (const std::pair<size_t, Color> Line : m_LineColors)
    {
        Rect Bounds { Scrollable()->GetAbsoluteBounds() };
        Bounds.Max.X = Bounds.Min.X + Scrollable()->ContentSize().X;
        Bounds.Min.Y += (Line.first - 1) * LineHeight();
        Bounds.Max.Y = Bounds.Min.Y + LineHeight();
        Brush.Rectangle(Bounds, Line.second);
    }
}

bool TextEditor::InsertSpaces() const
{
    return GetProperty(ThemeProperties::TextEditor_InsertSpaces).Bool();
}

}
