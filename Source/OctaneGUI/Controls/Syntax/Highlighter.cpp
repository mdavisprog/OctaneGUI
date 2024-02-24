/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

#include "Highlighter.h"
#include "../../TextSpan.h"
#include "../TextInput.h"

#include <algorithm>

namespace OctaneGUI
{
namespace Syntax
{

Highlighter::Highlighter(TextInput& Input)
    : m_Input(Input)
{
}

Highlighter::~Highlighter()
{
}

Highlighter& Highlighter::SetSymbols(const std::vector<std::u32string>& Symbols)
{
    m_Symbols = Symbols;
    return *this;
}

const std::vector<std::u32string>& Highlighter::Symbols() const
{
    return m_Symbols;
}

Highlighter& Highlighter::SetRules(Rules& Rules_)
{
    m_Rules = Rules_;
    return *this;
}

Color Highlighter::DefaultColor() const
{
    return m_Input.TextColor();
}

Highlighter& Highlighter::SetKeywordColor(Color KeywordColor)
{
    m_KeywordColor = KeywordColor;
    return *this;
}

Color Highlighter::KeywordColor() const
{
    return m_KeywordColor;
}

static bool IsInSpans(size_t Position, const std::vector<TextSpan>& Spans)
{
    for (const TextSpan& Span : Spans)
    {
        if (Span.Start <= Position && Position < Span.End)
        {
            return true;
        }
    }

    return false;
}

static bool IsWord(const Rules& Rules_, const std::u32string_view& View, const std::u32string& Keyword, size_t Position)
{
    const size_t Start { Position > 0 ? Position - 1 : 0 };
    const size_t End { Position + Keyword.length() };

    const bool IsStartValid { Start == 0 ? Rules_.IsValidIdentifier(View[Start]) : !Rules_.IsValidIdentifier(View[Start]) };
    const bool IsEndValid { End >= View.length() ? Rules_.IsValidIdentifier(View[View.length() - 1]) : !Rules_.IsValidIdentifier(View[End]) };

    return IsStartValid && IsEndValid;
}

static std::vector<TextSpan> SpansFrom(const Rules& Rules_, const std::u32string_view& View, const std::vector<std::u32string>& List, const std::vector<TextSpan>& Ranges, Color Tint)
{
    std::vector<TextSpan> Result;

    for (const std::u32string& Item : List)
    {
        size_t Pos = View.find(Item);
        while (Pos != std::u32string_view::npos)
        {
            if (!IsInSpans(Pos, Ranges) && IsWord(Rules_, View, Item, Pos))
            {
                size_t End = std::min<size_t>(View.length(), Pos + Item.length());
                Result.push_back({ Pos, End, Tint });
            }

            Pos = View.find(Item, Pos + Item.length());
        }
    }

    return Result;
}

std::vector<TextSpan> Highlighter::GetSpans(const std::u32string_view& View) const
{
    std::vector<TextSpan> Result;

    if (!ShouldHighlight())
    {
        return Result;
    }

    std::vector<TextSpan> RangeSpans;
    for (const Range& Range_ : m_Rules.Ranges)
    {
        size_t Pos = View.find(Range_.Start);
        size_t End = View.find(Range_.End);

        // Multiline range checking. The start of the range may be out of view,
        // so if the end of the range is found, then assume everything before
        // it is marked with the starting tag (e.g. multiline comments).
        if (Range_.MultiLine)
        {
            if (End < Pos && End != std::u32string_view::npos)
            {
                size_t EndPos = End + Range_.End.length();
                RangeSpans.push_back({ 0, EndPos, Range_.Tint });
                Pos = View.find(Range_.Start, EndPos);
            }
        }

        while (Pos != std::u32string_view::npos)
        {
            const size_t Offset { Pos + Range_.Start.length() };
            End = std::min<size_t>(View.length(), View.find(Range_.End, Offset));
            size_t EndPos = End + Range_.End.length();
            if (!Range_.MultiLine)
            {
                const size_t LineEnd { View.find(U"\n", Offset) };
                if (LineEnd < End)
                {
                    EndPos = LineEnd + 1;
                }
            }
            RangeSpans.push_back({ Pos, EndPos, Range_.Tint });
            Pos = View.find(Range_.Start, EndPos);
        }
    }

    std::vector<TextSpan> KeywordSpans = SpansFrom(m_Rules, View, m_Rules.Keywords, RangeSpans, m_KeywordColor);
    std::vector<TextSpan> SymbolSpans = SpansFrom(m_Rules, View, m_Symbols, RangeSpans, m_SymbolColor);

    std::vector<TextSpan> Spans;
    Spans.insert(Spans.end(), RangeSpans.begin(), RangeSpans.end());
    Spans.insert(Spans.end(), KeywordSpans.begin(), KeywordSpans.end());
    Spans.insert(Spans.end(), SymbolSpans.begin(), SymbolSpans.end());

    std::sort(Spans.begin(), Spans.end(), [](const TextSpan& A, const TextSpan& B) -> bool
        {
            return A.Start < B.Start;
        });

    size_t Start = 0;
    for (const TextSpan& Span : Spans)
    {
        if (Start <= Span.Start)
        {
            Result.push_back({ Start, Span.Start, DefaultColor() });
            Result.push_back(Span);
            Start = Span.End;
        }
    }

    if (Start < View.length())
    {
        Result.push_back({ Start, View.length(), DefaultColor() });
    }

    return Result;
}

bool Highlighter::ShouldHighlight() const
{
    return !m_Rules.IsEmpty() || !m_Symbols.empty();
}

}
}
