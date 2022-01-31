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

#pragma once

#include "Container.h"

namespace OctaneUI
{

class MenuItem;

class Menu : public Container
{
	CLASS(Menu)

public:
	Menu(Window* InWindow);
	virtual ~Menu();

	Menu* AddItem(const char* InText, OnEmptySignature Fn = nullptr);
	std::shared_ptr<MenuItem> GetItem(const char* InText) const;
	Menu* AddSeparator();
	Menu* Close();

	void GetMenuItems(std::vector<std::shared_ptr<MenuItem>>& Items) const;

	virtual void OnLoad(const Json& Root) override;

private:
	typedef std::unordered_map<MenuItem*, OnEmptySignature> MenuItemMap;

	void Resize();
	void OnHovered(MenuItem* Item);
	void OnSelected(MenuItem* Item);
	void OnSeparatorHovered(Control* Item);
	void SetSelected(const std::shared_ptr<Menu>& InMenu, bool Selected) const;

	std::shared_ptr<Panel> m_Panel;
	std::shared_ptr<VerticalContainer> m_Container;
	std::vector<std::shared_ptr<MenuItem>> m_Items;
	MenuItemMap m_Callbacks;
	std::shared_ptr<Menu> m_Menu;
};

}
