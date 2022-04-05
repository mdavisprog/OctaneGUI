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

#include "TextSelectable.h"
#include "../Json.h"
#include "../Paint.h"
#include "../ThemeProperties.h"
#include "Text.h"

namespace OctaneGUI
{

TextSelectable::TextSelectable(Window* InWindow)
	: Control(InWindow)
{
	m_Text = std::make_shared<Text>(InWindow);
	m_Text->SetParent(this);
}

TextSelectable::~TextSelectable()
{
}

TextSelectable& TextSelectable::SetSelected(bool Selected)
{
	m_Selected = Selected;
	return *this;
}

bool TextSelectable::IsSelected() const
{
	return m_Selected;
}

TextSelectable& TextSelectable::SetText(const char* Contents)
{
	m_Text->SetText(Contents);
	UpdateSize();
	return *this;
}

const char* TextSelectable::GetText() const
{
	return m_Text->GetText();
}

TextSelectable& TextSelectable::SetAlignment(HorizontalAlignment Align)
{
	m_Align = Align;
	Invalidate();
	return *this;
}

HorizontalAlignment TextSelectable::GetAlignment() const
{
	return m_Align;
}

TextSelectable& TextSelectable::SetOnHovered(OnTextSelectableSignature Fn)
{
	m_OnHovered = Fn;
	return *this;
}

TextSelectable& TextSelectable::SetOnPressed(OnTextSelectableSignature Fn)
{
	m_OnPressed = Fn;
	return *this;
}

void TextSelectable::OnPaint(Paint& Brush) const
{
	if (m_Hovered || m_Selected)
	{
		Brush.Rectangle(GetAbsoluteBounds(), GetProperty(ThemeProperties::TextSelectable_Hovered).ToColor());
		m_Text->SetProperty(ThemeProperties::Text, GetProperty(ThemeProperties::TextSelectable_Text_Hovered));
	}

	m_Text->OnPaint(Brush);
	m_Text->ClearProperty(ThemeProperties::Text);
}

void TextSelectable::Update()
{
	const Vector2 Offset = GetProperty(ThemeProperties::TextSelectable_Offset).Vector();
	const float CenterY = GetSize().Y * 0.5f - m_Text->GetSize().Y * 0.5f;
	Vector2 Position = Offset + Vector2(0.0f, CenterY);

	switch (m_Align)
	{
	case HorizontalAlignment::Center:
		Position = GetSize() * 0.5f - m_Text->GetSize() * 0.5f;
		break;

	case HorizontalAlignment::Right:
		Position = Vector2(GetBounds().Max.X, GetBounds().Min.Y + CenterY) - Offset - Vector2(m_Text->GetSize().X, 0.0f);
		break;

	default: break;
	}

	m_Text->SetPosition(Position);
}

void TextSelectable::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	m_Text->OnLoad(Root["Text"]);
	UpdateSize();
}

bool TextSelectable::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	if (m_OnPressed)
	{
		m_OnPressed(*this);
	}

	return true;
}

void TextSelectable::OnMouseEnter()
{
	m_Hovered = true;

	if (m_OnHovered)
	{
		m_OnHovered(*this);
	}

	Invalidate();
}

void TextSelectable::OnMouseLeave()
{
	m_Hovered = false;
	Invalidate();
}

void TextSelectable::OnThemeLoaded()
{
	m_Text->OnThemeLoaded();
	UpdateSize();
}

std::shared_ptr<Text> TextSelectable::GetTextControl() const
{
	return m_Text;
}

void TextSelectable::UpdateSize()
{
	const Vector2 Padding = GetProperty(ThemeProperties::TextSelectable_Padding).Vector();
	const Vector2 Size = m_Text->GetBounds().Expand(Padding).GetSize();
	SetSize(Size);
}

}
