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

#include "../Windowing.h"
#include "OctaneGUI/OctaneGUI.h"
#include "SDL.h"
#include "SDL_syswm.h"

// X11 defines 'None' as 0. Some OctaneGUI enums will have a 'None' entry
// which will cause a conflict when attempting to compile any code that uses
// these enum entries. 'None' define is not used in this file so ignore for now.
#undef None

#include <unordered_map>

namespace Frontend
{
namespace Windowing
{

std::unordered_map<OctaneGUI::Window*, SDL_Window*> g_Windows;
std::unordered_map<uint32_t, std::vector<SDL_Event>> g_UnhandledEvents {};
std::unordered_map<SDL_SystemCursor, SDL_Cursor*> g_SystemCursors {};

OctaneGUI::Keyboard::Key GetKey(SDL_Keycode Code)
{
    switch (Code)
    {
    case SDLK_p: return OctaneGUI::Keyboard::Key::P;
    case SDLK_v: return OctaneGUI::Keyboard::Key::V;
    case SDLK_ESCAPE: return OctaneGUI::Keyboard::Key::Escape;
    case SDLK_BACKSPACE: return OctaneGUI::Keyboard::Key::Backspace;
    case SDLK_DELETE: return OctaneGUI::Keyboard::Key::Delete;
    case SDLK_LEFT: return OctaneGUI::Keyboard::Key::Left;
    case SDLK_RIGHT: return OctaneGUI::Keyboard::Key::Right;
    case SDLK_UP: return OctaneGUI::Keyboard::Key::Up;
    case SDLK_DOWN: return OctaneGUI::Keyboard::Key::Down;
    case SDLK_HOME: return OctaneGUI::Keyboard::Key::Home;
    case SDLK_END: return OctaneGUI::Keyboard::Key::End;
    case SDLK_LSHIFT: return OctaneGUI::Keyboard::Key::LeftShift;
    case SDLK_RSHIFT: return OctaneGUI::Keyboard::Key::RightShift;
    case SDLK_LCTRL: return OctaneGUI::Keyboard::Key::LeftControl;
    case SDLK_RCTRL: return OctaneGUI::Keyboard::Key::RightControl;
    case SDLK_LALT: return OctaneGUI::Keyboard::Key::LeftAlt;
    case SDLK_RALT: return OctaneGUI::Keyboard::Key::RightAlt;
    case SDLK_RETURN: return OctaneGUI::Keyboard::Key::Enter;
    case SDLK_TAB: return OctaneGUI::Keyboard::Key::Tab;
    default: break;
    }

    return OctaneGUI::Keyboard::Key::None;
}

OctaneGUI::Mouse::Button GetMouseButton(uint8_t Button)
{
    switch (Button)
    {
    case SDL_BUTTON_RIGHT: return OctaneGUI::Mouse::Button::Right;
    case SDL_BUTTON_MIDDLE: return OctaneGUI::Mouse::Button::Middle;
    case SDL_BUTTON_LEFT:
    default: break;
    }

    return OctaneGUI::Mouse::Button::Left;
}

SDL_SystemCursor GetMouseCursor(OctaneGUI::Mouse::Cursor Cursor)
{
    switch (Cursor)
    {
    case OctaneGUI::Mouse::Cursor::IBeam: return SDL_SYSTEM_CURSOR_IBEAM;
    case OctaneGUI::Mouse::Cursor::SizeWE: return SDL_SYSTEM_CURSOR_SIZEWE;
    case OctaneGUI::Mouse::Cursor::SizeNS: return SDL_SYSTEM_CURSOR_SIZENS;
    case OctaneGUI::Mouse::Cursor::Arrow:
    default: break;
    }

    return SDL_SYSTEM_CURSOR_ARROW;
}

OctaneGUI::Window* WindowID(const char* ID)
{
    for (const std::pair<OctaneGUI::Window*, SDL_Window*>& Item : g_Windows)
    {
        const std::string WinID { Item.first->ID() };
        if (WinID == ID)
        {
            return Item.first;
        }
    }

    return nullptr;
}

void* NativeHandle(SDL_Window* Window)
{
    SDL_SysWMinfo WM {};
    SDL_GetWindowWMInfo(Window, &WM);

#if defined(WINDOWS)
    return (void*)WM.info.win.window;
#elif defined(APPLE)
    return (void*)WM.info.cocoa.window;
#else
    return nullptr;
#endif
}

// Events in SDL have an associated window ID for which the event is tied to. With multiple windows,
// this ID may not match the window that is pumping the events. If the IDs do not match, the event
// is added to a queue for the appropriate window.
// On some platforms, a window ID of '0' may appear. This means that the event should be handled by
// all windows. This function will push the event to all windows if the SDL events are being pumped.
bool AddUnhandledEvent(SDL_Event& Event, const uint32_t EventWindowID, const uint32_t WindowID)
{
    // If the event's window ID is '0', then all other windows should have this event queued.
    if (EventWindowID == 0)
    {
        for (std::unordered_map<OctaneGUI::Window*, SDL_Window*>::iterator It = g_Windows.begin();
             It != g_Windows.end();
             ++It)
        {
            const uint32_t ItWindowID = SDL_GetWindowID(It->second);
            if (SDL_GetWindowID(It->second) != WindowID)
            {
                AddUnhandledEvent(Event, ItWindowID, WindowID);
            }
        }
    }
    // If the event does not match the target window, then we must queue it.
    else if (EventWindowID != WindowID)
    {
        std::vector<SDL_Event>& Events = g_UnhandledEvents[EventWindowID];
        Events.push_back(Event);
        // This event is not meant for the target window. Should not continue processing.
        return false;
    }

    // Calling function should continue processing this event.
    return true;
}

OctaneGUI::Event HandleEvent(SDL_Event& Event, const uint32_t WindowID, bool IsPumping)
{
    // If windowID for any event is 0, then that event should affect all windows.
    switch (Event.type)
    {
    case SDL_QUIT: return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.key.windowID, WindowID))
        {
            break;
        }

        return OctaneGUI::Event(
            Event.key.type == SDL_KEYDOWN ? OctaneGUI::Event::Type::KeyPressed : OctaneGUI::Event::Type::KeyReleased,
            OctaneGUI::Event::Key(GetKey(Event.key.keysym.sym)));
    }

    case SDL_MOUSEMOTION:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.motion.windowID, WindowID))
        {
            break;
        }

        return OctaneGUI::Event(
            OctaneGUI::Event::MouseMove(Event.motion.x, Event.motion.y));
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.button.windowID, WindowID))
        {
            break;
        }

        OctaneGUI::Mouse::Count Count = OctaneGUI::Mouse::Count::Single;
        if (Event.button.clicks == 2)
        {
            Count = OctaneGUI::Mouse::Count::Double;
        }
        else if (Event.button.clicks == 3)
        {
            Count = OctaneGUI::Mouse::Count::Triple;
        }

        return OctaneGUI::Event(
            Event.button.type == SDL_MOUSEBUTTONDOWN ? OctaneGUI::Event::Type::MousePressed : OctaneGUI::Event::Type::MouseReleased,
            OctaneGUI::Event::MouseButton(GetMouseButton(Event.button.button), (float)Event.button.x, (float)Event.button.y, Count));
    }

    case SDL_MOUSEWHEEL:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.wheel.windowID, WindowID))
        {
            break;
        }

        return OctaneGUI::Event(
            OctaneGUI::Event::MouseWheel(Event.wheel.x, Event.wheel.y));
    }

    case SDL_TEXTINPUT:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.text.windowID, WindowID))
        {
            break;
        }

        return OctaneGUI::Event(
            OctaneGUI::Event::Text(*(uint32_t*)Event.text.text));
    }

    case SDL_WINDOWEVENT:
    {
        if (IsPumping && !AddUnhandledEvent(Event, Event.window.windowID, WindowID))
        {
            break;
        }

        switch (Event.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED: return OctaneGUI::Event(
            OctaneGUI::Event::WindowResized((float)Event.window.data1, (float)Event.window.data2));

        case SDL_WINDOWEVENT_CLOSE: return OctaneGUI::Event(
            OctaneGUI::Event::Type::WindowClosed);

        case SDL_WINDOWEVENT_ENTER: return OctaneGUI::Event(
            OctaneGUI::Event::Type::WindowEnter);

        case SDL_WINDOWEVENT_LEAVE: return OctaneGUI::Event(
            OctaneGUI::Event::Type::WindowLeave);
        }
    }

    default: break;
    }

    return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

bool Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_version Version {};
    SDL_GetVersion(&Version);
    printf("Using SDL version %d.%d.%d\n", Version.major, Version.minor, Version.patch);

    g_SystemCursors[SDL_SYSTEM_CURSOR_ARROW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    g_SystemCursors[SDL_SYSTEM_CURSOR_IBEAM] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    g_SystemCursors[SDL_SYSTEM_CURSOR_SIZEWE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    g_SystemCursors[SDL_SYSTEM_CURSOR_SIZENS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);

    return true;
}

void NewWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        int Flags = SDL_WINDOW_ALLOW_HIGHDPI;

        if (Window->Modal())
        {
            Flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        }

        if (Window->CustomTitleBar())
        {
            Flags |= SDL_WINDOW_BORDERLESS;
        }
        else
        {
            if (Window->IsResizable())
            {
                Flags |= SDL_WINDOW_RESIZABLE;
            }
        }

#if OPENGL
        Flags |= SDL_WINDOW_OPENGL;
#endif

        SDL_Window* Instance = SDL_CreateWindow(
            Window->GetTitle(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            (int)Window->GetSize().X,
            (int)Window->GetSize().Y,
            Flags);

        if (Window->Modal())
        {
            // This doesn't seem to be affecting any behavior but will keep
            // this around for further testing.
            OctaneGUI::Window* Main = WindowID("Main");
            if (Main != nullptr)
            {
                SDL_SetWindowModalFor(Instance, g_Windows[Main]);
            }

            SetAlwaysOnTop(NativeHandle(Instance));
        }

        ShowMinimize(NativeHandle(Instance), Window->CanMinimize());

        const int DisplayIndex = SDL_GetWindowDisplayIndex(Instance);
        SDL_DisplayMode DisplayMode {};
        SDL_GetCurrentDisplayMode(DisplayIndex, &DisplayMode);
        Window->SetDeviceSize({ (float)DisplayMode.w, (float)DisplayMode.h });

        g_Windows[Window] = Instance;
    }
}

void DestroyWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    SDL_Window* Instance = g_Windows[Window];
    g_UnhandledEvents.erase(SDL_GetWindowID(Instance));
    SDL_DestroyWindow(Instance);
    g_Windows.erase(Window);
}

void RaiseWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    SDL_RaiseWindow(g_Windows[Window]);
}

void ToggleWindow(OctaneGUI::Window* Window, bool Enable)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    void* Handle = NativeHandle(g_Windows[Window]);
    Windowing::SetEnabled(Handle, Enable);

    if (Enable)
    {
        RaiseWindow(Window);
    }

    Windowing::SetMovable(Handle, Enable);
}

void NewFrame()
{
    // Only hold a max of 10 unhandled events.
    for (std::unordered_map<uint32_t, std::vector<SDL_Event>>::iterator It = g_UnhandledEvents.begin();
         It != g_UnhandledEvents.end();
         ++It)
    {
        std::vector<SDL_Event>& Events = It->second;
        if (Events.size() > 10)
        {
            Events.erase(Events.begin(), Events.begin() + Events.size() - 10);
        }
    }
}

OctaneGUI::Event Event(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);
    }

    SDL_Window* Instance = g_Windows[Window];
    const uint32_t WindowID = SDL_GetWindowID(Instance);

    std::vector<SDL_Event>& Events = g_UnhandledEvents[WindowID];
    while (!Events.empty())
    {
        SDL_Event Event = Events[0];
        Events.erase(Events.begin());
        OctaneGUI::Event Processed = HandleEvent(Event, WindowID, false);
        if (Processed.GetType() != OctaneGUI::Event::Type::None)
        {
            return Processed;
        }
    }

    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        OctaneGUI::Event Processed = HandleEvent(Event, WindowID, true);
        if (Processed.GetType() != OctaneGUI::Event::Type::None)
        {
            return Processed;
        }
    }

    return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

void Exit()
{
    for (const std::pair<SDL_SystemCursor, SDL_Cursor*>& SystemCursor : g_SystemCursors)
    {
        SDL_FreeCursor(SystemCursor.second);
    }

    for (const std::pair<OctaneGUI::Window*, SDL_Window*>& Item : g_Windows)
    {
        SDL_DestroyWindow(Item.second);
    }

    g_Windows.clear();
    SDL_Quit();
}

void SetClipboardContents(const std::u32string& Contents)
{
    SDL_SetClipboardText(OctaneGUI::String::ToMultiByte(Contents).c_str());
}

std::u32string GetClipboardContents()
{
    return OctaneGUI::String::ToUTF32(SDL_GetClipboardText());
}

void SetWindowTitle(OctaneGUI::Window* Window, const char* Title)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    SDL_SetWindowTitle(g_Windows[Window], Title);
}

void SetMouseCursor(OctaneGUI::Window* Window, OctaneGUI::Mouse::Cursor Cursor)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    SDL_SystemCursor SystemCursor = GetMouseCursor(Cursor);

    if (g_SystemCursors.find(SystemCursor) == g_SystemCursors.end())
    {
        printf("System cursor %d not supported or implemented!\n", (int)SystemCursor);
        return;
    }

    SDL_SetCursor(g_SystemCursors[SystemCursor]);
}

SDL_Window* Get(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return nullptr;
    }

    return g_Windows[Window];
}

}
}
