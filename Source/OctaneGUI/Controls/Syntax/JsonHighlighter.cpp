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

#include "JsonHighlighter.h"
#include "../../String.h"
#include "../../TextSpan.h"
#include "Lexer.h"

namespace OctaneGUI
{
namespace Syntax
{

JsonHighlighter::JsonHighlighter()
	: Highlighter()
{
}

std::vector<TextSpan> JsonHighlighter::Update(const std::u32string_view& Span) const
{
	std::vector<TextSpan> Result;

	size_t Pos = 0;
	LexerUTF32 Lex {};
	Lex
		.SetOnEmitToken([&](const LexerUTF32::Token& Token) -> void
			{
			})
		.SetOnEmitSymbol([&](const char32_t Symbol) -> void
			{
				if (Symbol == U'"')
				{
					Result.push_back({ Pos, Lex.Index(), DefaultColor() });
					Pos = Lex.Index();
					Lex.Next();
					LexerUTF32::Token Token = Lex.ParseUntil(U'"');

					const char32_t Next = Lex.PeekNextValid();
					Color Tint { 255, 255, 255, 255 };
					if (Next == U':')
					{
						Tint = { 120, 250, 254, 255 };
					}
					else if (Next == U',' || Next == U'}' || Next == U']')
					{
						Tint = { 206, 145, 120, 255 };
					}
					Lex.Next();
					Result.push_back({ Pos, Lex.Index(), Tint });

					Pos = Lex.Index();
					Lex.Next();
				}
			})
		.RegisterSymbols({ U'"' })
		.Begin(Span.data(), Span.length());

	// TODO: Should move this to EmitToken callback. That should be fired for any remaining characters left.
	if (Pos < Span.length())
	{
		Result.push_back({ Pos, Span.length(), DefaultColor() });
	}

	return Result;
}

}
}
