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

class ScrollableContainer;
class ScrollableViewControl;

/// @brief Interaction control that forwards input.
///
/// The ScrollableViewControl will always force this interaction to have focus.
/// This control forwards input to the ScrollableContainer ScrollBar controls when
/// appropriate, but prevent them from taking focus so that controls inside of
/// the container can continue to receive input.
class ScrollableViewInteraction : public Control
{
    CLASS(ScrollableViewInteraction)

public:
    ScrollableViewInteraction(Window* InWindow);

    ScrollableViewControl& ScrollableView() const;

    ScrollableViewInteraction& SetAlwaysFocus(bool AlwaysFocus);
    bool AlwaysFocus() const;

    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnMouseWheel(const Vector2& Delta) override;

private:
    bool m_AlwaysFocus { false };
};

/// @brief Container that manages a ScrollableContainer and user interactions.
///
/// ScrollableViewControl is a container that forwards the mouse input into the
/// contents of the ScrollableContainer. This container helps prevent the ScrollBar
/// controls of the ScrollableContainer from taking focus away from the controls inside
/// the ScrollableContainer.
class ScrollableViewControl : public Container
{
    CLASS(ScrollableViewControl)

public:
    ScrollableViewControl(Window* InWindow);

    ScrollableViewControl& SetInteraction(const std::shared_ptr<ScrollableViewInteraction>& Interaction);
    const std::shared_ptr<ScrollableViewInteraction>& Interaction() const;

    const std::shared_ptr<ScrollableContainer>& Scrollable() const;

    ScrollableViewControl& SetIgnoreOwnedControls(bool IgnoreOwnedControls);
    ScrollableViewControl& SetPendingFocus(const std::weak_ptr<Control>& PendingFocus);

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

    virtual Control& SetOnCreateContextMenu(Control::OnCreateContextMenuSignature&& Fn) override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;

protected:
    using Container::AddControl;
    using Container::InsertControl;
    using Container::RemoveControl;

    virtual void OnLayoutComplete() override;

private:
    bool AlwaysFocusInteraction() const;

    std::shared_ptr<ScrollableContainer> m_Scrollable { nullptr };
    std::shared_ptr<ScrollableViewInteraction> m_Interaction { nullptr };
    bool m_IgnoreOwnedControls { false };
    std::weak_ptr<Control> m_PendingFocus {};
};

}
