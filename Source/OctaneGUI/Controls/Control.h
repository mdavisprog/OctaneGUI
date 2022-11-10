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

#include "../CallbackDefs.h"
#include "../Class.h"
#include "../Keyboard.h"
#include "../Mouse.h"
#include "../Rect.h"
#include "../ThemeProperties.h"

#include <functional>
#include <memory>
#include <string>

namespace OctaneGUI
{

class Json;
class Paint;
class Theme;
class Window;

enum class Expand : uint8_t
{
    None,
    Width,
    Height,
    Both
};

enum class HorizontalAlignment : uint8_t
{
    Left,
    Center,
    Right
};

enum class VerticalAlignment : uint8_t
{
    Top,
    Center,
    Bottom
};

enum class Orientation : uint8_t
{
    Horizontal,
    Vertical
};

static const char* ToString(Orientation Type)
{
    if (Type == Orientation::Vertical)
    {
        return "Vertical";
    }

    return "Horizontal";
}

static Orientation ToOrientation(const std::string& Type)
{
    if (Type == "Vertical")
    {
        return Orientation::Vertical;
    }

    return Orientation::Horizontal;
}

static const char* ToString(HorizontalAlignment Type)
{
    switch (Type)
    {
    case HorizontalAlignment::Center: return "Center";
    case HorizontalAlignment::Right: return "Right";
    case HorizontalAlignment::Left:
    default: break;
    }

    return "Left";
}

/// @brief Base class for all controls in the library.
///
/// Controls are the base objects that make up a GUI element. All controls must be
/// a part of a Container in order to be positioned, updated, and painted.
///
/// This class contains a number of virtual functions that are invoked whenever the
/// state of the GUI changes. All classes that inherit from Control can override
/// the desired functions to implement the desired behavior.
class Control
    : public Class
    , public std::enable_shared_from_this<Control>
{
    CLASS(Control)

public:
    Control(Window* InWindow);
    virtual ~Control();

    /// @brief Sets this control's relative position.
    ///
    /// All control's positioning is relative to it's parent. When the position is
    /// set, the control's layout is invalidated, notifying the owning container
    /// to update it's layout with this new position for this control.
    ///
    /// @param Position Relative position of this control.
    /// @return This Control reference.
    Control& SetPosition(const Vector2& Position);

    /// @brief Returns the relative position of this control without any transformations.
    /// @return Vector2 relative position.
    Vector2 GetPosition() const;

    /// @brief Returns the absolute position of this control. Applies parent transformations.
    /// @return Vector2 absolute position.
    Vector2 GetAbsolutePosition() const;

    /// @brief Sets the size of this control.
    ///
    /// If the size is different, a layout invalidate request is sent and the owning container
    /// will re-layout its contents with this new size for this control. The OnResized function
    /// will be called on this control before the layout occurs.
    ///
    /// @param Size Vector2 of the new size for this control.
    /// @return This Control Reference.
    Control& SetSize(const Vector2& Size);

    /// @brief Returns the size of this control.
    /// @return Vector2 of this control's size.
    Vector2 GetSize() const;

    /// @brief Sets the parent for this control.
    ///
    /// The parent is usually a Container by in some cases may be another control.
    ///
    /// @param Parent The parent control for this control. Can be NULL.
    /// @return This Control reference.
    Control& SetParent(Control* Parent);

    /// @brief Returns the parent for this control.
    /// @return Pointer to the parent control. May be NULL.
    Control* GetParent() const;

    /// @brief Sets the Expand type for this control.
    ///
    /// The Expand type tells the owning Container how this control should be sized during
    /// any layouts. Control's by default do not expand in any direction. This value can
    /// either be None, Width, Height, or Both. If the control is set to be a fixed size,
    /// then no change is made when this function is called.
    ///
    /// @param InExpand The Expand type to set for this control.
    /// @return This Control reference.
    Control& SetExpand(Expand InExpand);

    /// @brief Returns this control's expand type.
    /// @return Expand type.
    Expand GetExpand() const;

    /// @brief Sets whether the parent control should be notified of this control's key events.
    ///
    /// Containers may want to handle key events but due to the architecture only allowing
    /// Controls to have focus, Containers will not be able to directly receive these events.
    /// Setting this flag will allow for this to occur.
    ///
    /// @param Forward Boolean to set whether key events should be forwarded.
    /// @return This Control reference.
    Control& SetForwardKeyEvents(bool Forward);

    /// @brief Returns whether this control should forward key events.
    /// @return Boolean value.
    bool ShouldForwardKeyEvents() const;

    /// @brief Sets the ID for this control.
    ///
    /// Giving a control an ID uniquely identifies this control. The full ID
    /// path to this control takes into account the parent ID. For example, if
    /// the parent control has an ID of 'Parent' and this control has an ID of
    /// 'Child', the full ID would be 'Parent.Child'.
    ///
    /// The main usage for giving control's an ID is to retrieve a reference to
    /// this control whenever an API that accepts a ControlList object is called.
    /// This object will retrieve all controls that have a specified ID and add them
    /// for retrieval by the caller. This allows for altering any additional settings
    /// and registering callbacks.
    ///
    /// @param ID String defining the unique identifier for this control.
    /// @return This Control reference.
    Control& SetID(const char* ID);

    /// @brief Retrieves the ID for this control.
    /// @return String pointer to the control's ID.
    const char* GetID() const;

    /// @brief Retrieves the fully qualified ID for this control. This will include
    /// all parent control's ID.
    /// @return String object with the full ID.
    std::string GetFullID() const;

    /// @brief Determines if this control has a valid ID.
    /// @return True if the control has an ID. False otherwise.
    bool HasID() const;

    /// @brief Determines if the given point is contained within the bounds of this control.
    /// @param Position The point to query if it is within the bounds.
    /// @return True if the point is in bounds. False otherwise.
    bool Contains(const Vector2& Position) const;

    /// @brief Retrieves the local bounds for this control. No transformations are applied.
    /// @return The Rect that defines the local bounds for this control.
    Rect GetBounds() const;

    /// @brief Retrieves the absolute bounds for this control. All parent transformations
    /// are applied to this control's local bounds.
    /// @return The Rect that defines the absolute bounds for this control.
    Rect GetAbsoluteBounds() const;

    /// @brief Sets the owning window for this control.
    ///
    /// This function is defined to be virtual as some controls may need to perform
    /// additional logic when the owning window has changed.
    ///
    /// @param InWindow The new owning Window object for this control.
    virtual void SetWindow(Window* InWindow);

    /// @brief Retrieves the owning window for this control.
    /// @return Pointer to the Window object for this control.
    Window* GetWindow() const;

    /// @brief Returns the render scale to use. This is usually (1, 1) but may be larger
    /// on platforms with high DPI displays such as the Mac's Retina display.
    /// @return The Vector2 scale for rendering.
    Vector2 RenderScale() const;

    /// @brief Returns the current mouse position within the owning Window.
    /// @return The Vector2 mouse position.
    Vector2 GetMousePosition() const;

    /// @brief Returns the currently active application Theme object.
    /// @return Shared pointer to the Theme object.
    std::shared_ptr<Theme> GetTheme() const;

    /// @brief Determines if the given key is currently pressed.
    /// @param Key The key to check if it is pressed.
    /// @return True if the given key is pressed. False otherwise.
    bool IsKeyPressed(Keyboard::Key Key) const;

    /// @brief Sets the callback for when this control is invalidated.
    ///
    /// The callback is invoked whenever a request for invalidation of
    /// this control occurs. The callback accepts the control and the
    /// type of invalidation requested.
    ///
    /// @param Fn The callback to invoke.
    /// @return This Control reference.
    Control& SetOnInvalidate(OnInvalidateSignature Fn);

    /// @brief Request this control to be invalidated.
    /// @param Type Can either be a Paint or Layout request.
    /// @return This Control reference.
    Control& Invalidate(InvalidateType Type = InvalidateType::Paint);

    Control& SetProperty(ThemeProperties::Property Property, const Variant& Value);
    const Variant& GetProperty(ThemeProperties::Property Property) const;
    void ClearProperty(ThemeProperties::Property Property);

    std::shared_ptr<Control> Share();
    std::shared_ptr<Control const> Share() const;

    template <class T>
    std::shared_ptr<T> TShare()
    {
        return std::dynamic_pointer_cast<T>(Share());
    }

    template <class T>
    std::shared_ptr<T const> TShare() const
    {
        return std::dynamic_pointer_cast<T const>(Share());
    }

    virtual void OnPaint(Paint& Brush) const;
    virtual void Update();
    virtual void OnFocused();
    virtual void OnUnfocused();
    virtual void OnLoad(const Json& Root);
    virtual void OnSave(Json& Root) const;
    virtual bool OnKeyPressed(Keyboard::Key Key);
    virtual void OnKeyReleased(Keyboard::Key Key);
    virtual void OnMouseMove(const Vector2& Position);
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count);
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button);
    virtual void OnMouseWheel(const Vector2& Delta);
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnResized();
    virtual void OnText(uint32_t Code);
    virtual void OnThemeLoaded();

protected:
    void Invalidate(std::shared_ptr<Control> Focus, InvalidateType Type) const;

    virtual bool IsFixedSize() const;

private:
    Control();

    Window* m_Window { nullptr };
    Control* m_Parent { nullptr };
    Rect m_Bounds {};
    Expand m_Expand { Expand::None };
    std::string m_ID {};
    OnInvalidateSignature m_OnInvalidate { nullptr };
    ThemeProperties m_ThemeProperties {};

    bool m_ForwardKeyEvents { false };
};

}
