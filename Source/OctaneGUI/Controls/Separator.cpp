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

#include "Separator.h"
#include "../Paint.h"

namespace OctaneGUI
{

Separator::Separator(Window* InWindow)
	: Control(InWindow)
{
	SetExpand(Expand::Width);
	SetSize({ 0.0f, 16.0f });
}

Separator& Separator::SetOnHover(OnControlSignature Fn)
{
	m_OnHover = Fn;
	return *this;
}

void Separator::OnPaint(Paint& Brush) const
{
	const Color Fill = GetProperty(ThemeProperties::Separator).ToColor();
	const float Thickness = GetProperty(ThemeProperties::Separator_Thickness).Float();
	const float Margins = GetProperty(ThemeProperties::Separator_Margins).Float();

	const Vector2 HalfSize = GetSize() * 0.5f;
	const Vector2 Start = GetAbsolutePosition() + Vector2(Margins, HalfSize.Y);
	const Vector2 End = GetAbsolutePosition() + Vector2(GetSize().X - Margins, HalfSize.Y);

	Brush.Line(Start, End, Fill, Thickness);
}

void Separator::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	SetExpand(Expand::Width);
}

void Separator::OnMouseEnter()
{
	if (m_OnHover)
	{
		m_OnHover(*this);
	}
}

}
