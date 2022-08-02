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

#include "Button.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"

namespace OctaneGUI
{

Button::Button(Window* InWindow)
    : Control(InWindow)
{
}

Button& Button::SetOnPressed(OnButtonSignature&& Fn)
{
    m_OnPressed = std::move(Fn);
    return *this;
}

Button& Button::SetOnClicked(OnButtonSignature&& Fn)
{
    m_OnClicked = std::move(Fn);
    return *this;
}

Button& Button::SetOnReleased(OnButtonSignature&& Fn)
{
    m_OnReleased = std::move(Fn);
    return *this;
}

Button& Button::SetDisabled(bool Disabled)
{
    m_Disabled = Disabled;
    return *this;
}

bool Button::IsDisabled() const
{
    return m_Disabled;
}

bool Button::IsHovered() const
{
    return m_State == State::Hovered;
}

bool Button::IsPressed() const
{
    return m_State == State::Pressed;
}

Button& Button::SetRadius(float Radius)
{
    return SetRadius({ Radius, Radius, Radius, Radius });
}

Button& Button::SetRadius(const Rect& Radius)
{
    m_Radius = Radius;
    return *this;
}

Rect Button::Radius() const
{
    return m_Radius;
}

bool Button::HasRadius() const
{
    return !m_Radius.Min.IsZero() || !m_Radius.Max.IsZero();
}

void Button::OnPaint(Paint& Brush) const
{
    PROFILER_SAMPLE_GROUP("Button::OnPaint");

    std::shared_ptr<Theme> TheTheme = GetTheme();

    const bool Is3D = GetProperty(ThemeProperties::Button_3D).Bool();

    Color BackgroundColor = GetProperty(ThemeProperties::Button).ToColor();
    switch (m_State)
    {
    case State::Hovered:
        if (!Is3D)
        {
            BackgroundColor = GetProperty(ThemeProperties::Button_Hovered).ToColor();
        }
        break;
    case State::Pressed: BackgroundColor = GetProperty(ThemeProperties::Button_Pressed).ToColor(); break;
    default: break;
    }

    if (Is3D)
    {
        const Rect Bounds = GetAbsoluteBounds();
        const Color Highlight = GetProperty(ThemeProperties::Button_Highlight_3D).ToColor();
        const Color Shadow = GetProperty(ThemeProperties::Button_Shadow_3D).ToColor();

        if (HasRadius())
        {
            Brush.Rectangle3DRounded(Bounds, m_Radius, BackgroundColor, Highlight, Shadow, m_State == State::Pressed);
        }
        else
        {
            Brush.Rectangle3D(Bounds, BackgroundColor, Highlight, Shadow, m_State == State::Pressed);
        }
    }
    else
    {
        if (HasRadius())
        {
            Brush.RectangleRounded(GetAbsoluteBounds(), BackgroundColor, m_Radius);
        }
        else
        {
            Brush.Rectangle(GetAbsoluteBounds(), BackgroundColor);
        }
    }
}

void Button::OnLoad(const Json& Root)
{
    Control::OnLoad(Root);

    m_Disabled = Root["Disabled"].Boolean();
    SetProperty(ThemeProperties::Button_3D, Root["3D"]);

    const Json& Radius = Root["Radius"];
    if (!Radius.IsNull())
    {
        if (Radius.IsNumber())
        {
            SetRadius(Radius.Number());
        }
        else
        {
            SetRadius(Rect::FromJson(Radius, m_Radius));
        }
    }
}

void Button::OnSave(Json& Root) const
{
    Control::OnSave(Root);

    Root["Disabled"] = m_Disabled;
    Root["State"] = ToString(m_State);
}

bool Button::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    if (m_State == State::Hovered)
    {
        m_State = State::Pressed;
        OnPressed();

        if (m_OnPressed)
        {
            m_OnPressed(*this);
        }

        Invalidate();
        return true;
    }

    return false;
}

void Button::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
    if (m_Disabled)
    {
        return;
    }

    const bool Hovered = Contains(Position);

    if (m_State == State::Pressed)
    {
        OnReleased();

        if (m_OnReleased)
        {
            m_OnReleased(*this);
        }

        if (Hovered)
        {
            OnClicked();

            if (m_OnClicked)
            {
                m_OnClicked(*this);
            }
        }
    }

    State NewState = Hovered ? State::Hovered : State::None;
    if (m_State != NewState)
    {
        m_State = NewState;
        Invalidate();
    }
}

void Button::OnMouseEnter()
{
    if (m_Disabled)
    {
        return;
    }

    if (m_State != State::Pressed)
    {
        m_State = State::Hovered;
    }

    Invalidate();
}

void Button::OnMouseLeave()
{
    if (m_Disabled)
    {
        return;
    }

    if (m_State != State::Pressed)
    {
        m_State = State::None;
    }

    Invalidate();
}

void Button::OnPressed()
{
}

void Button::OnReleased()
{
}

void Button::OnClicked()
{
}

const char* Button::ToString(State Type)
{
    switch (Type)
    {
    case State::Hovered: return "Hovered";
    case State::Pressed: return "Pressed";
    case State::None:
    default: break;
    }

    return "None";
}

}
