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

#include "Keyboard.h"
#include "Mouse.h"
#include "Popup.h"
#include "Rect.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace OctaneUI
{

class Application;
class Container;
class Control;
class Icons;
class MenuBar;
class Paint;
class Theme;
class VertexBuffer;

class Window
{
public:
	typedef std::function<void(Window*, const std::vector<VertexBuffer>&)> OnPaintSignature;

	Window(Application* InApplication);
	virtual ~Window();

	void SetTitle(const char* Title);
	const char* GetTitle() const;

	void SetSize(float Width, float Height);
	void SetSize(Vector2 Size);
	Vector2 GetSize() const;

	void SetPopup(const std::shared_ptr<Container>& Popup, bool Modal = false);

	void OnKeyPressed(Keyboard::Key Key);
	void OnKeyReleased(Keyboard::Key Key);
	void OnMouseMove(const Vector2& Position);
	void OnMousePressed(const Vector2& Position, Mouse::Button Button);
	void OnMouseReleased(const Vector2& Position, Mouse::Button Button);
	void OnText(uint32_t Code);

	void CreateContainer();
	std::shared_ptr<Container> GetContainer() const;
	std::shared_ptr<MenuBar> GetMenuBar() const;
	std::shared_ptr<Theme> GetTheme() const;
	std::shared_ptr<Icons> GetIcons() const;
	Vector2 GetMousePosition() const;

	void Update();
	void DoPaint(Paint& Brush);

	void SetOnPaint(OnPaintSignature Fn);

private:
	Window();

	Application* m_Application;
	std::string m_Title;
	Vector2 m_MousePosition;
	Rect m_Bounds;
	std::shared_ptr<Container> m_Container;
	std::shared_ptr<MenuBar> m_MenuBar;
	std::shared_ptr<Container> m_Body;
	bool m_Repaint;
	std::weak_ptr<Control> m_Focus;
	std::weak_ptr<Control> m_Hovered;
	Popup m_Popup;

	OnPaintSignature m_OnPaint;
};

}
