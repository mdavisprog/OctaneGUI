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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"

namespace Tests
{

TEST_SUITE(FlyString,

TEST_CASE(SingleEntry,
{
    const char* Literal = "Well Hello Friends";
    OctaneGUI::FlyString Value1 { Literal };
    OctaneGUI::FlyString Value2 { Value1 };
    OctaneGUI::FlyString Value3 = Literal;
    return Literal != Value1.Data() && Value1.Data() == Value2.Data() && Value1.Data() == Value3.Data() && Value2.Data() == Value3.Data();
})

TEST_CASE(SameLiteral,
{
    OctaneGUI::FlyString Value1 { "Well Hello Friends" };
    OctaneGUI::FlyString Value2 { "Well Hello Friends" };
    return Value1.Data() == Value2.Data();
})

)

}
