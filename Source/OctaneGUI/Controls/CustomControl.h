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

#include "Control.h"

namespace OctaneGUI
{

/// @brief Control class that provides an API for setting callbacks for all control events.
///
/// This class allows for creating custom controls without needing to create a sub-class
/// of control.
class CustomControl : public Control
{
    CLASS(CustomControl)

public:
    typedef std::function<void(const CustomControl&, Paint&)> OnPaintSignature;
    typedef std::function<void(CustomControl&)> OnCustomControlSignature;
    typedef std::function<void(CustomControl&, const Json&)> OnLoadSignature;
    typedef std::function<void(const CustomControl&, Json&)> OnSaveSignature;
    typedef std::function<bool(CustomControl&, Keyboard::Key)> OnKeyPressedSignature;
    typedef std::function<void(CustomControl&, Keyboard::Key)> OnKeyReleasedSignature;
    typedef std::function<void(CustomControl&, const Vector2&)> OnMouseMoveSignature;
    typedef std::function<bool(CustomControl&, const Vector2&, Mouse::Button, Mouse::Count)> OnMousePressedSignature;
    typedef std::function<void(CustomControl&, const Vector2&, Mouse::Button)> OnMouseReleasedSignature;
    typedef std::function<void(CustomControl&, const Vector2&)> OnMouseWheelSignature;
    typedef std::function<void(CustomControl&, uint32_t)> OnTextSignature;

    CustomControl(Window* InWindow);

    CustomControl& SetOnPaint(OnPaintSignature&& Fn);
    CustomControl& SetOnUpdate(OnCustomControlSignature&& Fn);
    CustomControl& SetOnFocused(OnCustomControlSignature&& Fn);
    CustomControl& SetOnUnfocused(OnCustomControlSignature&& Fn);
    CustomControl& SetOnLoad(OnLoadSignature&& Fn);
    CustomControl& SetOnSave(OnSaveSignature&& Fn);
    CustomControl& SetOnKeyPressed(OnKeyPressedSignature&& Fn);
    CustomControl& SetOnKeyReleased(OnKeyReleasedSignature&& Fn);
    CustomControl& SetOnMouseMove(OnMouseMoveSignature&& Fn);
    CustomControl& SetOnMousePressed(OnMousePressedSignature&& Fn);
    CustomControl& SetOnMouseReleased(OnMouseReleasedSignature&& Fn);
    CustomControl& SetOnMouseWheel(OnMouseWheelSignature&& Fn);
    CustomControl& SetOnMouseEnter(OnCustomControlSignature&& Fn);
    CustomControl& SetOnMouseLeave(OnCustomControlSignature&& Fn);
    CustomControl& SetOnResized(OnCustomControlSignature&& Fn);
    CustomControl& SetOnText(OnTextSignature&& Fn);
    CustomControl& SetOnThemeLoaded(OnCustomControlSignature&& Fn);

    virtual void OnPaint(Paint& Brush) const override;
    virtual void Update() override;
    virtual void OnFocused() override;
    virtual void OnUnfocused() override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;
    virtual bool OnKeyPressed(Keyboard::Key Key) override;
    virtual void OnKeyReleased(Keyboard::Key Key) override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnMouseWheel(const Vector2& Delta) override;
    virtual void OnMouseEnter() override;
    virtual void OnMouseLeave() override;
    virtual void OnResized() override;
    virtual void OnText(uint32_t Code) override;
    virtual void OnThemeLoaded() override;

private:
    OnPaintSignature m_OnPaint { nullptr };
    OnCustomControlSignature m_OnUpdate { nullptr };
    OnCustomControlSignature m_OnFocused { nullptr };
    OnCustomControlSignature m_OnUnfocused { nullptr };
    OnLoadSignature m_OnLoad { nullptr };
    OnSaveSignature m_OnSave { nullptr };
    OnKeyPressedSignature m_OnKeyPressed { nullptr };
    OnKeyReleasedSignature m_OnKeyReleased { nullptr };
    OnMouseMoveSignature m_OnMouseMove { nullptr };
    OnMousePressedSignature m_OnMousePressed { nullptr };
    OnMouseReleasedSignature m_OnMouseReleased { nullptr };
    OnMouseWheelSignature m_OnMouseWheel { nullptr };
    OnCustomControlSignature m_OnMouseEnter { nullptr };
    OnCustomControlSignature m_OnMouseLeave { nullptr };
    OnCustomControlSignature m_OnResized { nullptr };
    OnTextSignature m_OnText { nullptr };
    OnCustomControlSignature m_OnThemeLoaded { nullptr };
};

}
