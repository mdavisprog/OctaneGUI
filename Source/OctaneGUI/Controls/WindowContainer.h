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

#include "VerticalContainer.h"

namespace OctaneGUI
{

class MenuBar;
class TitleBar;

/// @brief Root container for Windows which consist of a title bar, menu bar, and body.
///
/// This container contains the controls and logic for handling a title bar,
/// menu bar, and a body. The title bar will only be present if a custom title
/// bar is requested for the owning widnow. Each window will hold an instance
/// of this container and is the container in which interactions will occur.
class WindowContainer : public VerticalContainer
{
    CLASS(WindowContainer)

public:
    WindowContainer(Window* InWindow);

    WindowContainer& Clear();
    WindowContainer& CloseMenuBar();

    WindowContainer& ShowTitleBar(bool Show);
    WindowContainer& SetTitle(const char32_t* Title);
    WindowContainer& SetMaximized(bool Maximized);

    /// @brief Notified by the owning Window if the focus state has changed.
    ///
    /// This function will update the title bar's color based on the theme.
    ///
    /// @param Focused True if the owning Window has focus. False otherwise.
    /// @return The reference to this WindowContainer.
    WindowContainer& SetFocused(bool Focused);

    bool IsInTitleBar(const Vector2& Position) const;

    const std::shared_ptr<Container>& Body() const;
    const std::shared_ptr<MenuBar>& GetMenuBar() const;

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

    virtual void OnLoad(const Json& Root) override;

private:
    std::shared_ptr<TitleBar> m_TitleBar { nullptr };
    std::shared_ptr<MenuBar> m_MenuBar { nullptr };
    std::shared_ptr<Container> m_Body { nullptr };
};

}
