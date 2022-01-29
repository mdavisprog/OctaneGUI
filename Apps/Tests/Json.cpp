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

#include "OctaneUI/OctaneUI.h"
#include "TestSuite.h"

namespace Tests
{

TEST_SUITE(Json,

TEST_CASE(IsNull,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("");
	return Root.IsNull();
})

TEST_CASE(IsObject,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("{}");
	return Root.IsObject();
})

TEST_CASE(IsArray,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("[]");
	return Root.IsArray();
})

TEST_CASE(IsBoolean,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("true");
	return Root.IsBoolean() && Root.GetBoolean() == true;
})

TEST_CASE(IsNumber,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("1.0");
	return Root.IsNumber() && Root.GetNumber() == 1.0f;
})

TEST_CASE(IsString,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("\"String\"");
	return Root.IsString() && std::string(Root.GetString()) == "String";
})

TEST_CASE(BooleanArray,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("[true, false, false]");
	VERIFYF(Root.GetCount() == 3, "Incorrect number of elements: %d\n", Root.GetCount())
	VERIFY(Root[0u].GetBoolean() == true)
	VERIFY(Root[1u].GetBoolean() == false)
	VERIFY(Root[2u].GetBoolean() == false)
	return true;
})

TEST_CASE(NumberArray,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("[3.14, -1.0, 42]");
	VERIFYF(Root.GetCount() == 3, "Incorrect number of elements: %d\n", Root.GetCount())
	VERIFY(Root[0u].GetNumber() == 3.14f)
	VERIFY(Root[1u].GetNumber() == -1.0f)
	VERIFY(Root[2u].GetNumber() == 42.0f)
	return true;
})

TEST_CASE(StringArray,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("[\"Apple\", \"Orange\", \"Banana\"]");
	VERIFYF(Root.GetCount() == 3, "Incorrect number of elements: %d\n", Root.GetCount())
	VERIFY(std::string(Root[0u].GetString()) == "Apple")
	VERIFY(std::string(Root[1u].GetString()) == "Orange")
	VERIFY(std::string(Root[2u].GetString()) == "Banana")
	return true;
})

TEST_CASE(ObjectArray,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("[{\"Key\": \"One\"}, {\"Key\": \"Two\"}, {\"Key\": \"Three\"}]");
	VERIFYF(Root.GetCount() == 3, "Incorrect number of objects: %d\n", Root.GetCount())

	const OctaneUI::Json& One = Root[0u];
	VERIFY(std::string(One["Key"].GetString()) == "One")

	const OctaneUI::Json& Two = Root[1u];
	VERIFY(std::string(Two["Key"].GetString()) == "Two")

	const OctaneUI::Json& Three = Root[2u];
	VERIFY(std::string(Three["Key"].GetString()) == "Three")

	return true;
})

TEST_CASE(Object,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("{\"Number\": 1, \"Boolean\": true, \"String\": \"Hello World\"}");

	float Number = Root["Number"].GetNumber();
	VERIFYF(Number == 1.0f, "Number field is incorrect: %f\n", Number)

	bool Boolean = Root["Boolean"].GetBoolean();
	VERIFYF(Boolean == true, "Boolean field is incorrect\n")

	std::string String = Root["String"].GetString();
	VERIFYF(String == "Hello World", "String field is incorrect: %s\n", String.c_str())

	VERIFYF(Root["Invalid"].IsNull(), "Invalid field should not exist!\n")

	return true;
})

TEST_CASE(SubObject,
{
	OctaneUI::Json Root = OctaneUI::Json::Parse("{\"Sub\": {\"Value\": 255}}");
	const OctaneUI::Json& Sub = Root["Sub"];
	VERIFYF(Sub.IsObject(), "Sub value is not an object!\n")
	const float Value = Sub["Value"].GetNumber();
	VERIFYF(Value == 255.0f, "Value %f != 255\n", Value)
	return true;
})

)

}
