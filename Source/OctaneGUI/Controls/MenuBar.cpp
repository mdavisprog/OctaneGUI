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

#include "MenuBar.h"
#include "../Font.h"
#include "../Json.h"
#include "../String.h"
#include "../Theme.h"
#include "../Window.h"
#include "ControlList.h"
#include "HorizontalContainer.h"
#include "Menu.h"
#include "MenuItem.h"
#include "Panel.h"
#include "TextSelectable.h"

namespace OctaneGUI
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
		.SetOnHovered([this](TextSelectable& Item) -> void
			{
				OnHover(static_cast<MenuItem&>(Item));
			})
		.SetOnPressed([this](TextSelectable& Item) -> void
			{
				OnSelected(static_cast<MenuItem&>(Item));
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

std::shared_ptr<Menu> MenuBar::Item(const char* Name) const
{
	for (const std::shared_ptr<MenuItem>& Item_ : m_MenuItems)
	{
		if (String::ToMultiByte(Item_->GetText()) == Name)
		{
			return Item_->GetMenu();
		}
	}

	return nullptr;
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

MenuBar& MenuBar::ClearMenuItems()
{
	m_Container->ClearControls();
	m_MenuItems.clear();
	SetSize({});
	Invalidate(InvalidateType::Both);
	return *this;
}

void MenuBar::Close()
{
	if (!m_Opened.expired())
	{
		Close(*m_Opened.lock().get());
		m_Opened.reset();
	}

	m_Open = false;
}

void MenuBar::GetControlList(ControlList& List) const
{
	std::vector<std::shared_ptr<MenuItem>> Items;
	GetMenuItems(Items);

	std::vector<std::shared_ptr<Control>> ControlItems {};
	ControlItems.insert(ControlItems.end(), Items.begin(), Items.end());
	List.AddControls(ControlItems);
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

void MenuBar::OnHover(MenuItem& Hovered)
{
	if (m_Open)
	{
		Open(Hovered);
	}
}

void MenuBar::OnSelected(MenuItem& Selected)
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

void MenuBar::Open(MenuItem& Item)
{
	if (!m_Opened.expired())
	{
		if (m_Opened.lock().get() == &Item)
		{
			return;
		}

		Close(*m_Opened.lock().get());
		m_Opened.reset();
	}

	if (!Item.GetMenu())
	{
		return;
	}

	const Vector2 Position = Item.GetAbsolutePosition();
	Item.GetMenu()->SetPosition(Position + Vector2(0.0f, GetSize().Y));
	Item.GetMenu()->Resize();
	GetWindow()->SetPopup(Item.GetMenu());
	Item.SetSelected(true);
	m_Open = true;

	// This needs to be called after SetPopup. The SetPopup function will attempt
	// to close any existing popups, which will close this MenuBar, which will close the
	// selected menu item's menu.
	m_Opened = WeakPtr(Item);

	Invalidate();
}

void MenuBar::Close(MenuItem& Item) const
{
	Item.SetSelected(false);

	if (Item.GetMenu())
	{
		Item.GetMenu()->Close();
	}
}

std::weak_ptr<MenuItem> MenuBar::WeakPtr(const MenuItem& Item) const
{
	for (const std::shared_ptr<MenuItem>& Item_ : m_MenuItems)
	{
		if (Item_.get() == &Item)
		{
			return Item_;
		}
	}

	return std::weak_ptr<MenuItem>();
}

}
