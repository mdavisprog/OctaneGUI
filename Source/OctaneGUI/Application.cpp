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

#include "Application.h"
#include "Assert.h"
#include "Controls/Container.h"
#include "Controls/ControlList.h"
#include "Controls/WindowContainer.h"
#include "Event.h"
#include "Icons.h"
#include "Json.h"
#include "Paint.h"
#include "Profiler.h"
#include "Texture.h"
#include "Theme.h"
#include "Window.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

namespace OctaneGUI
{

Application::Application()
{
    Texture::SetOnLoad([this](const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height) -> uint32_t
        {
            if (m_OnLoadTexture)
            {
                return m_OnLoadTexture(Data, Width, Height);
            }

            return 0;
        });
}

Application::~Application()
{
}

bool Application::Initialize(const char* JsonStream, std::unordered_map<std::string, ControlList>& WindowControls)
{
    if (!Initialize())
    {
        Shutdown();
        return false;
    }

    const Json Root = Json::Parse(JsonStream);

    if (Root.IsNull())
    {
        Shutdown();
        return false;
    }

    const Json& Windows = Root["Windows"];
    if (!Windows.IsObject())
    {
        Shutdown();
        return false;
    }

    m_HighDPI = Root["HighDPI"].Boolean(m_HighDPI);

    // First, create and load base settings for each defined window.
    Windows.ForEach([&](const std::string& Key, const Json& Value) -> void
        {
            ControlList List;
            std::shared_ptr<Window> Item = CreateWindow(Key.c_str());
            Item->LoadRoot(Value);
        });

    // Create the main window. Some rendering platforms require a window to be created before
    // being able to load textures.
    Assert(m_Windows.find("Main") != m_Windows.end(), "No 'Main' window defined!");
    DisplayWindow("Main");

    m_Icons = std::make_shared<Icons>();
    const Json& IconsObject = Root["Icons"];
    if (IconsObject.IsNull() || IconsObject["UseInternal"].Boolean())
    {
        m_Icons->Initialize();
    }
    else
    {
        const Vector2 IconSize = Vector2::FromJson(IconsObject["Size"]) * GetMainWindow()->RenderScale();
        Assert(IconSize.Length() > 0.0f, "Icon size is not valid!");

        std::vector<Icons::Definition> Definitions;
        const Json& Types = IconsObject["Types"];
        for (unsigned int I = 0; I < Types.Count(); I++)
        {
            const Json& Item = Types[I];
            Definitions.push_back({ Item["Type"].String(), Item["FileName"].String() });
        }

        m_Icons->Initialize(Definitions, IconSize);
    }

    m_Theme->Load(Root["Theme"]);

    m_FileSystem.SetUseSystemFileDialog(Root["UseSystemFileDialog"].Boolean());

    // Now load the contents for each window. Some of the controls may require
    // a valid font. The font is loaded with the theme.
    Windows.ForEach([&](const std::string& Key, const Json& Value) -> void
        {
            ControlList List;
            m_Windows[Key]->LoadContents(Value, List);
            WindowControls[Key] = List;
        });

    return true;
}

void Application::Shutdown()
{
    for (auto& Item : m_Windows)
    {
        OnWindowAction(Item.second.get(), WindowAction::Destroy);
    }

    m_Windows.clear();
    m_Theme = nullptr;
    m_Icons = nullptr;
    m_IsRunning = false;

    if (m_OnExit)
    {
        m_OnExit();
    }
}

void Application::Update()
{
    for (auto& Item : m_Windows)
    {
        if (Item.second->IsVisible())
        {
            Item.second->Update();
        }
    }

    for (auto& Item : m_Windows)
    {
        if (Item.second->IsVisible())
        {
            Paint Brush(m_Theme);
            Item.second->DoPaint(Brush);
        }
    }

    for (auto& Item : m_Windows)
    {
        if (Item.second->ShouldClose())
        {
            DestroyWindow(Item.second);
        }
    }
}

int Application::Run()
{
    if (m_OnEvent)
    {
        while (m_IsRunning)
        {
            PROFILER_FRAME();

            if (m_OnNewFrame)
            {
                m_OnNewFrame();
            }

            int EventsProcessed = 0;
            for (auto& Item : m_Windows)
            {
                // Attempt to process all events for a single window.
                while (Item.second->IsVisible())
                {
                    int Processed = ProcessEvent(Item.second);
                    EventsProcessed += Processed;

                    if (Processed == 0)
                    {
                        break;
                    }
                }

                if (!m_IsRunning)
                {
                    EventsProcessed = 0;
                    break;
                }
            }

            Update();

            if (EventsProcessed <= 0)
            {
                PROFILER_SAMPLE("Sleep");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    Shutdown();
    return 0;
}

void Application::Quit()
{
    m_IsRunning = false;
}

std::shared_ptr<Window> Application::GetMainWindow() const
{
    if (m_Windows.size() == 0)
    {
        return nullptr;
    }

    return m_Windows.at("Main");
}

std::shared_ptr<Window> Application::GetWindow(const char* ID) const
{
    Assert(m_Windows.find(ID) != m_Windows.end(), "Window with ID '%s' doesn't exist.", ID);
    return m_Windows.at(ID);
}

std::shared_ptr<Window> Application::EventFocus() const
{
    if (m_EventFocus.expired())
    {
        return nullptr;
    }

    return m_EventFocus.lock();
}

bool Application::IsMainWindow(Window* InWindow) const
{
    for (std::unordered_map<std::string, std::shared_ptr<Window>>::const_iterator It = m_Windows.begin(); It != m_Windows.end(); ++It)
    {
        if (It->second.get() == InWindow)
        {
            return It->first == "Main";
        }
    }

    return false;
}

bool Application::HasWindow(const char* ID) const
{
    for (std::unordered_map<std::string, std::shared_ptr<Window>>::const_iterator It = m_Windows.begin(); It != m_Windows.end(); ++It)
    {
        if (It->first == ID)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<Window> Application::NewWindow(const char* ID, const char* JsonStream)
{
    std::shared_ptr<Window> Result = CreateWindow(ID);
    Result->Load(JsonStream);
    return Result;
}

std::shared_ptr<Window> Application::NewWindow(const char* ID, const char* JsonStream, ControlList& List)
{
    std::shared_ptr<Window> Result = CreateWindow(ID);
    Result->Load(JsonStream, List);
    return Result;
}

bool Application::DisplayWindow(const char* ID)
{
    auto It = m_Windows.find(ID);
    if (It == m_Windows.end())
    {
        return false;
    }

    if (It->second->IsVisible())
    {
        return true;
    }

    const Vector2 RenderScale = It->second->RenderScale();

    OnWindowAction(It->second.get(), WindowAction::Create);

    It->second
        ->SetVisible(true)
        .SetOnSetTitle([this](Window& Target, const char* Title) -> void
            {
                if (m_OnSetWindowTitle)
                {
                    m_OnSetWindowTitle(&Target, Title);
                }
            })
        .SetOnMinimize([this](Window& Target) -> void
            {
                OnWindowAction(&Target, WindowAction::Minimize);
            })
        .SetOnMaximize([this](Window& Target) -> void
            {
                OnWindowAction(&Target, WindowAction::Maximize);
            })
        .SetOnSetPosition([this](Window& Target) -> void
            {
                OnWindowAction(&Target, WindowAction::Position);
            });

    // The window's scale may be updated when it is displayed. Calling 'OnThemeLoaded' will
    // resize any controls and fonts based on this scaling.
    if (It->second->RenderScale() != RenderScale)
    {
        It->second->GetRootContainer()->OnThemeLoaded();
    }

    if (It->second->Modal())
    {
        // Notify all windows the mouse has left the window as they will not be receiving
        // events and any focused control will need to be unfocused.
        for (const std::pair<std::string, std::shared_ptr<Window>>& Item : m_Windows)
        {
            if (Item.second->IsVisible())
            {
                Item.second->OnMouseLeave();
            }
        }

        m_Modals.push_back(It->second);
    }

    return true;
}

void Application::CloseWindow(const char* ID)
{
    if (m_Windows.find(ID) == m_Windows.end())
    {
        return;
    }

    DestroyWindow(m_Windows[ID]);
}

std::shared_ptr<Theme> Application::GetTheme() const
{
    return m_Theme;
}

std::shared_ptr<Icons> Application::GetIcons() const
{
    return m_Icons;
}

TextureCache& Application::GetTextureCache()
{
    return m_TextureCache;
}

bool Application::IsKeyPressed(Keyboard::Key Key) const
{
    return std::find(m_PressedKeys.begin(), m_PressedKeys.end(), Key) != m_PressedKeys.end();
}

void Application::SetClipboardContents(const std::u32string& Contents)
{
    if (m_OnSetClipboardContents)
    {
        m_OnSetClipboardContents(Contents);
    }
}

std::u32string Application::ClipboardContents() const
{
    if (m_OnGetClipboardContents)
    {
        return m_OnGetClipboardContents();
    }

    return U"";
}

Application& Application::SetMouseCursor(Window* Target, Mouse::Cursor Cursor)
{
    if (Target == nullptr)
    {
        return *this;
    }

    if (m_OnSetMouseCursor)
    {
        m_OnSetMouseCursor(Target, Cursor);
    }

    return *this;
}

const FileSystem& Application::FS() const
{
    return m_FileSystem;
}

FileSystem& Application::FS()
{
    return m_FileSystem;
}

#if TOOLS
Tools::Interface& Application::Tools()
{
    return m_Tools;
}

Application& Application::SetIgnoreModals(bool IgnoreModals)
{
    m_IgnoreModals = IgnoreModals;
    return *this;
}
#endif

Application& Application::SetOnWindowAction(OnWindowActionSignature&& Fn)
{
    m_OnWindowAction = std::move(Fn);
    return *this;
}

Application& Application::SetOnPaint(OnWindowPaintSignature&& Fn)
{
    m_OnPaint = std::move(Fn);
    return *this;
}

Application& Application::SetOnNewFrame(OnEmptySignature&& Fn)
{
    m_OnNewFrame = std::move(Fn);
    return *this;
}

Application& Application::SetOnEvent(OnWindowEventSignature&& Fn)
{
    m_OnEvent = std::move(Fn);
    return *this;
}

Application& Application::SetOnLoadTexture(OnLoadTextureSignature&& Fn)
{
    m_OnLoadTexture = std::move(Fn);
    return *this;
}

Application& Application::SetOnExit(OnEmptySignature&& Fn)
{
    m_OnExit = std::move(Fn);
    return *this;
}

Application& Application::SetOnSetClipboardContents(OnSetClipboardContentsSignature&& Fn)
{
    m_OnSetClipboardContents = std::move(Fn);
    return *this;
}

Application& Application::SetOnGetClipboardContents(OnGetClipboardContentsSignature&& Fn)
{
    m_OnGetClipboardContents = std::move(Fn);
    return *this;
}

Application& Application::SetOnSetWindowTitle(OnSetWindowTitleSignature&& Fn)
{
    m_OnSetWindowTitle = std::move(Fn);
    return *this;
}

Application& Application::SetOnSetMouseCursor(OnSetMouseCursorSignature&& Fn)
{
    m_OnSetMouseCursor = std::move(Fn);
    return *this;
}

void Application::OnPaint(Window* InWindow, const VertexBuffer& Buffers)
{
    if (m_OnPaint)
    {
        m_OnPaint(InWindow, Buffers);
    }
}

std::shared_ptr<Window> Application::CreateWindow(const char* ID)
{
    std::shared_ptr<Window> Result = std::make_shared<Window>(this);
    m_Windows[ID] = Result;
    Result->SetHighDPI(m_HighDPI);
    Result->CreateContainer();
    Result->SetOnPaint(std::bind(&Application::OnPaint, this, std::placeholders::_1, std::placeholders::_2));
    Result->SetID(ID);
    return Result;
}

void Application::DestroyWindow(const std::shared_ptr<Window>& Item)
{
    if (!Item || !Item->IsVisible())
    {
        return;
    }

    OnWindowAction(Item.get(), WindowAction::Destroy);

    Item
        ->SetVisible(false)
        .SetOnSetTitle(nullptr)
        .SetOnMinimize(nullptr)
        .SetOnSetPosition(nullptr)
        .RequestClose(false)
        .Close();

    for (std::vector<std::weak_ptr<Window>>::const_iterator It = m_Modals.begin(); It != m_Modals.end();)
    {
        if (It->expired())
        {
            It = m_Modals.erase(It);
        }
        else
        {
            if (It->lock() == Item)
            {
                It = m_Modals.erase(It);
            }
            else
            {
                ++It;
            }
        }
    }

    if (!m_Modals.empty())
    {
        OnWindowAction(m_Modals.back().lock().get(), WindowAction::Enable);
    }
    else
    {
        for (const std::pair<std::string, std::shared_ptr<Window>>& Item : m_Windows)
        {
            OnWindowAction(Item.second.get(), WindowAction::Enable);
        }
    }

    if (std::string(Item->ID()) == "Main")
    {
        m_IsRunning = false;
    }
}

int Application::ProcessEvent(const std::shared_ptr<Window>& Item)
{
    if (!Item)
    {
        return 0;
    }

    int Processed = 0;
    Event E = m_OnEvent(Item.get());

#if TOOLS
    if (!m_Modals.empty() && !m_IgnoreModals)
#else
    if (!m_Modals.empty())
#endif
    {
        if (!m_Modals.back().expired())
        {
            const std::shared_ptr<Window> Top = m_Modals.back().lock();

            if (Top != Item)
            {
                if (E.GetType() != Event::Type::WindowResized)
                {
                    return Processed;
                }
            }
        }
        else
        {
            m_Modals.pop_back();
        }
    }

    m_EventFocus = Item;

    Processed++;
    switch (E.GetType())
    {
    case Event::Type::WindowClosed:
        DestroyWindow(Item);
        break;

    case Event::Type::KeyPressed:
    {
        Keyboard::Key Code = E.GetData().m_Key.m_Code;
        Item->OnKeyPressed(Code);
        if (std::find(m_PressedKeys.begin(), m_PressedKeys.end(), Code) == m_PressedKeys.end())
        {
            m_PressedKeys.push_back(Code);
        }
    }
    break;

    case Event::Type::KeyReleased:
    {
        Keyboard::Key Code = E.GetData().m_Key.m_Code;
        Item->OnKeyReleased(Code);
        std::vector<Keyboard::Key>::iterator Iter = std::remove(m_PressedKeys.begin(), m_PressedKeys.end(), Code);
        if (Iter != m_PressedKeys.end())
        {
            m_PressedKeys.erase(Iter);
        }
    }
    break;

    case Event::Type::MouseMoved:
        Item->OnMouseMove(E.GetData().m_MouseMove.m_Position);
        break;

    case Event::Type::MousePressed:
        Item->OnMousePressed(E.GetData().m_MouseButton.m_Position, E.GetData().m_MouseButton.m_Button, E.GetData().m_MouseButton.m_Count);
        break;

    case Event::Type::MouseReleased:
        Item->OnMouseReleased(E.GetData().m_MouseButton.m_Position, E.GetData().m_MouseButton.m_Button);
        break;

    case Event::Type::MouseWheel:
        Item->OnMouseWheel(E.GetData().m_MouseWheel.Delta);
        break;

    case Event::Type::WindowResized:
        Item->SetSize(E.GetData().m_Resized.m_Size);
        break;

    case Event::Type::Text:
        Item->OnText(E.GetData().m_Text.Code);
        break;

    case Event::Type::WindowEnter:
        Item->OnMouseEnter();
        break;

    case Event::Type::WindowLeave:
        Item->OnMouseLeave();
        break;
    
    case Event::Type::WindowMoved:
        Item->SyncPosition(E.GetData().m_Moved.m_Position);
        break;

    case Event::Type::None:
    default: Processed--; break;
    }

    m_EventFocus.reset();

    return Processed;
}

bool Application::Initialize()
{
    if (m_IsRunning)
    {
        return true;
    }

    m_Theme = std::make_shared<Theme>();
    m_Theme->SetOnThemeLoaded([this]() -> void
        {
            for (const std::pair<std::string, std::shared_ptr<Window>>& Item : m_Windows)
            {
                Item.second->ThemeLoaded();
                Item.second->GetRootContainer()->InvalidateLayout();
            }
        });

    m_IsRunning = true;

    return true;
}

void Application::OnWindowAction(Window* InWindow, WindowAction Action)
{
    if (m_OnWindowAction)
    {
        m_OnWindowAction(InWindow, Action);
    }
}

}
