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

#pragma once

#include <algorithm>
#include <cctype>
#include <functional>
#include <string>

namespace OctaneGUI
{
namespace Syntax
{

template <typename CHAR>
class Lexer
{
public:
	class Token
	{
	public:
		Token()
		{
		}

		Token(CHAR const* Ptr, size_t Length)
			: m_Ptr(Ptr)
			, m_Length(Length)
		{
		}

		std::string_view ToAnsi() const
		{
			return { m_Ptr, m_Length };
		}

		std::u32string_view ToUTF32() const
		{
			return { m_Ptr, m_Length };
		}

		size_t Length() const
		{
			return m_Length;
		}

		bool IsEmpty() const
		{
			return m_Length == 0;
		}

		CHAR const* Ptr() const
		{
			return m_Ptr;
		}

		void Clear()
		{
			m_Ptr = nullptr;
			m_Length = 0;
		}

	private:
		CHAR const* m_Ptr { nullptr };
		size_t m_Length { 0 };
	};

	typedef std::function<void(const Token&)> OnTokenSignature;
	typedef std::function<void(const CHAR)> OnSymbolSignature;

	Lexer()
	{
	}

	bool Begin(CHAR const* Buffer, size_t Length)
	{
		m_Buffer = Buffer;
		m_Length = Length;
		m_Index = 0;

		SkipWhitespace();

		size_t Pos = 0;
		while (!IsEnd())
		{
			CHAR Symbol = Current();
			if (IsSymbolRegistered(Symbol))
			{
				EmitToken(Pos);

				if (m_OnEmitSymbol)
				{
					m_OnEmitSymbol(Symbol);
				}

				Pos = m_Index + 1;
			}
			else if (IsWhitespace())
			{
				EmitToken(Pos);
				Pos = m_Index + 1;
			}

			Next();
		}

		return true;
	}

	CHAR Current() const
	{
		if (m_Index >= m_Length)
		{
			return 0;
		}

		return *(m_Buffer + m_Index);
	}

	CHAR Peek() const
	{
		const size_t Index = m_Index + 1;
		if (Index >= m_Length)
		{
			return 0;
		}
		return *(m_Buffer + Index);
	}

	CHAR PeekNextValid() const
	{
		size_t Index = m_Index + 1;

		while (Index < m_Length)
		{
			const CHAR Ch = *(m_Buffer + Index);

			if (!std::isspace(Ch))
			{
				return Ch;
			}

			Index++;
		}

		return 0;
	}

	CHAR Next()
	{
		if (m_Index >= m_Length)
		{
			return Current();
		}

		m_Index++;
		return Current();
	}

	size_t Index() const
	{
		return m_Index;
	}

	Token ParseUntil(const CHAR Symbol)
	{
		const size_t Index = m_Index;
		while (Current() != Symbol && !IsEnd())
		{
			Next();
		}
		return { m_Buffer + Index, m_Index - Index };
	}

	bool IsWhitespace() const
	{
		return std::isspace(Current());
	}

	CHAR SkipWhitespace()
	{
		while (!IsEnd() && IsWhitespace())
		{
			Next();
		}

		return Current();
	}

	bool IsEnd() const
	{
		return m_Index >= m_Length;
	}

	Lexer& RegisterSymbols(const std::vector<CHAR>& Symbols)
	{
		m_Symbols = Symbols;
		return *this;
	}

	Lexer& SetOnEmitToken(OnTokenSignature&& Fn)
	{
		m_OnEmitToken = std::move(Fn);
		return *this;
	}

	Lexer& SetOnEmitSymbol(OnSymbolSignature&& Fn)
	{
		m_OnEmitSymbol = std::move(Fn);
		return *this;
	}

private:
	bool IsSymbolRegistered(const CHAR Symbol) const
	{
		if (m_Symbols.empty())
		{
			return false;
		}

		return std::find(m_Symbols.begin(), m_Symbols.end(), Symbol) != m_Symbols.end();
	}

	void EmitToken(size_t Start) const
	{
		if (Start >= m_Index)
		{
			return;
		}

		if (m_OnEmitToken)
		{
			m_OnEmitToken({ m_Buffer + Start, m_Index - Start });
		}
	}

	CHAR const* m_Buffer { nullptr };
	size_t m_Length { 0 };
	size_t m_Index { 0 };
	std::vector<CHAR> m_Symbols {};
	OnTokenSignature m_OnEmitToken { nullptr };
	OnSymbolSignature m_OnEmitSymbol { nullptr };
};

typedef Lexer<char> LexerAnsi;
typedef Lexer<char32_t> LexerUTF32;

}
}
