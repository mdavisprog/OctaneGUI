/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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
#include "../../Color.h"
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

Highlighter& Highlighter::SetKeywords(const std::vector<std::u32string>& Keywords)
{
    m_Keywords = Keywords;
    return *this;
}

const std::vector<std::u32string>& Highlighter::Keywords() const
{
    return m_Keywords;
}

Color Highlighter::DefaultColor() const
{
    return m_Input.TextColor();
}

std::vector<TextSpan> Highlighter::GetSpans(const std::u32string_view& View) const
{
    std::vector<TextSpan> Result;

    if (m_Keywords.empty())
    {
        return Result;
    }

    std::vector<TextSpan> Spans;
    for (const std::u32string& Keyword : m_Keywords)
    {
        size_t Offset = 0;
        size_t Pos = View.find(Keyword, Offset);
        while (Pos != std::u32string_view::npos)
        {
            size_t End = std::min<size_t>(View.length(), Pos + Keyword.length());
            Spans.push_back({ Pos, End, { 0, 255, 0, 255 } });
            Offset += Keyword.length();
            Pos = View.find(Keyword, Offset);
        }
    }

    std::sort(Spans.begin(), Spans.end(), [](const TextSpan& A, const TextSpan& B) -> bool
        {
            return A.Start < B.Start;
        });

    size_t Start = 0;
    for (const TextSpan& Span : Spans)
    {
        if (Start < Span.Start)
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

}
}
