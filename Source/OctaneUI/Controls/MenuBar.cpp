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
#include "../Theme.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "Menu.h"
#include "MenuBar.h"
#include "MenuItem.h"
#include "Panel.h"
#include "TextSelectable.h"

namespace OctaneUI
{

MenuBar::MenuBar(Window* InWindow)
	: Container(InWindow)
{
	SetExpand(Expand::Width);

	m_Panel = AddControl<Panel>();
	m_Panel->SetExpand(Expand::Both);

	m_Container = AddControl<HorizontalContainer>();
	m_Container->SetExpand(Expand::Both);
}

std::shared_ptr<Menu> MenuBar::AddItem(const char* InText)
{
	std::shared_ptr<MenuItem> Item = std::make_shared<MenuItem>(GetWindow());
	Item->SetIsMenuBar(true)
		.SetOnHovered([this](const TextSelectable& Item) -> void
		{
			OnHover(static_cast<const MenuItem&>(Item));
		})
		.SetOnPressed([this](const TextSelectable& Item) -> void
		{
			OnSelected(static_cast<const MenuItem&>(Item));
		})
		.SetAlignment(HorizontalAlignment::Center)
		.SetText(InText)
		.SetExpand(Expand::Height);
	m_Container->InsertControl(Item);

	if (m_MenuItems.size() == 0)
	{
		const Vector2 Padding = GetProperty(ThemeProperties::MenuBar_Padding).Vector();
		SetSize(Vector2(0.0f, Item->GetSize().Y));
	}

	m_MenuItems.push_back(Item);
	Invalidate();

	return Item->CreateMenu();
}

void MenuBar::GetMenuItems(std::vector<std::shared_ptr<MenuItem>>& Items) const
{
	Items.insert(Items.end(), m_MenuItems.begin(), m_MenuItems.end());

	for (const std::shared_ptr<MenuItem>& Item : m_MenuItems)
	{
		const std::shared_ptr<Menu> ItemMenu = Item->GetMenu();

		if (ItemMenu)
		{
			ItemMenu->GetMenuItems(Items);
		}
	}
}

void MenuBar::Close()
{
	if (m_Menu)
	{
		m_Menu->Close();
	}

	m_Open = false;
	m_Menu = nullptr;
}

void MenuBar::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);

	const Json& Menus = Root["Items"];
	for (int I = 0; I < Menus.Count(); I++)
	{
		const Json& Item = Menus[I];

		AddItem(Item["Text"].String());
		m_MenuItems.back()->OnLoad(Item);
	}

	SetExpand(Expand::Width);
}

void MenuBar::OnThemeLoaded()
{
	Container::OnThemeLoaded();

	for (const std::shared_ptr<MenuItem>& Item : m_MenuItems)
	{
		if (Item->GetMenu())
		{
			Item->GetMenu()->OnThemeLoaded();
		}
	}
}

void MenuBar::OnHover(const MenuItem& Hovered)
{
	if (m_Open)
	{
		Open(Hovered);
	}
}

void MenuBar::OnSelected(const MenuItem& Selected)
{
	if (GetWindow()->GetPopup() == Selected.GetMenu())
	{
		GetWindow()->ClosePopup();
		m_Open = false;
	}
	else
	{
		Open(Selected);
	}
}

void MenuBar::Open(const MenuItem& Item)
{
	m_Menu = Item.GetMenu();
	if (!m_Menu)
	{
		return;
	}

	m_Menu->Close();
	const Vector2 Position = Item.GetPosition();
	m_Menu->SetPosition(Position + Vector2(0.0f, GetSize().Y));
	GetWindow()->SetPopup(m_Menu);
	m_Open = true;
	Invalidate();
}

}
