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

#include "SystemInfo.h"

namespace OctaneGUI
{

SystemInfo::SystemInfo()
{
}

SystemInfo& SystemInfo::AddDisplay(const Display& Display_)
{
    m_Displays.push_back(Display_);
    return *this;
}

SystemInfo::Display SystemInfo::GetDisplay(size_t Index) const
{
    if (Index >= m_Displays.size())
    {
        return {};
    }

    return m_Displays[Index];
}

SystemInfo::Display SystemInfo::GetDisplay(const Vector2& Position) const
{
    for (const Display& Item : m_Displays)
    {
        if (Item.Bounds.Contains(Position))
        {
            return Item;
        }
    }

    return {};
}

}
