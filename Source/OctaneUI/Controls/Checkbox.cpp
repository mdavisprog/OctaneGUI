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

#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Window.h"
#include "Checkbox.h"
#include "Text.h"

namespace OctaneUI
{

Checkbox::Checkbox(Window* InWindow)
	: Button(InWindow)
{
	m_Text = std::make_shared<Text>(InWindow);
	m_Text->SetParent(this);
}

Checkbox::~Checkbox()
{
}

Checkbox* Checkbox::SetText(const char* InText)
{
	m_Text->SetText(InText);
	Update();
	UpdateSize();
	return this;
}

const char* Checkbox::GetText() const
{
	return m_Text->GetText();
}

Checkbox* Checkbox::SetState(State InState)
{
	m_State = InState;
	return this;
}

Checkbox::State Checkbox::GetState() const
{
	return m_State;
}

Checkbox* Checkbox::SetTriState(bool IsTriState)
{
	m_TriState = IsTriState;

	if (m_TriState)
	{
		if (m_State == State::Intermediate)
		{
			m_State = State::Checked;
			Invalidate();
		}
	}

	return this;
}

bool Checkbox::IsTriState() const
{
	return m_TriState;
}

void Checkbox::OnPaint(Paint& Brush) const
{
	const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
	const Vector2 Size = BoxSize();
	const Vector2 BoxPosition = GetAbsolutePosition() + Vector2(0.0f, GetSize().Y * 0.5f - Size.Y * 0.5f);
	const Rect Bounds(BoxPosition, BoxPosition + Size);
	const bool Is3D = GetProperty(ThemeProperties::Button_3D).Bool();

	Brush.RectangleOutline(GetAbsoluteBounds(), Color(255, 0, 0, 255));

	if (Is3D)
	{
		Brush.Rectangle3D(
			Bounds,
			GetProperty(ThemeProperties::Button_Pressed).ToColor(),
			GetProperty(ThemeProperties::Button_Highlight_3D).ToColor(),
			GetProperty(ThemeProperties::Button_Shadow_3D).ToColor(),
			true
		);
	}
	else
	{
		Color Background = GetProperty(ThemeProperties::Button).ToColor();
		if (IsHovered())
		{
			Background = GetProperty(ThemeProperties::Button_Hovered).ToColor();
		}
		else if (IsPressed())
		{
			Background = GetProperty(ThemeProperties::Button_Pressed).ToColor();
		}

		Brush.Rectangle(Rect(BoxPosition, BoxPosition + Size), Background);
	}

	if (m_State == State::Checked)
	{
		const Vector2 Position = BoxPosition + Size * 0.5f - TexCoords.GetSize() * 0.5f;
		Brush.Image(
			Rect(Position, Position + TexCoords.GetSize()),
			GetWindow()->GetIcons()->GetUVsNormalized(Icons::Type::Check),
			GetWindow()->GetIcons()->GetTexture(),
			GetProperty(ThemeProperties::Check).ToColor()
		);
	}
	else if (m_State == State::Intermediate)
	{
		const Vector2 Shrink = {3.0f, 3.0f};
		Rect Inner = Rect(
			BoxPosition + Shrink,
			BoxPosition + Size - Shrink
		);
		Brush.Rectangle(Inner, GetProperty(ThemeProperties::Check).ToColor());
	}

	m_Text->OnPaint(Brush);
}

void Checkbox::Update()
{
	m_Text->SetPosition({BoxSize().X + 12.0f, GetSize().Y * 0.5f - m_Text->GetSize().Y * 0.5f});
}

void Checkbox::OnLoad(const Json& Root)
{
	Button::OnLoad(Root);

	m_Text->OnLoad(Root["Text"]);
	SetTriState(Root["TriState"].Boolean(false));

	Update();
	UpdateSize();
}

void Checkbox::OnThemeLoaded()
{
	Control::OnThemeLoaded();

	m_Text->OnThemeLoaded();
	Update();
	UpdateSize();
}

void Checkbox::OnClicked()
{
	switch (m_State)
	{
	case State::None: m_State = m_TriState ? State::Intermediate : State::Checked; break;
	case State::Intermediate: m_State = State::Checked; break;
	case State::Checked:
	default: m_State = State::None;
	}

	Invalidate();
}

Vector2 Checkbox::BoxSize() const
{
	const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
	return TexCoords.GetSize() + Vector2(3.0f, 3.0f);
}

void Checkbox::UpdateSize()
{
	const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
	const Vector2 Padding = GetProperty(ThemeProperties::TextSelectable_Padding).Vector();
	Vector2 Size(m_Text->GetPosition().X + m_Text->GetSize().X, m_Text->GetSize().Y + Padding.Y * 2.0f);
	SetSize(Size);
}

}
