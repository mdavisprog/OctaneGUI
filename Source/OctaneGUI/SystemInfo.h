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

#pragma once

#include "Rect.h"

#include <string>
#include <vector>

namespace OctaneGUI
{

class SystemInfo
{
public:
    struct DPI
    {
    public:
        float Diagonal { 0.0f };
        float Horizontal { 0.0f };
        float Vertical { 0.0f };
    };

    struct Display
    {
    public:
        enum class Orientation
        {
            Unknown,
            Landscape,
            LandscapeFlipped,
            Portrait,
            PortraitFlipped,
        };

        std::string Name {};
        Rect Bounds {};
        Rect Usable {};
        DPI DPI_ {};
        Orientation Orientation_ { Orientation::Unknown };
    };

    SystemInfo();

    SystemInfo& AddDisplay(const Display& Display_);
    Display GetDisplay(size_t Index) const;
    Display GetDisplay(const Vector2& Position) const;

private:
    std::vector<Display> m_Displays {};
};

}
