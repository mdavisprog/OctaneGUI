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

#include "Control.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "../Theme.h"
#include "../Window.h"
#include "Container.h"

#include <cassert>

namespace OctaneGUI
{

Expand ToExpand(const std::string& Value)
{
    const std::string Lower = String::ToLower(Value);
    if (Lower == "width")
    {
        return Expand::Width;
    }
    else if (Lower == "height")
    {
        return Expand::Height;
    }
    else if (Lower == "both")
    {
        return Expand::Both;
    }

    return Expand::None;
}

const char* ToString(Expand Value)
{
    switch (Value)
    {
    case Expand::Width: return "Width";
    case Expand::Height: return "Height";
    case Expand::Both: return "Both";
    default: break;
    }

    return "None";
}

Control::Control(Window* InWindow)
    : Class()
    , std::enable_shared_from_this<Control>()
    , m_Window(InWindow)
{
}

Control::~Control()
{
}

Control* Control::SetPosition(const Vector2& Position)
{
    m_Bounds.SetPosition(Position);
    Invalidate(InvalidateType::Layout);
    return this;
}

Vector2 Control::GetPosition() const
{
    return m_Bounds.Min;
}

Vector2 Control::GetAbsolutePosition() const
{
    if (m_Parent == nullptr)
    {
        return GetPosition();
    }

    return m_Parent->GetAbsolutePosition() + GetPosition();
}

Control* Control::SetSize(const Vector2& Size)
{
    const Vector2 LastSize = GetSize();
    m_Bounds.Max = m_Bounds.Min + Size;
    if (LastSize != Size)
    {
        Invalidate(InvalidateType::Layout);
        OnResized();
    }
    return this;
}

Vector2 Control::GetSize() const
{
    return m_Bounds.GetSize();
}

Control* Control::SetParent(Control* Parent)
{
    m_Parent = Parent;
    return this;
}

Control* Control::GetParent() const
{
    return m_Parent;
}

Control* Control::SetExpand(Expand InExpand)
{
    if (IsFixedSize())
    {
        return this;
    }

    m_Expand = InExpand;
    return this;
}

Expand Control::GetExpand() const
{
    return m_Expand;
}

Control& Control::SetForwardKeyEvents(bool Forward)
{
    m_ForwardKeyEvents = Forward;
    return *this;
}

bool Control::ShouldForwardKeyEvents() const
{
    return m_ForwardKeyEvents;
}

Control* Control::SetID(const char* ID)
{
    m_ID = ID;
    return this;
}

const char* Control::GetID() const
{
    return m_ID.c_str();
}

std::string Control::GetFullID() const
{
    std::string Result;

    if (m_Parent != nullptr)
    {
        Result = m_Parent->GetFullID();
    }

    if (!Result.empty() && !m_ID.empty())
    {
        Result += ".";
    }

    Result += m_ID;

    return Result;
}

bool Control::HasID() const
{
    return !m_ID.empty();
}

bool Control::Contains(const Vector2& Position) const
{
    return GetAbsoluteBounds().Contains(Position);
}

Rect Control::GetBounds() const
{
    return m_Bounds;
}

Rect Control::GetAbsoluteBounds() const
{
    Rect Result = m_Bounds;
    Result.SetPosition(GetAbsolutePosition());
    return Result;
}

void Control::SetWindow(Window* InWindow)
{
    m_Window = InWindow;
}

Window* Control::GetWindow() const
{
    return m_Window;
}

Vector2 Control::GetMousePosition() const
{
    if (m_Window)
    {
        return m_Window->GetMousePosition();
    }

    return Vector2();
}

std::shared_ptr<Theme> Control::GetTheme() const
{
    return m_Window->GetTheme();
}

bool Control::IsKeyPressed(Keyboard::Key Key) const
{
    return m_Window->IsKeyPressed(Key);
}

Control* Control::SetOnInvalidate(OnInvalidateSignature Fn)
{
    m_OnInvalidate = Fn;
    return this;
}

void Control::Invalidate(InvalidateType Type)
{
    if (m_OnInvalidate)
    {
        m_OnInvalidate(Share(), Type);
    }
}

Control& Control::SetProperty(ThemeProperties::Property Property, const Variant& Value)
{
    if (Value.IsNull())
    {
        return *this;
    }

    assert(Property < ThemeProperties::Max);
    m_ThemeProperties[Property] = Value;

    return *this;
}

const Variant& Control::GetProperty(ThemeProperties::Property Property) const
{
    if (m_ThemeProperties.Has(Property))
    {
        return m_ThemeProperties[Property];
    }

    return GetTheme()->Get(Property);
}

void Control::ClearProperty(ThemeProperties::Property Property)
{
    m_ThemeProperties.Clear(Property);
}

std::shared_ptr<Control> Control::Share()
{
    return shared_from_this();
}

std::shared_ptr<Control const> Control::Share() const
{
    return shared_from_this();
}

void Control::OnPaint(Paint& Brush) const
{
}

void Control::Update()
{
}

void Control::OnFocused()
{
}

void Control::OnUnfocused()
{
}

void Control::OnLoad(const Json& Root)
{
    m_ID = Root["ID"].String();
    SetSize(Vector2::FromJson(Root["Size"], GetSize()));
    
    if (!IsFixedSize())
    {
        m_Expand = ToExpand(Root["Expand"].String());
    }
}

void Control::OnSave(Json& Root) const
{
    Root["ID"] = m_ID;
    Root["Expand"] = ToString(m_Expand);
    Root["Position"] = std::move(Vector2::ToJson(GetPosition()));
    Root["Size"] = std::move(Vector2::ToJson(GetSize()));
    Root["Window"] = m_Window->GetTitle();
}

bool Control::OnKeyPressed(Keyboard::Key Key)
{
    if (m_ForwardKeyEvents && m_Parent != nullptr)
    {
        return m_Parent->OnKeyPressed(Key);
    }

    return false;
}

void Control::OnKeyReleased(Keyboard::Key Key)
{
    if (m_ForwardKeyEvents && m_Parent != nullptr)
    {
        m_Parent->OnKeyReleased(Key);
    }
}

void Control::OnMouseMove(const Vector2& Position)
{
}

bool Control::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    return false;
}

void Control::OnMouseReleased(const Vector2& Position, Mouse::Button Button)
{
}

void Control::OnMouseWheel(const Vector2& Delta)
{
    if (m_Parent != nullptr)
    {
        m_Parent->OnMouseWheel(Delta);
    }
}

void Control::OnMouseEnter()
{
}

void Control::OnMouseLeave()
{
}

void Control::OnResized()
{
}

void Control::OnText(uint32_t Code)
{
}

void Control::OnThemeLoaded()
{
}

void Control::Invalidate(std::shared_ptr<Control> Focus, InvalidateType Type) const
{
    if (!Focus)
    {
        return;
    }

    if (m_OnInvalidate)
    {
        m_OnInvalidate(Focus, Type);
    }
}

bool Control::IsFixedSize() const
{
    return false;
}

Control::Control()
{
}

}
