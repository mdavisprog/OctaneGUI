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
	, m_Open(false)
{
	SetExpand(Expand::Width);

	m_Panel = AddPanel();
	m_Panel->SetExpand(Expand::Both);

	m_Container = AddHorizontalContainer();
	m_Container->SetExpand(Expand::Both);
}

MenuBar::~MenuBar()
{
}

std::shared_ptr<Menu> MenuBar::AddItem(const char* Label)
{
	std::shared_ptr<MenuItem> Item = std::make_shared<MenuItem>(GetWindow());
	Item->SetIsMenuBar(true)
		->SetOnHover(std::bind(&MenuBar::OnHover, this, std::placeholders::_1))
		->SetOnSelected(std::bind(&MenuBar::OnSelected, this, std::placeholders::_1))
		->SetAlignment(HorizontalAlignment::Center)
		->SetText(Label)
		->SetExpand(Expand::Height);
	m_Container->InsertControl(Item);

	if (m_MenuItems.size() == 0)
	{
		const Vector2 Padding = GetTheme()->GetConstant(Theme::Vector2Constants::MenuBar_Padding);
		SetSize(Vector2(0.0f, GetTheme()->GetFont()->GetSize()));
	}

	m_MenuItems.push_back(Item);
	Invalidate();

	return Item->CreateMenu();
}

void MenuBar::GetMenuItems(std::vector<std::shared_ptr<MenuItem>>& Items) const
{
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
	for (int I = 0; I < Menus.GetCount(); I++)
	{
		const Json& Item = Menus[I];

		std::shared_ptr<Menu> NewMenu = AddItem(Item["Text"].GetString());
		if (NewMenu)
		{
			NewMenu->OnLoad(Item);
		}
	}
}

void MenuBar::OnHover(MenuItem* Hovered)
{
	if (m_Open)
	{
		Open(Hovered);
	}
}

void MenuBar::OnSelected(MenuItem* Selected)
{
	Open(Selected);
}

void MenuBar::Open(MenuItem* Item)
{
	if (Item == nullptr)
	{
		return;
	}

	m_Menu = Item->GetMenu();
	if (!m_Menu)
	{
		return;
	}

	m_Menu->Close();
	const Vector2 Position = Item->GetPosition();
	m_Menu->SetPosition(Position + Vector2(0.0f, GetSize().Y));
	GetWindow()->SetPopup(m_Menu);
	m_Open = true;
	Invalidate();
}

}
