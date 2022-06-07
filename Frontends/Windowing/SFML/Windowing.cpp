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

namespace Windowing
{

std::unordered_map<OctaneGUI::Window*, std::shared_ptr<sf::RenderWindow>> s_Windows {};

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
	case sf::Keyboard::Enter: return OctaneGUI::Keyboard::Key::Enter;
	case sf::Keyboard::Tilde: return OctaneGUI::Keyboard::Key::Tilde;
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

bool Initialize()
{
	return true;
}

void CreateWindow(OctaneGUI::Window* Window)
{
	if (s_Windows.find(Window) == s_Windows.end())
	{
		const OctaneGUI::Vector2 Size = Window->GetSize();
		sf::RenderWindow* RenderWindow = new sf::RenderWindow(sf::VideoMode((int)Size.X, (int)Size.Y), Window->GetTitle());
		RenderWindow->setFramerateLimit(0);
		RenderWindow->setVerticalSyncEnabled(false);
		s_Windows[Window] = std::shared_ptr<sf::RenderWindow>(RenderWindow);
	}
}

void DestroyWindow(OctaneGUI::Window* Window)
{
	if (s_Windows.find(Window) == s_Windows.end())
	{
		return;
	}

	s_Windows[Window]->close();
	s_Windows.erase(Window);
}

OctaneGUI::Event Event(OctaneGUI::Window* Window)
{
	if (s_Windows.find(Window) == s_Windows.end())
	{
		return OctaneGUI::Event(OctaneGUI::Event::Type::None);
	}

	const std::shared_ptr<sf::RenderWindow>& RenderWindow = s_Windows[Window];

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
			return OctaneGUI::Event(
				OctaneGUI::Event::Type::MousePressed,
				OctaneGUI::Event::MouseButton(GetMouseButton(Event.mouseButton.button), (float)Event.mouseButton.x, (float)Event.mouseButton.y));

		case sf::Event::MouseButtonReleased:
			return OctaneGUI::Event(
				OctaneGUI::Event::Type::MouseReleased,
				OctaneGUI::Event::MouseButton(GetMouseButton(Event.mouseButton.button), (float)Event.mouseButton.x, (float)Event.mouseButton.y));

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
			return OctaneGUI::Event(OctaneGUI::Event::Text(Event.text.unicode));

		case sf::Event::Resized:
			RenderWindow->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)Event.size.width, (float)Event.size.height)));
			return OctaneGUI::Event(OctaneGUI::Event::WindowResized((float)Event.size.width, (float)Event.size.height));

		default: break;
		}
	}

	return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

void Exit()
{
	for (const std::pair<OctaneGUI::Window*, std::shared_ptr<sf::RenderWindow>>& Item : s_Windows)
	{
		Item.second->close();
	}

	s_Windows.clear();
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
	if (s_Windows.find(Window) == s_Windows.end())
	{
		return;
	}

	s_Windows[Window]->setTitle(Title);
}

const std::shared_ptr<sf::RenderWindow>& Get(OctaneGUI::Window* Window)
{
	return s_Windows[Window];
}

}
