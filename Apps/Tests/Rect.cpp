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

#include "TestSuite.h"
#include "OctaneGUI/OctaneGUI.h"

namespace Tests
{

TEST_SUITE(Rect,

TEST_CASE(Intersection,
{
    const OctaneGUI::Rect RectA(0.0f, 0.0f, 10.0f, 8.0f);
    const OctaneGUI::Rect RectB(2.0f, 3.0f, 7.0f, 9.0f);
    const OctaneGUI::Rect RectC = RectA.Intersection(RectB);
    return RectC == OctaneGUI::Rect(2.0f, 3.0f, 7.0f, 8.0f);
})

TEST_CASE(NoIntersection,
{
    const OctaneGUI::Rect RectA(0.0f, 0.0f, 10.0f, 8.0f);
    const OctaneGUI::Rect RectB(2.0f, 3.0f, 5.0f, 6.0f);
    const OctaneGUI::Rect RectC = RectA.Intersection(RectB);
    return RectC == OctaneGUI::Rect(2.0f, 3.0f, 5.0f, 6.0f);
})

)

}
