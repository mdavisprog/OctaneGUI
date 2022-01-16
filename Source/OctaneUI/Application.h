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
#include "Vector2.h"

#include <memory>
#include <vector>

namespace OctaneUI
{

class Event;
class Font;
class Icons;
class Theme;
class VertexBuffer;
class Window;

class Application
{
public:
	typedef std::function<void(Window*)> OnWindowSignature;
	typedef std::function<void(Window*, const std::vector<VertexBuffer>&)> OnWindowPaintSignature;
	typedef std::function<Event(Window*)> OnWindowEventSignature;
	typedef std::function<uint32_t(const std::vector<uint8_t>&, uint32_t, uint32_t)> OnLoadTextureSignature;

	Application();
	virtual ~Application();

	bool Initialize(const char* Title);
	void Shutdown();
	void Update();
	int Run();
	std::shared_ptr<Font> LoadFont(const char* Path);

	std::shared_ptr<Window> GetMainWindow() const;
	std::shared_ptr<Window> NewWindow(const char* Title, float Width, float Height);
	std::shared_ptr<Theme> GetTheme() const;
	std::shared_ptr<Icons> GetIcons() const;

	Application& SetOnCreateWindow(OnWindowSignature Fn);
	Application& SetOnDestroyWindow(OnWindowSignature Fn);
	Application& SetOnPaint(OnWindowPaintSignature Fn);
	Application& SetOnEvent(OnWindowEventSignature Fn);
	Application& SetOnLoadTexture(OnLoadTextureSignature Fn);
	Application& SetOnExit(OnEmptySignature Fn);

private:
	void OnPaint(Window* InWindow, const std::vector<VertexBuffer>& Buffers);
	std::shared_ptr<Window> CreateWindow(const char* Title, const Vector2& Size);
	void DestroyWindow(const std::shared_ptr<Window>& Item);
	void ProcessEvent(const std::shared_ptr<Window>& Item);

	std::vector<std::shared_ptr<Window>> m_Windows;
	std::shared_ptr<Theme> m_Theme;
	std::shared_ptr<Icons> m_Icons;
	bool m_IsRunning;
	OnWindowSignature m_OnCreateWindow;
	OnWindowSignature m_OnDestroyWindow;
	OnWindowPaintSignature m_OnPaint;
	OnWindowEventSignature m_OnEvent;
	OnLoadTextureSignature m_OnLoadTexture;
	OnEmptySignature m_OnExit;
};

}
