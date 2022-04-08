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

#include "Tree.h"
#include "../Icons.h"
#include "../Json.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "ImageButton.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

Tree::Tree(Window* InWindow)
	: Container(InWindow)
{
	m_Row = AddControl<HorizontalContainer>();
	m_Toggle = m_Row->AddControl<ImageButton>();
	m_Toggle
		->SetOnClicked([this](const Button&) -> void
			{
				SetExpand(!m_Expand);
			})
		.SetProperty(ThemeProperties::Button_Padding, Vector2())
		.SetProperty(ThemeProperties::Button, Color())
		.SetProperty(ThemeProperties::Button_Hovered, Color())
		.SetProperty(ThemeProperties::Button_Pressed, Color())
		.SetProperty(ThemeProperties::Button_3D, false);

	m_Text = m_Row->AddControl<Text>();

	EnableToggle(false);
}

std::shared_ptr<Tree> Tree::AddChild(const char* Label)
{
	if (!m_List)
	{
		m_List = AddControl<VerticalContainer>();
		m_List->SetPosition({ m_Toggle->GetSize().X + 4.0f, m_Row->DesiredSize().Y });
		EnableToggle(true);
		SetExpand(true);
	}

	std::shared_ptr<Tree> Result = m_List->AddControl<Tree>();
	Result->SetLabel(Label);
	InvalidateLayout();
	return Result;
}

Tree& Tree::SetLabel(const char* Label)
{
	m_Text->SetText(Label);
	Invalidate();
	return *this;
}

const char* Tree::Label() const
{
	return m_Text->GetText();
}

Vector2 Tree::DesiredSize() const
{
	Vector2 Result = m_Row->DesiredSize();

	if (m_List && m_Expand)
	{
		Result += m_List->DesiredSize();
	}

	return Result;
}

void Tree::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);

	SetLabel(Root["Text"].String());

	const Json& Items = Root["Items"];
	Items.ForEach([this](const Json& Item) -> void
		{
			std::shared_ptr<Tree> Child = AddChild("");
			Child->OnLoad(Item);
		});
}

void Tree::OnThemeLoaded()
{
	m_Toggle->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
	Container::OnThemeLoaded();
}

void Tree::EnableToggle(bool Enable)
{
	if (Enable && m_Toggle->IsDisabled())
	{
		m_Toggle
			->SetTexture(GetWindow()->GetIcons()->GetTexture())
			.SetDisabled(false);
	}
	else if (!m_Toggle->IsDisabled())
	{
		m_Toggle->SetTexture(nullptr);
		m_Toggle->SetDisabled(true);
	}

	// Overriding the texture resets the size so need to update here.
	m_Toggle->SetSize({ 16.0f, 16.0f });
}

void Tree::SetExpand(bool Expand)
{
	if (m_Expand == Expand)
	{
		return;
	}

	if (!m_List)
	{
		m_Expand = false;
		return;
	}

	m_Expand = Expand;
	const Rect UVs = m_Expand ? GetWindow()->GetIcons()->GetUVs(Icons::Type::Expand) : GetWindow()->GetIcons()->GetUVs(Icons::Type::Collapse);
	m_Toggle
		->SetUVs(UVs)
		.SetSize({ 16.0f, 16.0f });
	m_Toggle->Update();

	if (m_Expand)
	{
		if (!HasControl(m_List))
		{
			InsertControl(m_List);
		}
	}
	else
	{
		RemoveControl(m_List);
	}

	Invalidate(InvalidateType::Both);
}

}
