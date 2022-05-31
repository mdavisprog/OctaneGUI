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

#include "RadioButton.h"
#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"
#include "../Theme.h"
#include "Container.h"
#include "Text.h"

#include <cmath>

namespace OctaneGUI
{

RadioButton::RadioButton(Window* InWindow)
	: Button(InWindow)
{
	m_Text = std::make_shared<Text>(InWindow);
	m_Text->SetParent(this);
	m_Radius = std::round(m_Text->GetFont()->Size() * 0.5f);

	m_Text->SetPosition({ m_Radius * 2.0f, 0.0f });
}

RadioButton& RadioButton::SetText(const char32_t* InText)
{
	m_Text->SetText(InText);
	Layout();
	return *this;
}

const char32_t* RadioButton::GetText() const
{
	return m_Text->GetText();
}

RadioButton& RadioButton::SetSelected(bool Selected)
{
	if (m_Selected == Selected)
	{
		return *this;
	}

	m_Selected = Selected;

	if (Selected)
	{
		Container* Parent = dynamic_cast<Container*>(GetParent());
		if (Parent != nullptr)
		{
			for (const std::shared_ptr<Control>& Item : Parent->Controls())
			{
				if (Item.get() == this)
				{
					continue;
				}

				const std::shared_ptr<RadioButton> ItemButton = std::dynamic_pointer_cast<RadioButton>(Item);
				if (ItemButton)
				{
					ItemButton->SetSelected(false);
				}
			}
		}
	}

	Invalidate();
	return *this;
}

bool RadioButton::IsSelected() const
{
	return m_Selected;
}

void RadioButton::OnPaint(Paint& Brush) const
{
	PROFILER_SAMPLE_GROUP("RadioButton::OnPaint");

	Color BackgroundTint = GetProperty(ThemeProperties::RadioButton).ToColor();
	Color OutlineTint = GetProperty(ThemeProperties::RadioButton_Outline).ToColor();
	float OutlineThickness = GetProperty(ThemeProperties::RadioButton_Outline_Thickness).Float();
	if (IsHovered())
	{
		BackgroundTint = GetProperty(ThemeProperties::RadioButton_Hovered).ToColor();
	}
	else if (IsPressed())
	{
		BackgroundTint = GetProperty(ThemeProperties::RadioButton_Pressed).ToColor();
	}

	const Vector2 Position = GetAbsolutePosition() + Vector2(m_Radius * 0.5f, m_Radius);
	Brush.Circle(Position, m_Radius, BackgroundTint);
	Brush.CircleOutline(Position, m_Radius, OutlineTint, OutlineThickness);

	if (m_Selected)
	{
		Brush.Circle(Position, std::round(m_Radius * 0.5f), GetProperty(ThemeProperties::RadioButton_Selected).ToColor());
	}

	m_Text->OnPaint(Brush);
}

void RadioButton::OnLoad(const Json& Root)
{
	m_Text->OnLoad(Root["Text"]);
	m_Radius = Root["Radius"].Number(m_Radius);

	Layout();
}

void RadioButton::OnClicked()
{
	SetSelected(true);
}

void RadioButton::Layout()
{
	const Vector2 TextSize = m_Text->GetSize();
	m_Radius = std::round(TextSize.Y * 0.5f);
	const float Offset = m_Radius * 2.5;
	m_Text->SetPosition({ Offset, 0.0f });
	SetSize({ m_Radius + Offset + TextSize.X, TextSize.Y });
}

}
