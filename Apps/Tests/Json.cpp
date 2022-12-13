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

TEST_SUITE(Json,

TEST_CASE(IsNull,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("");
    return Root.IsNull();
})

TEST_CASE(IsNullValue,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("null", IsError);
    return !IsError && Root.IsNull();
})

TEST_CASE(IsObject,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("{}");
    return Root.IsObject();
})

TEST_CASE(IsArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[]");
    return Root.IsArray();
})

TEST_CASE(IsBoolean,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("true");
    return Root.IsBoolean() && Root.Boolean() == true;
})

TEST_CASE(IsNumber,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("1.0");
    return Root.IsNumber() && Root.Number() == 1.0f;
})

TEST_CASE(IsString,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"("String")");
    return Root.IsString() && std::string(Root.String()) == "String";
})

TEST_CASE(BooleanArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[true, false, false]");
    VERIFYF(Root.Count() == 3, "Incorrect number of elements: %d", Root.Count())
    VERIFY(Root[0u].Boolean() == true)
    VERIFY(Root[1u].Boolean() == false)
    VERIFY(Root[2u].Boolean() == false)
    return true;
})

TEST_CASE(NumberArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[3.14, -1.0, 42]");
    VERIFYF(Root.Count() == 3, "Incorrect number of elements: %d", Root.Count())
    VERIFY(Root[0u].Number() == 3.14f)
    VERIFY(Root[1u].Number() == -1.0f)
    VERIFY(Root[2u].Number() == 42.0f)
    return true;
})

TEST_CASE(StringArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"(["Apple", "Orange", "Banana"])");
    VERIFYF(Root.Count() == 3, "Incorrect number of elements: %d", Root.Count())
    VERIFY(std::string(Root[0u].String()) == "Apple")
    VERIFY(std::string(Root[1u].String()) == "Orange")
    VERIFY(std::string(Root[2u].String()) == "Banana")
    return true;
})

TEST_CASE(ObjectArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"([{"Key": "One"}, {"Key": "Two"}, {"Key": "Three"}])");
    VERIFYF(Root.Count() == 3, "Incorrect number of objects: %d", Root.Count())

    const OctaneGUI::Json& One = Root[0u];
    VERIFY(std::string(One["Key"].String()) == "One")

    const OctaneGUI::Json& Two = Root[1u];
    VERIFY(std::string(Two["Key"].String()) == "Two")

    const OctaneGUI::Json& Three = Root[2u];
    VERIFY(std::string(Three["Key"].String()) == "Three")

    return true;
})

TEST_CASE(Object,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Number": 1, "Boolean": true, "String": "Hello World"})");

    float Number = Root["Number"].Number();
    VERIFYF(Number == 1.0f, "Number field is incorrect: %f", Number)

    bool Boolean = Root["Boolean"].Boolean();
    VERIFYF(Boolean == true, "Boolean field is incorrect")

    std::string String = Root["String"].String();
    VERIFYF(String == "Hello World", "String field is incorrect: %s", String.c_str())

    VERIFYF(Root["Invalid"].IsNull(), "Invalid field should not exist!")

    return true;
})

TEST_CASE(SubObject,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Sub": {"Value": 255}})");
    const OctaneGUI::Json& Sub = Root["Sub"];
    VERIFYF(Sub.IsObject(), "Sub value is not an object!")
    const float Value = Sub["Value"].Number();
    VERIFYF(Value == 255.0f, "Value %f != 255", Value)
    return true;
})

TEST_CASE(CopyString,
{
    OctaneGUI::Json Root = "Hello";
    OctaneGUI::Json Copy = 0.0f;
    Copy = Root;
    return Root.IsString() && Copy.IsString() && Root == Copy;
})

TEST_CASE(CopyArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[1, 4, 2, 3]");
    OctaneGUI::Json Copy = "Hello";
    Copy = Root;
    return Root.IsArray() && Copy.IsArray() && Root == Copy;
})

TEST_CASE(CopyObject,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Bool": true, "Number": 3.14, "String": "Hello Friends"})");
    OctaneGUI::Json Copy = "Hello";
    Copy = Root;
    return Root.IsObject() && Copy.IsObject() && Root == Copy;
})

TEST_CASE(MoveArray,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[1, 4, 2, 3]");
    OctaneGUI::Json Move = "";
    Move = std::move(Root);
    return Root.IsNull() && Move.IsArray() && Root != Move;
})

TEST_CASE(MoveObject,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Bool": true, "Number": 3.14, "String": "Hello Friends"})");
    OctaneGUI::Json Move = "";
    Move = std::move(Root);
    return Root.IsNull() && Move.IsObject() && Root != Move;
})

TEST_CASE(Spaces,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("([\n{\n\"One\": 1\n},{\n\"Two\": 2\n}\n]");
    return Root.IsArray() && Root[0u]["One"].Number() == 1.0f && Root[1u]["Two"].Number() == 2.0f;
})

TEST_CASE(EmptyObject,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Empty": {}, "Filled": {"Property": "One"}})");
    const OctaneGUI::Json& Empty = Root["Empty"];
    const OctaneGUI::Json& Filled = Root["Filled"];
    VERIFYF(Empty.IsObject() && Empty.Count() == 0, "Empty object is not empty!");
    VERIFYF(Filled.IsObject() && Filled.Count() == 1, "Filled object is incorrect!");
    return std::string(Filled["Property"].String()) == "One";
})

TEST_CASE(CommaInString,
{
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Text": "Hello, World"})");
    return std::string(Root["Text"].String()) == "Hello, World";
})

TEST_CASE(InvalidKey_NoStartingQuote,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({Hello": "World"})", IsError);
    return IsError && Root["Column"].Number() == 2.0f;
})

TEST_CASE(InvalidKey_NoEndingQuote,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Hello: "World"})", IsError);
    // The first quote for "World" is parsed as the ending quote for "Hello". The parser
    // expects a valid ':' character but it was not found due to it being parsed as part of
    // the key. So the error will be that it did not find the ':' character at the expected
    // location before the 'W' character.
    return IsError && Root["Column"].Number() == 11.0f;
})

TEST_CASE(InvalidObjectError,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Text": "Hello")", IsError);
    return IsError;
})

TEST_CASE(InvalidObjectSyntaxError,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Object": "Hello": "World"}})", IsError);
    return IsError && Root["Column"].Number() == 19.0f;
})

TEST_CASE(InvalidSubObjectError,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"Object1": {"Hello": "World"}, "Object2": {"Foo: "Bar"}})", IsError);
    return IsError && Root["Column"].Number() == 52.0f;
})

TEST_CASE(InvalidArrayError,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[1, 2, 3", IsError);
    return IsError && Root["Column"].Number() == 9.0f;
})

TEST_CASE(InvalidArrayDelimiter,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("[1, 2 3]", IsError);
    return IsError && Root["Column"].Number() == 7.0f;
})

TEST_CASE(InvalidValue,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("nil", IsError);
    return IsError;
})

TEST_CASE(InvalidNumber,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse("34fe", IsError);
    return IsError;
})

TEST_CASE(InvalidMultiline,
{
    bool IsError = false;
    const char* Json = R"({
    "Object 1": {
        "Boolean": true,
        "Invalid": invalid
    }
})";
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(Json, IsError);
    return IsError && Root["Line"].Number() == 5.0f;
})

TEST_CASE(ValidEmptyString,
{
    bool IsError = false;
    OctaneGUI::Json Root = OctaneGUI::Json::Parse(R"({"": true})", IsError);
    return !IsError;
})

)

}
