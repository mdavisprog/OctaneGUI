/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

namespace OctaneGUI
{

class HorizontalContainer;
class Menu;
class MenuItem;
class Panel;

/// @brief A container of text items displayed below the window's title bar.
///
/// MenuBar contains a list of selectable text items that will open a menu
/// when clicked.
class MenuBar : public Container
{
    CLASS(MenuBar)

public:
    MenuBar(Window* InWindow);

    std::shared_ptr<Menu> AddItem(const char* InText);
    std::shared_ptr<Menu> Item(const char* Name) const;
    void GetMenuItems(std::vector<std::shared_ptr<MenuItem>>& Items) const;
    MenuBar& ClearMenuItems();
    void Close();

    virtual void GetControlList(ControlList& List) const override;

    virtual void OnLoad(const Json& Root) override;
    virtual void OnThemeLoaded() override;

private:
    void OnHover(MenuItem& Hovered);
    void OnSelected(MenuItem& Selected);
    void Open(MenuItem& Item);
    void Close(MenuItem& Item) const;

    std::weak_ptr<MenuItem> WeakPtr(const MenuItem& Item) const;

    std::shared_ptr<Panel> m_Panel { nullptr };
    std::shared_ptr<HorizontalContainer> m_Container { nullptr };
    std::vector<std::shared_ptr<MenuItem>> m_MenuItems;
    bool m_Open { false };
    std::weak_ptr<MenuItem> m_Opened {};
};

}
