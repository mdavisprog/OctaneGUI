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

#include "CustomControl.h"

namespace OctaneGUI
{

CustomControl::CustomControl(Window* InWindow)
    : Control(InWindow)
{
}

CustomControl& CustomControl::SetOnPaint(OnPaintSignature&& Fn)
{
    m_OnPaint = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnUpdate(OnCustomControlSignature&& Fn)
{
    m_OnUpdate = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnLoad(OnLoadSignature&& Fn)
{
    m_OnLoad = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnSave(OnSaveSignature&& Fn)
{
    m_OnSave = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnKeyPressed(OnKeyPressedSignature&& Fn)
{
    m_OnKeyPressed = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnKeyReleased(OnKeyReleasedSignature&& Fn)
{
    m_OnKeyReleased = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMouseMove(OnMouseMoveSignature&& Fn)
{
    m_OnMouseMove = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMousePressed(OnMousePressedSignature&& Fn)
{
    m_OnMousePressed = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMouseReleased(OnMouseReleasedSignature&& Fn)
{
    m_OnMouseReleased = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMouseWheel(OnMouseWheelSignature&& Fn)
{
    m_OnMouseWheel = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMouseEnter(OnCustomControlSignature&& Fn)
{
    m_OnMouseEnter = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnMouseLeave(OnCustomControlSignature&& Fn)
{
    m_OnMouseLeave = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnResized(OnCustomControlSignature&& Fn)
{
    m_OnResized = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnText(OnTextSignature&& Fn)
{
    m_OnText = std::move(Fn);
    return *this;
}

CustomControl& CustomControl::SetOnThemeLoaded(OnCustomControlSignature&& Fn)
{
    m_OnThemeLoaded = std::move(Fn);
    return *this;
}

void CustomControl::OnPaint(Paint& Brush) const
{
    if (m_OnPaint)
    {
        m_OnPaint(*this, Brush);
    }
}

void CustomControl::Update()
{
    if (m_OnUpdate)
    {
        m_OnUpdate(*this);
    }
}

void CustomControl::OnLoad(const Json& Root)
{
    if (m_OnLoad)
    {
        m_OnLoad(*this, Root);
    }
}

void CustomControl::OnSave(Json& Root) const
{
    if (m_OnSave)
    {
        m_OnSave(*this, Root);
    }
}

bool CustomControl::OnKeyPressed(Keyboard::Key Key)
{
    if (m_OnKeyPressed)
    {
        return m_OnKeyPressed(*this, Key);
    }

    return false;
}

void CustomControl::OnKeyReleased(Keyboard::Key Key)
{
    if (m_OnKeyReleased)
    {
        m_OnKeyReleased(*this, Key);
    }
}

void CustomControl::OnMouseMove(const Vector2& Position)
{
    if (m_OnMouseMove)
    {
        m_OnMouseMove(*this, Position);
    }
}

bool CustomControl::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    if (m_OnMousePressed)
    {
        return m_OnMousePressed(*this, Position, Button, Count);
    }

    return false;
}

void CustomControl::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
    if (m_OnMouseReleased)
    {
        m_OnMouseReleased(*this, Position, Button);
    }
}

void CustomControl::OnMouseWheel(const Vector2& Delta)
{
    if (m_OnMouseWheel)
    {
        m_OnMouseWheel(*this, Delta);
    }
}

void CustomControl::OnMouseEnter()
{
    if (m_OnMouseEnter)
    {
        m_OnMouseEnter(*this);
    }
}

void CustomControl::OnMouseLeave()
{
    if (m_OnMouseLeave)
    {
        m_OnMouseLeave(*this);
    }
}

void CustomControl::OnResized()
{
    if (m_OnResized)
    {
        m_OnResized(*this);
    }
}

void CustomControl::OnText(uint32_t Code)
{
    if (m_OnText)
    {
        m_OnText(*this, Code);
    }
}

void CustomControl::OnThemeLoaded()
{
    if (m_OnThemeLoaded)
    {
        m_OnThemeLoaded(*this);
    }
}

}
