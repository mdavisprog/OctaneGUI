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

class Tab;

/// @brief Container that holds other containers where only one is visible.
///
/// Each container is represented with a labeled tab that is drawn above
/// the selected container.
class TabContainer : public Container
{
    CLASS(TabContainer)

public:
    TabContainer(Window* InWindow);

    std::shared_ptr<Container> AddTab(const char32_t* Label);

    TabContainer& SetShowAdd(bool ShowAdd);
    bool ShowAdd() const;

    virtual void OnLoad(const Json& Root) override;

private:
    using Container::AddControl;
    using Container::InsertControl;
    using Container::RemoveControl;

    std::shared_ptr<Tab> CreateTab(const char32_t* Label);
    TabContainer& SetTab(const std::shared_ptr<Container>& Contents);
    TabContainer& SetTabSelected(const std::shared_ptr<Container>& Contents, bool Selected);

    std::shared_ptr<Container> m_Contents { nullptr };
    std::shared_ptr<Container> m_Tabs { nullptr };
    std::shared_ptr<Container> m_AddTab { nullptr };
    std::weak_ptr<Container> m_Tab {};

    bool m_ShowAdd { false };
};

}
