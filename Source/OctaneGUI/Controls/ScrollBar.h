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

#include "../Orientation.h"
#include "Container.h"

namespace OctaneGUI
{

class BoxContainer;
class ImageButton;
class ScrollBar;

class ScrollBarHandle : public Control
{
    CLASS(ScrollBarHandle)

public:
    ScrollBarHandle(Window* InWindow, ScrollBar* InScrollBar, Orientation InOrientation);

    Orientation GetOrientation() const;

    ScrollBarHandle& SetHandleSize(float HandleSize);
    float HandleSize() const;
    float PropertySize() const;
    float GetAvailableScrollSize() const;

    ScrollBarHandle& SetOnDrag(OnScrollBarSignature Fn);

    ScrollBarHandle& SetOffset(float Offset);
    float Offset() const;
    float OffsetPct() const;
    bool HasHandle() const;

    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnSave(Json& Root) const override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnMouseLeave() override;

private:
    Rect HandleBounds() const;
    void ClampOffset();

    Orientation m_Orientation { Orientation::Horizontal };
    float m_HandleSize { 0.0f };
    float m_Offset { 0.0f };
    bool m_Hovered { false };
    bool m_Drag { false };
    Vector2 m_DragOffset {};
    ScrollBar* m_ScrollBar { nullptr };
    OnScrollBarSignature m_OnDrag { nullptr };
};

class ScrollBar : public Container
{
    CLASS(ScrollBar)

public:
    ScrollBar(Window* InWindow, Orientation InOrientation);

    const std::shared_ptr<ScrollBarHandle>& Handle() const;

    ScrollBar& SetScrollBarSize(const Vector2& Size);
    Vector2 GetScrollBarSize() const;

    float PropertySize() const;
    Orientation GetOrientation() const;

    ScrollBar& SetAlwaysPaint(bool AlwaysPaint);
    bool ShouldPaint() const;

    ScrollBar& SetEnabled(bool Enabled);

    ScrollBar& SetOnScrollMin(OnScrollBarSignature&& Fn);
    ScrollBar& SetOnScrollMax(OnScrollBarSignature&& Fn);
    ScrollBar& SetOnRelease(OnScrollBarSignature&& Fn);

    virtual void Update() override;
    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnSave(Json& Root) const override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnThemeLoaded() override;

private:
    Rect HandleBackgroundBounds() const;
    void UpdateButtons();

    std::shared_ptr<ImageButton> m_MinButton { nullptr };
    std::shared_ptr<ImageButton> m_MaxButton { nullptr };
    std::shared_ptr<ScrollBarHandle> m_Handle { nullptr };

    bool m_AlwaysPaint { false };
    bool m_Buttons { false };
    bool m_Enabled { true };

    OnScrollBarSignature m_OnScrollMin { nullptr };
    OnScrollBarSignature m_OnScrollMax { nullptr };
    OnScrollBarSignature m_OnRelease { nullptr };
};

}
