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
#include "SFML/Graphics.hpp"

#include <memory>
#include <unordered_map>

#if defined(WINDOWS)
    #include "../Windows/Windowing.h"
#endif

namespace Frontend
{
namespace Windowing
{

#define MULTI_CLICK_TIME_MS 300

std::unordered_map<OctaneGUI::Window*, std::shared_ptr<sf::RenderWindow>> g_Windows {};
OctaneGUI::Clock g_MouseButtonClock {};
std::unordered_map<OctaneGUI::Mouse::Button, uint8_t> g_MouseClicks {};
std::unordered_map<sf::Cursor::Type, sf::Cursor> g_Cursors;

OctaneGUI::Keyboard::Key GetKeyCode(sf::Keyboard::Key Key)
{
    switch (Key)
    {
    case sf::Keyboard::P: return OctaneGUI::Keyboard::Key::P;
    case sf::Keyboard::V: return OctaneGUI::Keyboard::Key::V;
    case sf::Keyboard::Escape: return OctaneGUI::Keyboard::Key::Escape;
    case sf::Keyboard::Backspace: return OctaneGUI::Keyboard::Key::Backspace;
    case sf::Keyboard::Delete: return OctaneGUI::Keyboard::Key::Delete;
    case sf::Keyboard::Left: return OctaneGUI::Keyboard::Key::Left;
    case sf::Keyboard::Right: return OctaneGUI::Keyboard::Key::Right;
    case sf::Keyboard::Up: return OctaneGUI::Keyboard::Key::Up;
    case sf::Keyboard::Down: return OctaneGUI::Keyboard::Key::Down;
    case sf::Keyboard::Home: return OctaneGUI::Keyboard::Key::Home;
    case sf::Keyboard::End: return OctaneGUI::Keyboard::Key::End;
    case sf::Keyboard::LShift: return OctaneGUI::Keyboard::Key::LeftShift;
    case sf::Keyboard::RShift: return OctaneGUI::Keyboard::Key::RightShift;
    case sf::Keyboard::LControl: return OctaneGUI::Keyboard::Key::LeftControl;
    case sf::Keyboard::RControl: return OctaneGUI::Keyboard::Key::RightControl;
    case sf::Keyboard::LAlt: return OctaneGUI::Keyboard::Key::LeftAlt;
    case sf::Keyboard::RAlt: return OctaneGUI::Keyboard::Key::RightAlt;
    case sf::Keyboard::Enter: return OctaneGUI::Keyboard::Key::Enter;
    case sf::Keyboard::Tilde: return OctaneGUI::Keyboard::Key::Tilde;
    case sf::Keyboard::Tab: return OctaneGUI::Keyboard::Key::Tab;
    default: break;
    }

    return OctaneGUI::Keyboard::Key::None;
}

OctaneGUI::Mouse::Button GetMouseButton(sf::Mouse::Button Button)
{
    switch (Button)
    {
    case sf::Mouse::Button::Right: return OctaneGUI::Mouse::Button::Right;
    case sf::Mouse::Button::Middle: return OctaneGUI::Mouse::Button::Middle;
    default: break;
    }

    return OctaneGUI::Mouse::Button::Left;
}

sf::Cursor::Type GetCursorType(OctaneGUI::Mouse::Cursor Cursor)
{
    switch (Cursor)
    {
    case OctaneGUI::Mouse::Cursor::IBeam: return sf::Cursor::Text;
    case OctaneGUI::Mouse::Cursor::SizeWE: return sf::Cursor::SizeHorizontal;
    case OctaneGUI::Mouse::Cursor::SizeNS: return sf::Cursor::SizeVertical;
    case OctaneGUI::Mouse::Cursor::Arrow:
    default: break;
    }

    return sf::Cursor::Arrow;
}

const sf::Cursor& GetCursor(sf::Cursor::Type Type)
{
    if (g_Cursors.find(Type) == g_Cursors.end())
    {
        g_Cursors[Type].loadFromSystem(Type);
    }

    return g_Cursors[Type];
}

void SetMouseCursor(const std::shared_ptr<sf::RenderWindow>& Window, sf::Cursor::Type Type)
{
    Window->setMouseCursor(GetCursor(Type));
}

bool Initialize()
{
    return true;
}

void NewWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        unsigned int Style = sf::Style::None;
        if (!Window->CustomTitleBar())
        {
            Style |= sf::Style::Titlebar | sf::Style::Close;

            if (Window->IsResizable())
            {
                Style |= sf::Style::Resize;
            }
        }

        const OctaneGUI::Vector2 Size = Window->GetSize();
        sf::RenderWindow* RenderWindow = new sf::RenderWindow(sf::VideoMode((int)Size.X, (int)Size.Y), Window->GetTitle(), Style);
        RenderWindow->setFramerateLimit(0);
        RenderWindow->setVerticalSyncEnabled(false);
        g_Windows[Window] = std::shared_ptr<sf::RenderWindow>(RenderWindow);

#if defined(WINDOWS)
        if (Window->Modal())
        {
            Windows::SetAlwaysOnTop((void*)RenderWindow->getSystemHandle());
        }
#endif

        ShowMinimize((void*)RenderWindow->getSystemHandle(), Window->CanMinimize());

        sf::VideoMode Desktop = sf::VideoMode::getDesktopMode();
        Window->SetDeviceSize({ (float)Desktop.width, (float)Desktop.height });
        Window->SetPosition({ (float)RenderWindow->getPosition().x, (float)RenderWindow->getPosition().y });

        SetOnHitTest((void*)RenderWindow->getSystemHandle(), [](void* Handle, const OctaneGUI::Vector2& Point) -> HitTestResult
            {
                HitTestResult Result = HitTestResult::Normal;

                for (std::unordered_map<OctaneGUI::Window*, std::shared_ptr<sf::RenderWindow>>::const_iterator It = g_Windows.begin(); It != g_Windows.end(); ++It)
                {
                    if ((void*)It->second->getSystemHandle() == Handle)
                    {
                        if (It->first->GetRootContainer()->IsInTitleBar(Point))
                        {
                            Result = HitTestResult::Draggable;
                        }
                        break;
                    }
                }

                return Result;
            });
    }
}

void DestroyWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    g_Windows[Window]->close();
    g_Windows.erase(Window);
}

void RaiseWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    g_Windows[Window]->requestFocus();
}

void MinimizeWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    Minimize((void*)g_Windows[Window]->getSystemHandle());
}

void MaximizeWindow(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

#if defined(WINDOWS)
    void* Handle = g_Windows[Window]->getSystemHandle();
    if (Window->IsMaximized())
    {
        Windows::RestoreWindow(Handle);
    }
    else
    {
        Windows::MaximizeWindow(Handle);
    }
#endif
}

void ToggleWindow(OctaneGUI::Window* Window, bool Enable)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    const std::shared_ptr<sf::RenderWindow>& RenderWindow = g_Windows[Window];

#if defined(WINDOWS)
    Windows::SetEnabled((void*)RenderWindow->getSystemHandle(), Enable);
#endif

    if (Enable)
    {
        Windowing::Focus((void*)RenderWindow->getSystemHandle());
    }
}

void NewFrame()
{
}

OctaneGUI::Event Event(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return OctaneGUI::Event(OctaneGUI::Event::Type::None);
    }

    const std::shared_ptr<sf::RenderWindow>& RenderWindow = g_Windows[Window];

    sf::Event Event;
    if (RenderWindow->pollEvent(Event))
    {
        switch (Event.type)
        {
        case sf::Event::Closed:
            return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);

        case sf::Event::KeyPressed:
            return OctaneGUI::Event(
                OctaneGUI::Event::Type::KeyPressed,
                OctaneGUI::Event::Key(GetKeyCode(Event.key.code)));

        case sf::Event::KeyReleased:
            return OctaneGUI::Event(
                OctaneGUI::Event::Type::KeyReleased,
                OctaneGUI::Event::Key(GetKeyCode(Event.key.code)));

        case sf::Event::MouseMoved:
            return OctaneGUI::Event(OctaneGUI::Event::MouseMove((float)Event.mouseMove.x, (float)Event.mouseMove.y));

        case sf::Event::MouseButtonPressed:
        {
            const bool IncClick = g_MouseButtonClock.MeasureMS() <= MULTI_CLICK_TIME_MS;
            const OctaneGUI::Mouse::Button Button = GetMouseButton(Event.mouseButton.button);

            uint8_t& Clicks = g_MouseClicks[Button];
            Clicks = IncClick ? Clicks + 1 : 1;
            Clicks = Clicks > 3 ? 1 : Clicks;

            OctaneGUI::Mouse::Count Count = OctaneGUI::Mouse::Count::Single;
            if (Clicks == 2)
            {
                Count = OctaneGUI::Mouse::Count::Double;
            }
            else if (Clicks == 3)
            {
                Count = OctaneGUI::Mouse::Count::Triple;
            }

            g_MouseButtonClock.Reset();
            return OctaneGUI::Event(
                OctaneGUI::Event::Type::MousePressed,
                OctaneGUI::Event::MouseButton(Button, (float)Event.mouseButton.x, (float)Event.mouseButton.y, Count));
        }

        case sf::Event::MouseButtonReleased:
            return OctaneGUI::Event(
                OctaneGUI::Event::Type::MouseReleased,
                OctaneGUI::Event::MouseButton(GetMouseButton(Event.mouseButton.button), (float)Event.mouseButton.x, (float)Event.mouseButton.y, OctaneGUI::Mouse::Count::Single));

        case sf::Event::MouseWheelScrolled:
        {
            if (Event.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel)
            {
                return OctaneGUI::Event(
                    OctaneGUI::Event::MouseWheel((int)Event.mouseWheelScroll.delta, 0));
            }

            return OctaneGUI::Event(
                OctaneGUI::Event::MouseWheel(0, (int)Event.mouseWheelScroll.delta));
        }

        case sf::Event::TextEntered:
        {
            // Ignore tab characters when using SFML. The 'sf::Keyboard::Key::Tab' will capture these characters.
            if (Event.text.unicode == '\t')
            {
                break;
            }

            return OctaneGUI::Event(OctaneGUI::Event::Text(Event.text.unicode));
        }

        case sf::Event::Resized:
            RenderWindow->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)Event.size.width, (float)Event.size.height)));
            return OctaneGUI::Event(OctaneGUI::Event::WindowResized((float)Event.size.width, (float)Event.size.height));

        case sf::Event::MouseEntered:
            return OctaneGUI::Event(OctaneGUI::Event::Type::WindowEnter);

        case sf::Event::MouseLeft:
            return OctaneGUI::Event(OctaneGUI::Event::Type::WindowLeave);

        default: break;
        }
    }

    return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

void Exit()
{
    for (const std::pair<OctaneGUI::Window*, std::shared_ptr<sf::RenderWindow>>& Item : g_Windows)
    {
        Item.second->close();
    }

    g_Windows.clear();
}

void SetClipboardContents(const std::u32string& Contents)
{
    sf::String String;

    for (const char32_t Ch : Contents)
    {
        String += (sf::Uint32)Ch;
    }

    sf::Clipboard::setString(String);
}

std::u32string GetClipboardContents()
{
    std::u32string Result;

    for (const sf::Uint32 Ch : sf::Clipboard::getString().toUtf32())
    {
        Result += Ch;
    }

    return Result;
}

void SetWindowTitle(OctaneGUI::Window* Window, const char* Title)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    g_Windows[Window]->setTitle(Title);
}

void SetWindowPosition(OctaneGUI::Window* Window)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    g_Windows[Window]->setPosition({ (int)Window->GetPosition().X, (int)Window->GetPosition().Y });
}

void SetMouseCursor(OctaneGUI::Window* Window, OctaneGUI::Mouse::Cursor Cursor)
{
    if (g_Windows.find(Window) == g_Windows.end())
    {
        return;
    }

    SetMouseCursor(g_Windows[Window], GetCursorType(Cursor));
}

const std::shared_ptr<sf::RenderWindow>& Get(OctaneGUI::Window* Window)
{
    return g_Windows[Window];
}

}
}
