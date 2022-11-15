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

#include "Window.h"
#include "Application.h"
#include "Controls/ControlList.h"
#include "Controls/MenuBar.h"
#include "Controls/MenuItem.h"
#include "Controls/WindowContainer.h"
#include "Json.h"
#include "Paint.h"
#include "Profiler.h"
#include "Timer.h"

#if TOOLS
    #include "Tools/CommandPalette.h"
#endif

#include <algorithm>

namespace OctaneGUI
{

Window::Window(Application* InApplication)
    : m_Application(InApplication)
{
    m_Popup.SetOnInvalidate([=](std::shared_ptr<Control> Focus, InvalidateType Type) -> void
        {
            if ((Type == InvalidateType::Layout || Type == InvalidateType::Both))
            {
                RequestLayout(std::dynamic_pointer_cast<Container>(Focus));
            }

            m_Repaint = true;
        });

    m_Popup.SetOnClose([=](const Container& Focus) -> void
        {
            if (m_OnPopupClose)
            {
                m_OnPopupClose(Focus);
                m_OnPopupClose = nullptr;
            }

            m_Container->CloseMenuBar();
            m_Repaint = true;
        });
}

Window::~Window()
{
    m_Container = nullptr;
}

void Window::SetTitle(const char* Title)
{
    m_Title = Title;

    if (m_OnSetTitle)
    {
        m_OnSetTitle(*this, Title);
    }
}

const char* Window::GetTitle() const
{
    return m_Title.c_str();
}

Window& Window::SetPosition(Vector2 Position)
{
    const Vector2 Size = m_Bounds.GetSize();
    m_Bounds.Min = Position;
    SetSize(Size);

    if (m_OnSetPosition)
    {
        m_OnSetPosition(*this);
    }
    return *this;
}

Vector2 Window::GetPosition() const
{
    return m_Bounds.Min;
}

Window& Window::SyncPosition(const Vector2& Position, bool MaximizeEvent)
{
    const Vector2 Size = m_Bounds.GetSize();
    m_Bounds.Min = Position;
    m_Bounds.Max = m_Bounds.Min + Size;

    if (!MaximizeEvent && IsMaximized())
    {
        SetMaximized(false);
        // TODO: Restore bounds to match the behavior found on Windows when maximized window is dragged.
    }

    return *this;
}

Window& Window::SetSize(const Vector2& Size)
{
    m_Bounds.Max = m_Bounds.Min + Size;
    m_Container->SetSize(m_Bounds.GetSize() * m_RenderScale);
    m_Container->InvalidateLayout();
    return *this;
}

Window& Window::SetSizeNotify(const Vector2& Size)
{
    SetSize(Size);

    if (m_OnSetSize)
    {
        m_OnSetSize(*this);
    }

    return *this;
}

Vector2 Window::GetSize() const
{
    return m_Bounds.Max - m_Bounds.Min;
}

Rect Window::RestoreBounds() const
{
    return m_RestoreBounds;
}

Window& Window::SetDeviceSize(Vector2 Size)
{
    m_DeviceSize = Size;
    return *this;
}

Vector2 Window::DeviceSize() const
{
    return m_DeviceSize;
}

Window& Window::SetRenderScale(Vector2 RenderScale)
{
    m_RenderScale = RenderScale;
    m_Container->SetSize(m_Bounds.GetSize() * m_RenderScale);
    return *this;
}

Vector2 Window::RenderScale() const
{
    return m_RenderScale;
}

Window& Window::SetHighDPI(bool HighDPI)
{
    if (HighDPI)
    {
        SetFlags(WindowFlags::HighDPI);
    }
    else
    {
        UnsetFlags(WindowFlags::HighDPI);
    }
    return *this;
}

bool Window::HighDPI() const
{
    return m_Flags & WindowFlags::HighDPI;
}

void Window::SetID(const char* ID)
{
    m_ID = ID;
}

const char* Window::ID() const
{
    return m_ID.c_str();
}

bool Window::HasID() const
{
    return !m_ID.empty();
}

Window& Window::SetVisible(bool Visible)
{
    m_Visible = Visible;
    m_Repaint = Visible;
    if (m_Visible)
    {
        SetCustomTitleBar(CustomTitleBar());
    }
    return *this;
}

bool Window::IsVisible() const
{
    return m_Visible;
}

Window& Window::RequestClose(bool Request)
{
    m_RequestClose = Request;
    return *this;
}

bool Window::ShouldClose() const
{
    return m_RequestClose;
}

void Window::Close()
{
    if (m_OnClose)
    {
        m_OnClose(*this);
    }
}

Window& Window::SetResizable(bool Resizable)
{
    if (Resizable)
    {
        SetFlags(WindowFlags::Resizable);
    }
    else
    {
        UnsetFlags(WindowFlags::Resizable);
    }
    return *this;
}

bool Window::IsResizable() const
{
    return m_Flags & WindowFlags::Resizable;
}

Window& Window::Minimize()
{
    if (m_OnMinimize)
    {
        m_OnMinimize(*this);
    }

    return *this;
}

Window& Window::SetCanMinimize(bool CanMinimize)
{
    if (CanMinimize)
    {
        SetFlags(WindowFlags::CanMinimize);
    }
    else
    {
        UnsetFlags(WindowFlags::CanMinimize);
    }
    return *this;
}

bool Window::CanMinimize() const
{
    return m_Flags & WindowFlags::CanMinimize;
}

Window& Window::Maximize()
{
    if (m_OnMaximize)
    {
        if (!IsMaximized())
        {
            m_RestoreBounds = m_Bounds;
        }

        m_OnMaximize(*this);
        SetMaximized(!IsMaximized());
    }

    return *this;
}

Window& Window::SetMaximized(bool Maximized)
{
    if (Maximized)
    {
        SetFlags(WindowFlags::Maximized);
    }
    else
    {
        UnsetFlags(WindowFlags::Maximized);
    }
    m_Container->SetMaximized(Maximized);
    m_Container->InvalidateLayout();
    return *this;
}

bool Window::IsMaximized() const
{
    return m_Flags & WindowFlags::Maximized;
}

Window& Window::SetCustomTitleBar(bool CustomTitleBar)
{
    if (CustomTitleBar)
    {
        UnsetFlags(WindowFlags::TitleBar);
    }
    else
    {
        SetFlags(WindowFlags::TitleBar);
    }

    // Icons are not loaded until after the initial window is created. The CustomTitleBar property can be set during
    // loading which can occur before these icons are loaded. Another attempt to set the custom title bar will be
    // done when the window visibility is set to true.
    if (m_Container && GetIcons() != nullptr)
    {
        m_Container->ShowTitleBar(CustomTitleBar);
        SetFocused(Focused());
    }

    return *this;
}

bool Window::CustomTitleBar() const
{
    return !(m_Flags & WindowFlags::TitleBar);
}

Window& Window::SetFlags(uint64_t Flags)
{
    m_Flags = m_Flags | Flags;
    return *this;
}

Window& Window::UnsetFlags(uint64_t Flags)
{
    m_Flags = m_Flags & ~Flags;
    return *this;
}

Window& Window::SetFocus(const std::shared_ptr<Control>& Focus)
{
    UpdateFocus(Focus);
    return *this;
}

const std::weak_ptr<Control>& Window::Focus() const
{
    return m_Focus;
}

const std::weak_ptr<Control>& Window::Hovered() const
{
    return m_Hovered;
}

Window& Window::SetFocused(bool Focused)
{
    if (Focused)
    {
        SetFlags(WindowFlags::Focused);
    }
    else
    {
        UnsetFlags(WindowFlags::Focused);
    }

    m_Container->SetFocused(Focused);

    return *this;
}

bool Window::Focused() const
{
    return m_Flags & WindowFlags::Focused;
}

bool Window::Modal() const
{
    return m_Flags & WindowFlags::Modal;
}

Application& Window::App() const
{
    return *m_Application;
}

void Window::SetPopup(const std::shared_ptr<Container>& Popup, OnContainerSignature Callback, bool Modal)
{
    m_Popup.Open(Popup, Modal);
    m_OnPopupClose = Callback;
    RequestLayout(Popup);
}

void Window::ClosePopup()
{
    if (!m_Focus.expired())
    {
        if (m_Popup.HasControl(m_Focus.lock()))
        {
            m_Focus.lock()->OnUnfocused();
            m_Focus.reset();
        }
    }

    m_Popup.Close();
}

const std::shared_ptr<Container>& Window::GetPopup() const
{
    return m_Popup.GetContainer();
}

bool Window::IsPopupOpen() const
{
    if (m_Popup.GetContainer())
    {
        return true;
    }

    return false;
}

void Window::OnKeyPressed(Keyboard::Key Key)
{
#if TOOLS
    if (Key == Keyboard::Key::P && (IsKeyPressed(Keyboard::Key::LeftControl) || IsKeyPressed(Keyboard::Key::RightControl)))
    {
        App().Tools().ShowCommandPalette(this);
        return;
    }
#endif

    if (m_Focus.expired() || Key == Keyboard::Key::None)
    {
        return;
    }

    std::shared_ptr<Control> Focused = m_Focus.lock();
    Focused->OnKeyPressed(Key);
}

void Window::OnKeyReleased(Keyboard::Key Key)
{
    if (m_Focus.expired() || Key == Keyboard::Key::None)
    {
        return;
    }

    std::shared_ptr<Control> Focused = m_Focus.lock();
    Focused->OnKeyReleased(Key);
}

void Window::OnMouseMove(const Vector2& Position)
{
    m_MousePosition = Position * m_RenderScale;

    std::weak_ptr<Control> Hovered = m_Popup.GetControl(m_MousePosition);

    if (!m_Popup.IsModal() && Hovered.expired())
    {
        Hovered = m_Container->GetControl(m_MousePosition);
    }

    std::shared_ptr<Control> Current = Hovered.lock();
    std::shared_ptr<Control> Previous = m_Hovered.lock();
    if (Current != Previous)
    {
        if (Previous)
        {
            Previous->OnMouseLeave();
        }

        m_Hovered = Hovered;

        if (Current)
        {
            Current->OnMouseEnter();
        }
    }

    if (Current)
    {
        Current->OnMouseMove(m_MousePosition);
    }

    if (!m_Focus.expired())
    {
        std::shared_ptr<Control> Focused = m_Focus.lock();

        if (Current != Focused)
        {
            Focused->OnMouseMove(m_MousePosition);
        }
    }
}

void Window::OnMousePressed(const Vector2& Position, Mouse::Button MouseButton, Mouse::Count Count)
{
    std::shared_ptr<Control> New = nullptr;
    if (!m_Hovered.expired())
    {
        std::shared_ptr<Control> Hovered = m_Hovered.lock();
        bool Pressed = Hovered->OnMousePressed(Position * m_RenderScale, MouseButton, Count);

        if (Pressed)
        {
            New = Hovered;
        }
    }

    UpdateFocus(New);
}

void Window::OnMouseReleased(const Vector2& Position, Mouse::Button MouseButton)
{
    std::shared_ptr<Control> Hovered;
    std::shared_ptr<Control> Focused;

    if (!m_Hovered.expired())
    {
        Hovered = m_Hovered.lock();
    }

    if (!m_Focus.expired())
    {
        Focused = m_Focus.lock();
    }

    if (Hovered && Hovered != Focused)
    {
        Hovered->OnMouseReleased(Position * m_RenderScale, MouseButton);
    }

    if (Focused)
    {
        Focused->OnMouseReleased(Position * m_RenderScale, MouseButton);
    }
}

void Window::OnMouseWheel(const Vector2& Delta)
{
    if (!m_Hovered.expired())
    {
        m_Hovered.lock()->OnMouseWheel(Delta);
    }
}

void Window::OnMouseEnter()
{
    m_Application->SetMouseCursor(this, Mouse::Cursor::Arrow);
}

void Window::OnMouseLeave()
{
    if (!m_Hovered.expired())
    {
        m_Hovered.lock()->OnMouseLeave();
    }

    m_Hovered.reset();
}

void Window::OnText(uint32_t Code)
{
    if (m_Focus.expired())
    {
        return;
    }

    std::shared_ptr<Control> Focused = m_Focus.lock();
    Focused->OnText(Code);
}

void Window::ThemeLoaded()
{
    m_Container->OnThemeLoaded();
}

void Window::CreateContainer()
{
    m_Container = std::make_shared<WindowContainer>(this);
    m_Container
        ->SetOnInvalidate([=](std::shared_ptr<Control> Focus, InvalidateType Type) -> void
            {
                if ((Type == InvalidateType::Layout || Type == InvalidateType::Both))
                {
                    RequestLayout(std::dynamic_pointer_cast<Container>(Focus));
                }

                m_Repaint = true;
            });

    m_Repaint = true;
}

std::shared_ptr<Container> Window::GetContainer() const
{
    return m_Container->Body();
}

std::shared_ptr<MenuBar> Window::GetMenuBar() const
{
    return m_Container->GetMenuBar();
}

std::shared_ptr<WindowContainer> Window::GetRootContainer() const
{
    return m_Container;
}

std::shared_ptr<Theme> Window::GetTheme() const
{
    return m_Application->GetTheme();
}

std::shared_ptr<Icons> Window::GetIcons() const
{
    return m_Application->GetIcons();
}

TextureCache& Window::GetTextureCache() const
{
    return m_Application->GetTextureCache();
}

Vector2 Window::GetMousePosition() const
{
    return m_MousePosition;
}

bool Window::IsKeyPressed(Keyboard::Key Key) const
{
    return m_Application->IsKeyPressed(Key);
}

Window& Window::SetMouseCursor(Mouse::Cursor Cursor)
{
    if (m_MouseCursor == Cursor)
    {
        return *this;
    }

    m_MouseCursor = Cursor;
    m_Application->SetMouseCursor(this, Cursor);
    return *this;
}

Mouse::Cursor Window::MouseCursor() const
{
    return m_MouseCursor;
}

void Window::Update()
{
    PROFILER_SAMPLE_GROUP((std::string("Window::Update (") + GetTitle() + ")").c_str());

    UpdateTimers();

    if (!m_LayoutRequests.empty())
    {
        const std::vector<std::weak_ptr<Container>> Requests = m_LayoutRequests;
        m_LayoutRequests.clear();
        for (const std::weak_ptr<Container>& Item : Requests)
        {
            if (!Item.expired())
            {
                Item.lock()->Layout();
            }
        }

        if (m_OnLayout)
        {
            m_OnLayout(*this);
        }
    }

    m_Popup.Update();
}

void Window::DoPaint(Paint& Brush)
{
    if (m_Repaint)
    {
        PROFILER_SAMPLE_GROUP((std::string("Window::OnPaint (") + GetTitle() + ")").c_str());

        m_Container->OnPaint(Brush);
        m_Popup.OnPaint(Brush);
        m_Repaint = false;
        m_OnPaint(this, Brush.GetBuffer());
    }
}

void Window::Repaint()
{
    m_Repaint = true;
}

void Window::Load(const char* JsonStream)
{
    Load(Json::Parse(JsonStream));
}

void Window::Load(const char* JsonStream, ControlList& List)
{
    Load(Json::Parse(JsonStream));
    Populate(List);
}

void Window::Load(const Json& Root)
{
    LoadRoot(Root);
    LoadContents(Root);
}

void Window::Load(const Json& Root, ControlList& List)
{
    Load(Root);
    Populate(List);
}

void Window::LoadRoot(const Json& Root)
{
    const std::string Title = Root["Title"].String();
    float Width = Root["Width"].Number(640.0f);
    float Height = Root["Height"].Number(480.0f);

    SetTitle(Title.c_str());
    SetSize({ Width, Height });
    SetResizable(Root["Resizable"].Boolean(IsResizable()));
    SetCanMinimize(Root["CanMinimize"].Boolean(CanMinimize()));
    SetCustomTitleBar(Root["CustomTitleBar"].Boolean(CustomTitleBar()));

    if (Root["Modal"].Boolean(Modal()))
    {
        SetFlags(WindowFlags::Modal);
    }
}

void Window::LoadContents(const Json& Root)
{
    m_Container->OnLoad(Root);
}

void Window::LoadContents(const Json& Root, ControlList& List)
{
    LoadContents(Root);
    Populate(List);
}

void Window::Clear()
{
    m_Container->Clear();
    m_Popup.Close();
    m_LayoutRequests.clear();
}

std::shared_ptr<Timer> Window::CreateTimer(int Interval, bool Repeat, OnEmptySignature&& Callback)
{
    std::shared_ptr Result = std::make_unique<Timer>(Interval, Repeat, this, std::move(Callback));
    return std::move(Result);
}

void Window::StartTimer(const std::shared_ptr<Timer>& Object)
{
    for (std::vector<TimerHandle>::iterator It = m_Timers.begin(); It != m_Timers.end();)
    {
        TimerHandle& Handle = *It;

        if (Handle.Object.expired())
        {
            It = m_Timers.erase(It);
        }
        else if (Handle.Object.lock() == Object)
        {
            Handle.Elapsed.Reset();
            return;
        }
        else
        {
            It++;
        }
    }

    m_Timers.emplace_back(Object);
}

bool Window::ClearTimer(const std::shared_ptr<Timer>& Object)
{
    for (std::vector<TimerHandle>::const_iterator It = m_Timers.begin(); It != m_Timers.end();)
    {
        const TimerHandle& Handle = *It;

        if (Handle.Object.expired() || Handle.Object.lock() == Object)
        {
            m_Timers.erase(It);
            return true;
        }
        else
        {
            ++It;
        }
    }

    return false;
}

Window& Window::SetOnPaint(OnPaintSignature&& Fn)
{
    m_OnPaint = std::move(Fn);
    return *this;
}

Window& Window::SetOnSetTitle(OnSetTitleSignature&& Fn)
{
    m_OnSetTitle = std::move(Fn);
    return *this;
}

Window& Window::SetOnSetPosition(OnWindowSignature&& Fn)
{
    m_OnSetPosition = std::move(Fn);
    return *this;
}

Window& Window::SetOnSetSize(OnWindowSignature&& Fn)
{
    m_OnSetSize = std::move(Fn);
    return *this;
}

Window& Window::SetOnMinimize(OnWindowSignature&& Fn)
{
    m_OnMinimize = std::move(Fn);
    return *this;
}

Window& Window::SetOnMaximize(OnWindowSignature&& Fn)
{
    m_OnMaximize = std::move(Fn);
    return *this;
}

Window& Window::SetOnClose(OnWindowSignature&& Fn)
{
    m_OnClose = std::move(Fn);
    return *this;
}

Window& Window::SetOnLayout(OnWindowSignature&& Fn)
{
    m_OnLayout = std::move(Fn);
    return *this;
}

Window::Window()
{
}

void Window::Populate(ControlList& List) const
{
    m_Container->GetControlList(List);
}

void Window::RequestLayout(std::shared_ptr<Container> Request)
{
    if (!Request)
    {
        return;
    }

    bool Found = false;
    for (const std::weak_ptr<Container>& Item : m_LayoutRequests)
    {
        if (!Item.expired() && Item.lock() == Request)
        {
            Found = true;
            break;
        }
    }

    if (!Found)
    {
        m_LayoutRequests.push_back(Request);
    }
}

void Window::UpdateTimers()
{
    for (std::vector<TimerHandle>::iterator It = m_Timers.begin(); It != m_Timers.end();)
    {
        TimerHandle& Handle = *It;

        if (Handle.Object.expired())
        {
            It = m_Timers.erase(It);
            continue;
        }

        std::shared_ptr<Timer> Object = Handle.Object.lock();
        if (Handle.Elapsed.MeasureMS() >= Object->Interval())
        {
            Object->Invoke();

            if (Object->Repeat())
            {
                Handle.Elapsed.Reset();
                It++;
            }
            else
            {
                It = m_Timers.erase(It);
            }
        }
        else
        {
            It++;
        }
    }
}

void Window::UpdateFocus(const std::shared_ptr<Control>& Focus)
{
    std::shared_ptr<Control> Focused = m_Focus.lock();

    bool ShouldClosePopup = false;
    if (Focus != Focused)
    {
        if (Focused)
        {
            Focused->OnUnfocused();
        }

        m_Focus = Focus;

        if (Focus)
        {
            ShouldClosePopup = !m_Popup.HasControl(Focus);
            Focus->OnFocused();
        }
        else
        {
            ShouldClosePopup = true;
        }
    }

    if (ShouldClosePopup)
    {
        m_Popup.Close();
    }
}

}
