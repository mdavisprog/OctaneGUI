/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

#include "Windowing.h"
#include "OctaneGUI/OctaneGUI.h"

namespace Frontend
{
namespace Windowing
{

HitTestResult PerformHitTest(OctaneGUI::Window* Window, const OctaneGUI::Rect& Bounds, const OctaneGUI::Vector2& Point)
{
    if (Window == nullptr)
    {
        return HitTestResult::Normal;
    }

    const float Pad = 5.0f;
    const bool HitLeft = Bounds.Min.X - Pad <= Point.X && Point.X <= Bounds.Min.X + Pad;
    const bool HitTop = Bounds.Min.Y - Pad <= Point.Y && Point.Y <= Bounds.Min.Y + Pad;
    const bool HitRight = Bounds.Max.X - Pad <= Point.X && Point.X <= Bounds.Max.X + Pad;
    const bool HitBottom = Bounds.Max.Y - Pad <= Point.Y && Point.Y <= Bounds.Max.Y + Pad;

    if (HitLeft)
    {
        if (HitTop)
        {
            return HitTestResult::TopLeft;
        }
        else if (HitBottom)
        {
            return HitTestResult::BottomLeft;
        }

        return HitTestResult::Left;
    }
    else if (HitRight)
    {
        if (HitTop)
        {
            return HitTestResult::TopRight;
        }
        else if (HitBottom)
        {
            return HitTestResult::BottomRight;
        }

        return HitTestResult::Right;
    }
    else if (HitTop)
    {
        return HitTestResult::Top;
    }
    else if (HitBottom)
    {
        return HitTestResult::Bottom;
    }
    else if (Window->GetRootContainer()->IsInTitleBar(Point))
    {
        return HitTestResult::Draggable;
    }

    return HitTestResult::Normal;
}

}
}
