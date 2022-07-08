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

#include <unordered_map>

namespace Windowing
{

std::unordered_map<OctaneGUI::Window*, SDL_Window*> g_Windows;

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

bool Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void CreateWindow(OctaneGUI::Window* Window)
{
	if (g_Windows.find(Window) == g_Windows.end())
	{
		int Flags = SDL_WINDOW_ALLOW_HIGHDPI;

		if (Window->IsResizable())
		{
			Flags |= SDL_WINDOW_RESIZABLE;
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

		g_Windows[Window] = Instance;
	}
}

void DestroyWindow(OctaneGUI::Window* Window)
{
	if (g_Windows.find(Window) == g_Windows.end())
	{
		return;
	}

	SDL_DestroyWindow(g_Windows[Window]);
	g_Windows.erase(Window);
}

OctaneGUI::Event Event(OctaneGUI::Window* Window)
{
	if (g_Windows.find(Window) == g_Windows.end())
	{
		return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);
	}

	std::vector<SDL_Event> EventsToPush;
	SDL_Window* Instance = g_Windows[Window];
	const uint32_t WindowID = SDL_GetWindowID(Instance);
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_QUIT: return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);

		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			if (WindowID != Event.key.windowID)
			{
				EventsToPush.push_back(Event);
				break;
			}

			return OctaneGUI::Event(
				Event.key.type == SDL_KEYDOWN ? OctaneGUI::Event::Type::KeyPressed : OctaneGUI::Event::Type::KeyReleased,
				OctaneGUI::Event::Key(GetKey(Event.key.keysym.sym)));
		}

		case SDL_MOUSEMOTION:
		{
			if (WindowID != Event.motion.windowID)
			{
				EventsToPush.push_back(Event);
				break;
			}

			return OctaneGUI::Event(
				OctaneGUI::Event::MouseMove(Event.motion.x, Event.motion.y));
		}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			if (WindowID != Event.button.windowID)
			{
				EventsToPush.push_back(Event);
				break;
			}

			return OctaneGUI::Event(
				Event.button.type == SDL_MOUSEBUTTONDOWN ? OctaneGUI::Event::Type::MousePressed : OctaneGUI::Event::Type::MouseReleased,
				OctaneGUI::Event::MouseButton(GetMouseButton(Event.button.button), (float)Event.button.x, (float)Event.button.y));
		}

		case SDL_MOUSEWHEEL:
		{
			if (WindowID != Event.wheel.windowID)
			{
				EventsToPush.push_back(Event);
				break;
			}

			return OctaneGUI::Event(
				OctaneGUI::Event::MouseWheel(Event.wheel.x, Event.wheel.y));
		}

		case SDL_TEXTINPUT:
		{
			if (WindowID != Event.text.windowID)
			{
				EventsToPush.push_back(Event);
				break;
			}

			return OctaneGUI::Event(
				OctaneGUI::Event::Text(*(uint32_t*)Event.text.text));
		}

		case SDL_WINDOWEVENT:
		{
			if (WindowID != Event.window.windowID)
			{
				EventsToPush.push_back(Event);
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
	}

	for (SDL_Event& Value : EventsToPush)
	{
		SDL_PushEvent(&Value);
	}

	return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

void Exit()
{
	for (const std::pair<OctaneGUI::Window*, SDL_Window*>& Item : g_Windows)
	{
		SDL_DestroyWindow(Item.second);
	}

	g_Windows.clear();
	SDL_Quit();
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

SDL_Window* Get(OctaneGUI::Window* Window)
{
	if (g_Windows.find(Window) == g_Windows.end())
	{
		return nullptr;
	}

	return g_Windows[Window];
}

}
