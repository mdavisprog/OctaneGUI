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
{
	if (GetTheme())
	{
		m_Font = GetTheme()->GetFont();
	}
}

Text* Text::SetText(const char* InContents)
{
	m_Contents = InContents;

	if (m_Font)
	{
		int Lines = 0;
		m_ContentSize = m_Font->Measure(m_Contents, Lines);
		SetSize({m_ContentSize.X, m_Font->Size() * Lines});
	}

	return this;
}

const char* Text::GetText() const
{
	return m_Contents.c_str();
}

const std::string& Text::GetString() const
{
	return m_Contents;
}

uint32_t Text::Length() const
{
	return m_Contents.size();
}

const std::shared_ptr<Font>& Text::GetFont() const
{
	return m_Font;
}

float Text::LineHeight() const
{
	assert(m_Font);
	return m_Font->Size();
}

void Text::PushFormat(const Paint::TextFormat& Format)
{
	m_Formats.push_back(Format);
}

void Text::ClearFormats()
{
	m_Formats.clear();
}

void Text::OnPaint(Paint& Brush) const
{
	const Vector2 Position = GetAbsolutePosition();

	if (m_Formats.size() > 0)
	{
		Brush.Textf(m_Font, Position.Floor(), m_Contents, m_Formats);
	}
	else
	{
		Brush.Text(m_Font, Position.Floor(), m_Contents, GetProperty(ThemeProperties::Text).ToColor());
	}
}

void Text::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	SetText(Root["Text"].String());

	if (!Root["Color"].IsNull())
	{
		SetProperty(ThemeProperties::Text, Color::Parse(Root["Color"]));
	}
}

bool Text::IsFixedSize() const
{
	return true;
}

}
