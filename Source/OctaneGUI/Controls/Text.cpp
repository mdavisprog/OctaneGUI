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

#include "Text.h"
#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"
#include "../String.h"
#include "../Theme.h"

#include <cassert>

namespace OctaneGUI
{

Text::Text(Window* InWindow)
	: Control(InWindow)
{
	if (GetTheme())
	{
		m_Font = GetTheme()->GetFont();
	}
}

Text& Text::SetText(const char* InContents)
{
	SetText(String::ToUTF32(InContents).c_str());
	return *this;
}

Text& Text::SetText(const char32_t* InContents)
{
	m_Contents = InContents;
	UpdateSize();
	return *this;
}

const char32_t* Text::GetText() const
{
	return m_Contents.c_str();
}

const std::u32string& Text::GetString() const
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

Text& Text::SetFont(const char* Path)
{
	SetProperty(ThemeProperties::FontPath, Path);
	UpdateFont();
	return *this;
}

Text& Text::SetFontSize(float Size)
{
	SetProperty(ThemeProperties::FontSize, Size);
	UpdateFont();
	return *this;
}

Text& Text::SetWrap(bool Wrap)
{
	m_Wrap = Wrap;
	return *this;
}

bool Text::Wrap() const
{
	return m_Wrap;
}

void Text::PushSpan(const TextSpan& Span)
{
	m_Spans.push_back(Span);
}

void Text::PushSpans(const std::vector<TextSpan>& Spans)
{
	m_Spans.insert(m_Spans.end(), Spans.begin(), Spans.end());
}

void Text::ClearSpans()
{
	m_Spans.clear();
}

void Text::Update()
{
	UpdateSize();
}

void Text::OnPaint(Paint& Brush) const
{
	PROFILER_SAMPLE_GROUP("Text::OnPaint");

	const Vector2 Position = GetAbsolutePosition();

	if (m_Spans.size() > 0)
	{
		if (m_Wrap)
		{
			Brush.TextWrapped(m_Font, Position.Floor(), m_Contents, m_Spans, GetSize().X);
		}
		else
		{
			Brush.Textf(m_Font, Position.Floor(), m_Contents, m_Spans);
		}
	}
	else
	{
		Color TextColor = GetProperty(ThemeProperties::Text).ToColor();
		if (m_Wrap)
		{
			Brush.TextWrapped(m_Font, Position.Floor(), m_Contents, { { 0, m_Contents.size(), TextColor } }, GetSize().X);
		}
		else
		{
			Brush.Text(m_Font, Position.Floor(), m_Contents, TextColor);
		}
	}
}

void Text::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	SetProperty(ThemeProperties::FontPath, Root["Font"]);
	SetProperty(ThemeProperties::FontSize, Root["FontSize"]);
	SetProperty(ThemeProperties::Text, Root["Color"]);

	UpdateFont();

	SetText(Root["Text"].String());
	SetWrap(Root["Wrap"].Boolean());
}

void Text::OnSave(Json& Root) const
{
	Control::OnSave(Root);

	Root["Text"] = String::ToMultiByte(m_Contents);
	Root["ContentSize"] = std::move(Vector2::ToJson(m_ContentSize));
	Root["Font"] = m_Font->Path();
	Root["FontSize"] = GetProperty(ThemeProperties::FontSize).Float();
	Root["Wrap"] = m_Wrap;
}

void Text::OnThemeLoaded()
{
	UpdateFont();
	UpdateSize();
}

bool Text::IsFixedSize() const
{
	return !m_Wrap;
}

void Text::UpdateFont()
{
	const char* FontPath = GetProperty(ThemeProperties::FontPath).String(nullptr);
	const float FontSize = GetProperty(ThemeProperties::FontSize).Float(LineHeight());
	m_Font = GetTheme()->GetOrAddFont(FontPath, FontSize);
}

void Text::UpdateSize()
{
	if (m_Font)
	{
		int Lines = 0;
		if (m_Wrap && GetParent() != nullptr)
		{
			const float Width = GetParent()->GetSize().X;
			m_ContentSize = m_Font->Measure(m_Contents, Lines, Width);
		}
		else
		{
			m_ContentSize = m_Font->Measure(m_Contents, Lines);
		}

		SetSize({ m_ContentSize.X, m_Font->Size() * Lines });
	}
}

}
