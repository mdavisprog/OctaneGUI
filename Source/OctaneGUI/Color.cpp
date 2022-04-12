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

#include "Color.h"
#include "Json.h"

namespace OctaneGUI
{

const Color Color::White = { 255, 255, 255, 255 };
const Color Color::Black = { 0, 0, 0, 255 };

Color Color::Parse(const Json& Root)
{
	if (!Root.IsArray())
	{
		return Color::White;
	}

	return {
		Root.Count() > 0 ? (uint8_t)Root[0u].Number() : (uint8_t)255,
		Root.Count() > 1 ? (uint8_t)Root[1u].Number() : (uint8_t)255,
		Root.Count() > 2 ? (uint8_t)Root[2u].Number() : (uint8_t)255,
		Root.Count() > 3 ? (uint8_t)Root[3u].Number() : (uint8_t)255
	};
}

Json Color::ToJson(const Color& Value)
{
	Json Result(Json::Type::Array);

	Result.Push((float)Value.R);
	Result.Push((float)Value.G);
	Result.Push((float)Value.B);
	Result.Push((float)Value.A);

	return std::move(Result);
}

Color::Color()
{
}

Color::Color(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA)
	: R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
{
}

bool Color::operator==(const Color& Other) const
{
	return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
}

bool Color::operator!=(const Color& Other) const
{
	return !(R == Other.R && G == Other.G && B == Other.B && A == Other.A);
}

}
