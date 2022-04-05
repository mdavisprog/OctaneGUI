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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"

namespace Tests
{

TEST_SUITE(Variant,

TEST_CASE(Null,
{
	OctaneGUI::Variant Variant;
	return Variant.IsNull();
})

TEST_CASE(Bool,
{
	OctaneGUI::Variant Variant = true;
	return Variant.IsBool() && Variant.Bool() == true;
})

TEST_CASE(Byte,
{
	OctaneGUI::Variant Variant = (unsigned char)255;
	return Variant.IsByte() && Variant.Byte() == 255;
})

TEST_CASE(Int,
{
	OctaneGUI::Variant Variant = 42;
	return Variant.IsInt() && Variant.Int() == 42;
})

TEST_CASE(Float,
{
	OctaneGUI::Variant Variant = 3.14f;
	return Variant.IsFloat() && Variant.Float() == 3.14f;
})

TEST_CASE(String,
{
	OctaneGUI::Variant Variant = "Hello";
	return Variant.IsString() && std::string(Variant.String()) == "Hello";
})

TEST_CASE(Vector,
{
	const OctaneGUI::Vector2 Vector = OctaneGUI::Vector2(3.14f, 42.0f);
	OctaneGUI::Variant Variant = Vector;
	return Variant.IsVector() && Variant.Vector() == Vector;
})

TEST_CASE(Color,
{
	const OctaneGUI::Color Color(255, 0, 255, 255);
	OctaneGUI::Variant Variant = Color;
	return Variant.IsColor() && Variant.ToColor() == Color;
})

TEST_CASE(Copy,
{
	OctaneGUI::Variant Variant = "Hello";
	OctaneGUI::Variant Copy = 0;
	Copy = Variant;
	return Variant.IsString() && Copy.IsString() && std::string(Variant.String()) == Copy.String();
})

TEST_CASE(Move,
{
	OctaneGUI::Variant Variant = "Hello";
	OctaneGUI::Variant Move = "World";
	Move = std::move(Variant);
	return Variant.IsNull() && Move.IsString() && std::string(Move.String()) == "Hello";
})

)

}
