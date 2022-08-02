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

#include "Event.h"

namespace OctaneGUI
{

Event::Data::Data()
    : m_MouseMove()
{
}

Event::Event(Type InType)
    : m_Type(InType)
    , m_Data()
{
}

Event::Event(Type InType, const Key& InKey)
    : m_Type(InType)
{
    m_Data.m_Key = InKey;
}

Event::Event(const MouseMove& InMouseMove)
    : m_Type(Type::MouseMoved)
{
    m_Data.m_MouseMove = InMouseMove;
}

Event::Event(Type InType, const MouseButton& InMouseButton)
    : m_Type(InType)
{
    m_Data.m_MouseButton = InMouseButton;
}

Event::Event(const MouseWheel& InMouseWheel)
    : m_Type(Type::MouseWheel)
{
    m_Data.m_MouseWheel = InMouseWheel;
}

Event::Event(const Text& InText)
    : m_Type(Type::Text)
{
    m_Data.m_Text = InText;
}

Event::Event(const WindowResized& InResized)
    : m_Type(Type::WindowResized)
{
    m_Data.m_Resized = InResized;
}

Event::~Event()
{
}

Event::Type Event::GetType() const
{
    return m_Type;
}

const Event::Data& Event::GetData() const
{
    return m_Data;
}

Event::Event()
    : m_Type(Type::None)
{
}

}
