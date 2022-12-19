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

namespace OctaneGUI
{

class ScrollBar;
class Timer;

/// @brief Container that allows for scrolling.
///
/// ScrollableContainer allows for scrolling the contents of this container
/// if the size of the contents exceed the size of the container.
class ScrollableContainer : public Container
{
    CLASS(ScrollableContainer)

public:
    typedef std::function<void(const Vector2&)> OnScrollSignature;

    ScrollableContainer(Window* InWindow);

    bool IsInScrollBar(const Vector2& Point) const;
    bool IsScrollBarVisible(const std::shared_ptr<Control>& Item) const;
    bool IsScrolling() const;
    Vector2 ContentSize() const;
    Vector2 Overflow() const;
    Vector2 Offset() const;

    const std::shared_ptr<ScrollBar>& HorizontalScrollBar() const;
    const std::shared_ptr<ScrollBar>& VerticalScrollBar() const;

    ScrollableContainer& SetHorizontalSBEnabled(bool Enabled);
    ScrollableContainer& SetVerticalSBEnabled(bool Enabled);
    ScrollableContainer& SetOffset(const Vector2& Offset);
    ScrollableContainer& AddOffset(const Vector2& Delta);
    ScrollableContainer& ScrollIntoView(const std::shared_ptr<Control>& Item);

    Vector2 GetScrollableSize() const;
    float ScrollBarPropertySize() const;

    ScrollableContainer& SetScrollSpeed(const Vector2& ScrollSpeed);
    Vector2 ScrollSpeed() const;

    ScrollableContainer& SetOnScroll(OnScrollSignature&& Fn);

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

    virtual void Update() override;
    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnMouseWheel(const Vector2& Delta) override;
    virtual void OnResized() override;
    virtual void OnThemeLoaded() override;

private:
    Rect TranslatedBounds() const;
    Vector2 GetContentSize(const std::vector<std::shared_ptr<Control>>& Controls) const;
    Vector2 GetOverflow() const;
    void SetOffset(const Vector2& Offset, bool UpdateSBHandles);
    void UpdateScrollBarSizes();
    void UpdateScrollBarPositions();
    bool IsScrollBarControl(Control* Focus) const;

    std::shared_ptr<ScrollBar> m_HorizontalSB { nullptr };
    std::shared_ptr<ScrollBar> m_VerticalSB { nullptr };
    Vector2 m_ContentSize {};

    Vector2 m_ScrollOffset {};
    Vector2 m_ScrollSpeed { 10.0f, 10.0f };
    std::shared_ptr<Timer> m_ScrollTimer { nullptr };
    bool m_InitialScroll { true };

    OnScrollSignature m_OnScroll { nullptr };
};

}
