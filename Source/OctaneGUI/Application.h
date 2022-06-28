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

#include "CallbackDefs.h"
#include "Keyboard.h"
#include "TextureCache.h"
#include "Vector2.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace OctaneGUI
{

class ControlList;
class Event;
class Icons;
class Json;
class Theme;
class VertexBuffer;
class Window;

class Application
{
public:
	typedef std::function<void(Window*)> OnWindowSignature;
	typedef std::function<void(Window*, const VertexBuffer&)> OnWindowPaintSignature;
	typedef std::function<Event(Window*)> OnWindowEventSignature;
	typedef std::function<uint32_t(const std::vector<uint8_t>&, uint32_t, uint32_t)> OnLoadTextureSignature;
	typedef std::function<std::u32string(void)> OnGetClipboardContentsSignature;
	typedef std::function<void(Window*, const char*)> OnSetWindowTitleSignature;

	Application();
	virtual ~Application();

	bool Initialize(const char* JsonStream, std::unordered_map<std::string, ControlList>& WindowControls);
	void Shutdown();
	void Update();
	int Run();
	void Quit();

	std::shared_ptr<Window> GetMainWindow() const;
	std::shared_ptr<Window> GetWindow(const char* ID) const;
	bool IsMainWindow(Window* InWindow) const;
	std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream);
	std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream, ControlList& List);
	bool DisplayWindow(const char* ID) const;
	void CloseWindow(const char* ID);
	std::shared_ptr<Theme> GetTheme() const;
	std::shared_ptr<Icons> GetIcons() const;
	TextureCache& GetTextureCache();
	bool IsKeyPressed(Keyboard::Key Key) const;
	std::u32string ClipboardContents() const;

	Application& SetOnCreateWindow(OnWindowSignature&& Fn);
	Application& SetOnDestroyWindow(OnWindowSignature&& Fn);
	Application& SetOnPaint(OnWindowPaintSignature&& Fn);
	Application& SetOnEvent(OnWindowEventSignature&& Fn);
	Application& SetOnLoadTexture(OnLoadTextureSignature&& Fn);
	Application& SetOnExit(OnEmptySignature&& Fn);
	Application& SetOnGetClipboardContents(OnGetClipboardContentsSignature&& Fn);
	Application& SetOnSetWindowTitle(OnSetWindowTitleSignature&& Fn);

private:
	void OnPaint(Window* InWindow, const VertexBuffer& Buffer);
	std::shared_ptr<Window> CreateWindow(const char* ID);
	void DestroyWindow(const std::shared_ptr<Window>& Item);
	int ProcessEvent(const std::shared_ptr<Window>& Item);
	bool Initialize();

	std::unordered_map<std::string, std::shared_ptr<Window>> m_Windows;
	std::shared_ptr<Theme> m_Theme { nullptr };
	std::shared_ptr<Icons> m_Icons { nullptr };
	bool m_IsRunning { false };
	std::vector<Keyboard::Key> m_PressedKeys {};
	TextureCache m_TextureCache {};

	OnWindowSignature m_OnCreateWindow { nullptr };
	OnWindowSignature m_OnDestroyWindow { nullptr };
	OnWindowPaintSignature m_OnPaint { nullptr };
	OnWindowEventSignature m_OnEvent { nullptr };
	OnLoadTextureSignature m_OnLoadTexture { nullptr };
	OnEmptySignature m_OnExit { nullptr };
	OnGetClipboardContentsSignature m_OnGetClipboardContents { nullptr };
	OnSetWindowTitleSignature m_OnSetWindowTitle { nullptr };
};

}
