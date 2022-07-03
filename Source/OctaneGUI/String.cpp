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

#include "String.h"

namespace OctaneGUI
{

size_t String::FindFirstOfReverse(const std::u32string& Ref, const std::u32string& Search, size_t Pos)
{
	if (Pos == 0)
	{
		return std::string::npos;
	}

	size_t Start = Pos - 1;
	size_t Result = Ref.find_first_of(Search, Start);
	while (Result != 0 && Result > Start)
	{
		Start = Start - 1;
		Result = Ref.find_first_of(Search, Start);
	}

	if (Result == 0)
	{
		Result = std::string::npos;
	}
	else
	{
		Result++;
	}

	return Result;
}

size_t String::Count(const std::u32string_view& Ref, char32_t Character)
{
	size_t Result = 0;

	size_t Start = 0;
	size_t Pos = Ref.find(Character, Start);
	while (Pos != std::string::npos)
	{
		Result++;
		Start = Pos + 1;
		Pos = Ref.find(Character, Start);
	}

	return Result;
}

}
