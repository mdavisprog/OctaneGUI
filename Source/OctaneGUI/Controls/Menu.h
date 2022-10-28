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

#include "VerticalContainer.h"

#include <unordered_map>

namespace OctaneGUI
{

class Panel;
class MenuItem;
class VerticalContainer;

/// @brief A container that contains a list of selectable items that are displayed in a pop-up box.
///
/// Menu containers are either a part of a MenuBar container or can be standalone to be opened as a
/// context menu. Menus can also contain sub-menus which is opened when the associated menu item is
/// hovered.
class Menu : public Container
{
    CLASS(Menu)

public:
    Menu(Window* InWindow);

    template <class T, typename... TArgs>
    std::shared_ptr<T> AddItem(TArgs... Args)
    {
        return m_Container->AddControl<T>(Args...);
    }

    Menu& AddItem(const char* InText, OnEmptySignature Fn = nullptr);
    std::shared_ptr<MenuItem> GetItem(const char* InText) const;
    Menu& AddSeparator();
    Menu& Close();
    Vector2 Margins() const;

    void GetMenuItems(std::vector<std::shared_ptr<MenuItem>>& Items) const;

    void Resize();

    virtual void GetControlList(ControlList& List) const override;

    virtual void OnLoad(const Json& Root) override;

private:
    typedef std::unordered_map<const MenuItem*, OnEmptySignature> MenuItemMap;

    using Container::AddControl;
    using Container::InsertControl;

    void OnHovered(const MenuItem& Item);
    void OnSelected(const MenuItem& Item);
    void SetSelected(const std::shared_ptr<Menu>& InMenu, bool Selected) const;

    std::shared_ptr<Panel> m_Panel { nullptr };
    std::shared_ptr<VerticalContainer> m_Container {};
    std::vector<std::shared_ptr<MenuItem>> m_Items {};
    MenuItemMap m_Callbacks {};
    std::shared_ptr<Menu> m_Menu { nullptr };
};

}
