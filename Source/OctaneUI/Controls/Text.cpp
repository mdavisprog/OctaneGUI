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

#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Theme.h"
#include "Text.h"

namespace OctaneUI
{

Text::Text(Window* InWindow)
	: Control(InWindow)
	, m_Contents("")
{
}

Text::~Text()
{
}

Text* Text::SetText(const char* InContents)
{
	m_Contents = InContents;

	std::shared_ptr<Font> ThemeFont = GetTheme()->GetFont();
	if (ThemeFont)
	{
		Vector2 Size = ThemeFont->Measure(m_Contents);
		SetSize(Size);
	}

	return this;
}

const char* Text::GetText() const
{
	return m_Contents.c_str();
}

uint32_t Text::Length() const
{
	return m_Contents.size();
}

void Text::OnPaint(Paint& Brush) const
{
	Brush.Text(GetAbsolutePosition(), m_Contents, Color(255, 255, 255, 255));
}

void Text::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	SetText(Root["Text"].String());
}

bool Text::IsFixedSize() const
{
	return true;
}

}
