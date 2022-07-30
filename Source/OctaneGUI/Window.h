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

#include "Clock.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Popup.h"
#include "Rect.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Application;
class Container;
class Control;
class ControlList;
class Icons;
class Json;
class MenuBar;
class Paint;
class TextureCache;
class Theme;
class Timer;
class VertexBuffer;
class WindowContainer;

class Window
{
public:
	typedef std::function<void(Window*, const VertexBuffer&)> OnPaintSignature;
	typedef std::function<void(Window&, const char*)> OnSetTitleSignature;
	typedef std::function<void(Window&)> OnWindowSignature;

	Window(Application* InApplication);
	virtual ~Window();

	void SetTitle(const char* Title);
	const char* GetTitle() const;

	void SetSize(Vector2 Size);
	Vector2 GetSize() const;

	void SetID(const char* ID);
	const char* ID() const;
	bool HasID() const;

	Window& SetVisible(bool Visible);
	bool IsVisible() const;

	Window& RequestClose(bool Request = true);
	bool ShouldClose() const;
	void Close();

	Window& SetResizable(bool Resizable);
	bool IsResizable() const;

	Application& App() const;

	void SetPopup(const std::shared_ptr<Container>& Popup, OnContainerSignature Callback = nullptr, bool Modal = false);
	void ClosePopup();
	const std::shared_ptr<Container>& GetPopup() const;
	bool IsPopupOpen() const;

	void OnKeyPressed(Keyboard::Key Key);
	void OnKeyReleased(Keyboard::Key Key);
	void OnMouseMove(const Vector2& Position);
	void OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count = Mouse::Count::Single);
	void OnMouseReleased(const Vector2& Position, Mouse::Button Button);
	void OnMouseWheel(const Vector2& Delta);
	void OnMouseEnter();
	void OnMouseLeave();
	void OnText(uint32_t Code);
	void ThemeLoaded();

	void CreateContainer();

	std::shared_ptr<Container> GetContainer() const;
	std::shared_ptr<MenuBar> GetMenuBar() const;
	std::shared_ptr<Container> GetRootContainer() const;
	std::shared_ptr<Theme> GetTheme() const;
	std::shared_ptr<Icons> GetIcons() const;
	TextureCache& GetTextureCache() const;
	Vector2 GetMousePosition() const;
	bool IsKeyPressed(Keyboard::Key Key) const;
	Window& SetMouseCursor(Mouse::Cursor Cursor);

	void Update();
	void DoPaint(Paint& Brush);

	void Load(const char* JsonStream);
	void Load(const char* JsonStream, ControlList& List);
	void Load(const Json& Root);
	void Load(const Json& Root, ControlList& List);
	void LoadRoot(const Json& Root);
	void LoadContents(const Json& Root);
	void LoadContents(const Json& Root, ControlList& List);
	void Clear();

	std::shared_ptr<Timer> CreateTimer(int Interval, bool Repeat, OnEmptySignature&& Callback);
	void StartTimer(const std::shared_ptr<Timer>& Object);
	bool ClearTimer(const std::shared_ptr<Timer>& Object);

	Window& SetOnPaint(OnPaintSignature&& Fn);
	Window& SetOnSetTitle(OnSetTitleSignature&& Fn);
	Window& SetOnClose(OnWindowSignature&& Fn);
	Window& SetOnLayout(OnWindowSignature&& Fn);

private:
	struct TimerHandle
	{
	public:
		TimerHandle(const std::weak_ptr<Timer>& InObject)
			: Object(InObject)
		{
		}

		std::weak_ptr<Timer> Object;
		Clock Elapsed {};
	};

	Window();

	void Populate(ControlList& List) const;
	void RequestLayout(std::shared_ptr<Container> Request);
	void UpdateTimers();
	void UpdateFocus(const std::shared_ptr<Control>& Focus);

	Application* m_Application { nullptr };
	std::string m_Title {};
	std::string m_ID {};
	Vector2 m_MousePosition {};
	Rect m_Bounds {};
	std::shared_ptr<WindowContainer> m_Container { nullptr };
	bool m_Repaint { false };
	std::weak_ptr<Control> m_Focus {};
	std::weak_ptr<Control> m_Hovered {};
	Popup m_Popup {};
	bool m_Visible { false };
	bool m_RequestClose { false };
	bool m_Resizable { true };
	std::vector<std::weak_ptr<Container>> m_LayoutRequests;

	std::vector<TimerHandle> m_Timers {};

	OnPaintSignature m_OnPaint { nullptr };
	OnContainerSignature m_OnPopupClose { nullptr };
	OnSetTitleSignature m_OnSetTitle { nullptr };
	OnWindowSignature m_OnClose { nullptr };
	OnWindowSignature m_OnLayout { nullptr };
};

}
