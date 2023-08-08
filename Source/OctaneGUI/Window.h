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

namespace WindowFlags
{

enum Flags
{
    None = 0,
    Resizable = 1 << 0,
    HighDPI = 1 << 1,
    CanMinimize = 1 << 2,
    Modal = 1 << 3,
    TitleBar = 1 << 4,
    Maximized = 1 << 5,
    Focused = 1 << 6,

    Normal = Resizable | HighDPI | CanMinimize | TitleBar,
};

}

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

    Window& SetPosition(Vector2 Position);
    Window& SetPositionBoundsOnly(const Vector2& Position);
    Vector2 GetPosition() const;

    /// @brief Sync position from frontend.
    ///
    /// This function is meant to sync the position from what is reported from the Frontend.
    /// The m_Bounds member is updated and no callback is invoked when syncing occurs. This
    /// also prevents any invalidation to occur since no contents were resized.
    ///
    /// @param Position The new position of the window
    /// @param MaximizeEvent Whether the syncing occurred due to a maximize event.
    /// @return This reference for chaining.
    Window& SyncPosition(const Vector2& Position, bool MaximizeEvent);

    Window& SetSize(const Vector2& Size);

    /// @brief Calls the SetSize function but also invokes callback to update frontend window size.
    /// @param Size The new size of the window.
    /// @return This reference for chaining.
    Window& SetSizeNotify(const Vector2& Size);
    Vector2 GetSize() const;

    Rect RestoreBounds() const;

    Window& SetDeviceSize(Vector2 Size);
    Vector2 DeviceSize() const;

    Window& SetRenderScale(Vector2 RenderScale);
    Vector2 RenderScale() const;

    Window& SetHighDPI(bool HighDPI);
    bool HighDPI() const;

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

    Window& Minimize();
    Window& SetCanMinimize(bool CanMinimize);
    bool CanMinimize() const;

    Window& Maximize();
    Window& SetMaximized(bool Maximized);
    bool IsMaximized() const;

    Window& SetCustomTitleBar(bool CustomTitleBar);
    bool CustomTitleBar() const;

    Window& SetFlags(uint64_t Flags);
    Window& UnsetFlags(uint64_t Flags);

    Window& SetFocus(const std::shared_ptr<Control>& Focus);
    const std::weak_ptr<Control>& Focus() const;
    const std::weak_ptr<Control>& Hovered() const;

    Window& SetFocused(bool Focused);
    bool Focused() const;

    bool Modal() const;

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
    std::shared_ptr<WindowContainer> GetRootContainer() const;
    std::shared_ptr<Theme> GetTheme() const;
    std::shared_ptr<Icons> GetIcons() const;
    TextureCache& GetTextureCache() const;
    Vector2 GetMousePosition() const;
    bool IsKeyPressed(Keyboard::Key Key) const;
    Window& SetMouseCursor(Mouse::Cursor Cursor);
    Mouse::Cursor MouseCursor() const;
    Window& SetMousePosition(const Vector2& Position);

    void Update();
    void DoPaint(Paint& Brush);
    void Repaint();

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
    Window& SetOnSetPosition(OnWindowSignature&& Fn);
    Window& SetOnSetSize(OnWindowSignature&& Fn);
    Window& SetOnMinimize(OnWindowSignature&& Fn);
    Window& SetOnMaximize(OnWindowSignature&& Fn);
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
    Mouse::Cursor m_MouseCursor { Mouse::Cursor::Arrow };
    Rect m_Bounds {};
    Rect m_RestoreBounds {};
    Vector2 m_DeviceSize {};
    Vector2 m_RenderScale { 1.0f, 1.0f };
    std::shared_ptr<WindowContainer> m_Container { nullptr };
    bool m_Repaint { false };
    std::weak_ptr<Control> m_Focus {};
    std::weak_ptr<Control> m_Hovered {};
    Popup m_Popup {};
    bool m_Visible { false };
    bool m_RequestClose { false };
    uint64_t m_Flags { WindowFlags::Normal };
    std::vector<std::weak_ptr<Container>> m_LayoutRequests;

    std::vector<TimerHandle> m_Timers {};

    OnPaintSignature m_OnPaint { nullptr };
    OnContainerSignature m_OnPopupClose { nullptr };
    OnSetTitleSignature m_OnSetTitle { nullptr };
    OnWindowSignature m_OnSetPosition { nullptr };
    OnWindowSignature m_OnSetSize { nullptr };
    OnWindowSignature m_OnMinimize { nullptr };
    OnWindowSignature m_OnMaximize { nullptr };
    OnWindowSignature m_OnClose { nullptr };
    OnWindowSignature m_OnLayout { nullptr };
};

}
