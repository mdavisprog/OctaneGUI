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
	: Control(InWindow)
	, m_State(State::None)
	, m_Hovered(false)
	, m_TriState(false)
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
	const Vector2 BoxSize = TexCoords.GetSize() + Vector2(3.0f, 3.0f);
	const Vector2 BoxPosition = GetAbsolutePosition() + Vector2(6.0f, GetSize().Y * 0.5f - BoxSize.Y * 0.5f);
	const Rect Bounds(BoxPosition, BoxPosition + BoxSize);
	const bool Is3D = GetProperty(ThemeProperties::Checkbox_3D).Bool();

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
		Brush.Rectangle(
			Rect(BoxPosition, BoxPosition + BoxSize),
			m_Hovered ? GetProperty(ThemeProperties::Button_Pressed).ToColor() : GetProperty(ThemeProperties::Button).ToColor()
		);
	}

	if (m_State == State::Checked)
	{
		const Vector2 Position = BoxPosition + BoxSize * 0.5f - TexCoords.GetSize() * 0.5f;
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
			BoxPosition + BoxSize - Shrink
		);
		Brush.Rectangle(Inner, GetProperty(ThemeProperties::Check).ToColor());
	}

	m_Text->OnPaint(Brush);
}

void Checkbox::Update()
{
	const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
	m_Text->SetPosition({TexCoords.GetSize().X + 12.0f, GetSize().Y * 0.5f - m_Text->GetSize().Y * 0.5f});
}

void Checkbox::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	m_Text->OnLoad(Root["Text"]);
	SetTriState(Root["TriState"].Boolean(false));
	UpdateSize();
}

bool Checkbox::OnMousePressed(const Vector2& Position, Mouse::Button Button)
{
	switch (m_State)
	{
	case State::None: m_State = m_TriState ? State::Intermediate : State::Checked; break;
	case State::Intermediate: m_State = State::Checked; break;
	case State::Checked:
	default: m_State = State::None;
	}

	Invalidate();
	return true;
}

void Checkbox::OnMouseEnter()
{
	m_Hovered = true;
	Invalidate();
}

void Checkbox::OnMouseLeave()
{
	m_Hovered = false;
	Invalidate();
}

void Checkbox::OnThemeLoaded()
{
	Control::OnThemeLoaded();

	m_Text->OnThemeLoaded();
	UpdateSize();
}

void Checkbox::UpdateSize()
{
	const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
	const Vector2 Padding =GetProperty(ThemeProperties::TextSelectable_Padding).Vector();
	Vector2 Size(TexCoords.GetSize().X + m_Text->GetSize().X, m_Text->GetSize().Y + Padding.Y * 2.0f);
	SetSize(Size);
}

}
