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

class Control : public Class
{
	CLASS(Control)

public:
	Control(Window* InWindow);
	virtual ~Control();

	Control* SetPosition(const Vector2& Position);
	Vector2 GetPosition() const;
	Vector2 GetAbsolutePosition() const;

	Control* SetSize(const Vector2& Size);
	Vector2 GetSize() const;

	Control* SetParent(Control* Parent);
	Control* GetParent() const;

	Control* SetExpand(Expand InExpand);
	Expand GetExpand() const;

	Control& SetForwardKeyEvents(bool Forward);
	bool ShouldForwardKeyEvents() const;

	Control* SetID(const char* ID);
	const char* GetID() const;
	std::string GetFullID() const;
	bool HasID() const;

	bool Contains(const Vector2& Position) const;
	Rect GetBounds() const;
	Rect GetAbsoluteBounds() const;
	Window* GetWindow() const;
	Vector2 GetMousePosition() const;
	std::shared_ptr<Theme> GetTheme() const;
	bool IsKeyPressed(Keyboard::Key Key) const;

	Control* SetOnInvalidate(OnInvalidateSignature Fn);
	void Invalidate(InvalidateType Type = InvalidateType::Paint);

	Control& SetProperty(ThemeProperties::Property Property, const Variant& Value);
	const Variant& GetProperty(ThemeProperties::Property Property) const;
	void ClearProperty(ThemeProperties::Property Property);

	virtual void OnPaint(Paint& Brush) const;
	virtual void Update();
	virtual void OnFocused();
	virtual void OnUnfocused();
	virtual void OnLoad(const Json& Root);
	virtual void OnSave(Json& Root) const;
	virtual bool OnKeyPressed(Keyboard::Key Key);
	virtual void OnKeyReleased(Keyboard::Key Key);
	virtual void OnMouseMove(const Vector2& Position);
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button);
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button);
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnText(uint32_t Code);
	virtual void OnThemeLoaded();

protected:
	void Invalidate(Control* Focus, InvalidateType Type) const;

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
