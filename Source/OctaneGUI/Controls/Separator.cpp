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

#include "Separator.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"

namespace OctaneGUI
{

Separator::Separator(Window* InWindow)
    : Control(InWindow)
{
    SetExpand(Expand::Width);
    SetSize({ 0.0f, 16.0f });
}

Separator& Separator::SetOnHover(OnControlSignature&& Fn)
{
    m_OnHover = std::move(Fn);
    return *this;
}

Separator& Separator::SetOrientation(Orientation InOrientation)
{
    if (m_Orientation == InOrientation)
    {
        return *this;
    }

    m_Orientation = InOrientation;
    UpdateLayout();
    Invalidate(InvalidateType::Both);
    return *this;
}

Orientation Separator::GetOrientation() const
{
    return m_Orientation;
}

void Separator::OnPaint(Paint& Brush) const
{
    PROFILER_SAMPLE_GROUP("Separator::OnPaint");

    const Color Fill = GetProperty(ThemeProperties::Separator).ToColor();
    const float Thickness = GetProperty(ThemeProperties::Separator_Thickness).Float();
    const float Margins = GetProperty(ThemeProperties::Separator_Margins).Float();

    const Vector2 HalfSize = GetSize() * 0.5f;
    Vector2 Start;
    Vector2 End;

    if (m_Orientation == Orientation::Vertical)
    {
        Start = GetAbsolutePosition() + Vector2(HalfSize.X, Margins);
        End = GetAbsolutePosition() + Vector2(HalfSize.X, GetSize().Y - Margins);
    }
    else
    {
        Start = GetAbsolutePosition() + Vector2(Margins, HalfSize.Y);
        End = GetAbsolutePosition() + Vector2(GetSize().X - Margins, HalfSize.Y);
    }

    Brush.Line(Start, End, Fill, Thickness);
}

void Separator::OnLoad(const Json& Root)
{
    Control::OnLoad(Root);

    m_Orientation = ToOrientation(Root["Orientation"].String());
    UpdateLayout();
}

void Separator::OnMouseEnter()
{
    if (m_OnHover)
    {
        m_OnHover(*this);
    }
}

void Separator::UpdateLayout()
{
    if (m_Orientation == Orientation::Vertical)
    {
        SetExpand(Expand::Height);
        SetSize({ 16.0f, 0.0f });
    }
    else
    {
        SetExpand(Expand::Width);
        SetSize({ 0.0f, 16.0f });
    }
}

}
