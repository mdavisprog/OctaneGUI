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
#include "FileSystem.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "TextureCache.h"
#include "Vector2.h"

#if TOOLS
    #include "Tools/Tools.h"
#endif

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

enum class WindowAction : uint8_t
{
    Create,
    Destroy,
    Raise,
    Enable,
    Disable,
    Position,
    Minimize,
    Maximize,
};

class Application
{
public:
    typedef std::function<void(Window*)> OnWindowSignature;
    typedef std::function<void(Window*, const VertexBuffer&)> OnWindowPaintSignature;
    typedef std::function<Event(Window*)> OnWindowEventSignature;
    typedef std::function<void(Window*, WindowAction)> OnWindowActionSignature;
    typedef std::function<uint32_t(const std::vector<uint8_t>&, uint32_t, uint32_t)> OnLoadTextureSignature;
    typedef std::function<void(const std::u32string&)> OnSetClipboardContentsSignature;
    typedef std::function<std::u32string(void)> OnGetClipboardContentsSignature;
    typedef std::function<void(Window*, const char*)> OnSetWindowTitleSignature;
    typedef std::function<void(Window*, Mouse::Cursor)> OnSetMouseCursorSignature;

    Application();
    virtual ~Application();

    bool Initialize(const char* JsonStream, std::unordered_map<std::string, ControlList>& WindowControls);
    void Shutdown();
    void Update();
    int Run();
    void Quit();

    std::shared_ptr<Window> GetMainWindow() const;
    std::shared_ptr<Window> GetWindow(const char* ID) const;
    std::shared_ptr<Window> EventFocus() const;
    bool IsMainWindow(Window* InWindow) const;
    bool HasWindow(const char* ID) const;
    std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream);
    std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream, ControlList& List);
    bool DisplayWindow(const char* ID);
    void CloseWindow(const char* ID);
    std::shared_ptr<Theme> GetTheme() const;
    std::shared_ptr<Icons> GetIcons() const;
    TextureCache& GetTextureCache();
    bool IsKeyPressed(Keyboard::Key Key) const;
    void SetClipboardContents(const std::u32string& Contents);
    std::u32string ClipboardContents() const;
    Application& SetMouseCursor(Window* Target, Mouse::Cursor Cursor);

    const FileSystem& FS() const;
    FileSystem& FS();

#if TOOLS
    Tools::Interface& Tools();
    Application& SetIgnoreModals(bool IgnoreModals);
#endif

    Application& SetOnWindowAction(OnWindowActionSignature&& Fn);
    Application& SetOnPaint(OnWindowPaintSignature&& Fn);
    Application& SetOnNewFrame(OnEmptySignature&& Fn);
    Application& SetOnEvent(OnWindowEventSignature&& Fn);
    Application& SetOnLoadTexture(OnLoadTextureSignature&& Fn);
    Application& SetOnExit(OnEmptySignature&& Fn);
    Application& SetOnSetClipboardContents(OnSetClipboardContentsSignature&& Fn);
    Application& SetOnGetClipboardContents(OnGetClipboardContentsSignature&& Fn);
    Application& SetOnSetWindowTitle(OnSetWindowTitleSignature&& Fn);
    Application& SetOnSetMouseCursor(OnSetMouseCursorSignature&& Fn);

private:
    void OnPaint(Window* InWindow, const VertexBuffer& Buffer);
    std::shared_ptr<Window> CreateWindow(const char* ID);
    void DestroyWindow(const std::shared_ptr<Window>& Item);
    int ProcessEvent(const std::shared_ptr<Window>& Item);
    bool Initialize();
    void OnWindowAction(Window* InWindow, WindowAction Action);
    void LoadIcons(const Json& Root);

    std::unordered_map<std::string, std::shared_ptr<Window>> m_Windows;
    std::vector<std::weak_ptr<Window>> m_Modals {};
    std::weak_ptr<Window> m_EventFocus {};
    std::shared_ptr<Theme> m_Theme { nullptr };
    std::shared_ptr<Icons> m_Icons { nullptr };
    bool m_IsRunning { false };
    std::vector<Keyboard::Key> m_PressedKeys {};
    TextureCache m_TextureCache {};
    FileSystem m_FileSystem { *this };
    bool m_HighDPI { true };

#if TOOLS
    bool m_IgnoreModals { false };
    Tools::Interface m_Tools { *this };
#endif

    OnWindowActionSignature m_OnWindowAction { nullptr };
    OnWindowPaintSignature m_OnPaint { nullptr };
    OnEmptySignature m_OnNewFrame { nullptr };
    OnWindowEventSignature m_OnEvent { nullptr };
    OnLoadTextureSignature m_OnLoadTexture { nullptr };
    OnEmptySignature m_OnExit { nullptr };
    OnSetClipboardContentsSignature m_OnSetClipboardContents { nullptr };
    OnGetClipboardContentsSignature m_OnGetClipboardContents { nullptr };
    OnSetWindowTitleSignature m_OnSetWindowTitle { nullptr };
    OnSetMouseCursorSignature m_OnSetMouseCursor { nullptr };
};

}
