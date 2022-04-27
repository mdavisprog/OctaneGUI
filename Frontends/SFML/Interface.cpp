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

#include "../Interface.h"
#include "OctaneGUI/OctaneGUI.h"
#include "SFML/Graphics.hpp"

#include <unordered_map>

namespace Interface
{

class Container
{
public:
	Container()
	{
	}

	~Container()
	{
	}

	sf::RenderWindow* Renderer;
};

std::unordered_map<OctaneGUI::Window*, Container> Windows;
std::vector<sf::Texture*> Textures;

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

void OnCreateWindow(OctaneGUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		OctaneGUI::Vector2 Size = Window->GetSize();

		Container& Item = Windows[Window];
		Item.Renderer = new sf::RenderWindow(sf::VideoMode((int)Size.X, (int)Size.Y), Window->GetTitle());
		Item.Renderer->setFramerateLimit(0);
		Item.Renderer->setVerticalSyncEnabled(false);
	}
}

void OnDestroyWindow(OctaneGUI::Window* Window)
{
	auto Iter = Windows.find(Window);
	if (Iter == Windows.end())
	{
		return;
	}

	Container& Item = Windows[Window];
	Item.Renderer->close();

	delete Item.Renderer;
	Windows.erase(Iter);
}

OctaneGUI::Event OnEvent(OctaneGUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		return OctaneGUI::Event(OctaneGUI::Event::Type::None);
	}

	Container& Item = Windows[Window];
	sf::Event Event;
	if (Item.Renderer->pollEvent(Event))
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

		case sf::Event::TextEntered:
			return OctaneGUI::Event(OctaneGUI::Event::Text(Event.text.unicode));

		case sf::Event::Resized:
			Item.Renderer->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)Event.size.width, (float)Event.size.height)));
			return OctaneGUI::Event(OctaneGUI::Event::WindowResized((float)Event.size.width, (float)Event.size.height));

		default: break;
		}
	}

	return OctaneGUI::Event(OctaneGUI::Event::Type::None);
}

void OnPaint(OctaneGUI::Window* Window, const OctaneGUI::VertexBuffer& Buffer)
{
	if (Windows.find(Window) == Windows.end())
	{
		return;
	}

	Container& Item = Windows[Window];

	Item.Renderer->clear();

	const std::vector<OctaneGUI::Vertex>& Vertices = Buffer.GetVertices();
	const std::vector<uint32_t>& Indices = Buffer.GetIndices();

	for (const OctaneGUI::DrawCommand& Command : Buffer.Commands())
	{
		sf::VertexArray Array(sf::Triangles, Command.IndexCount());
		sf::Texture* Texture = nullptr;

		if (Command.TextureID() > 0)
		{
			for (sf::Texture* Value : Textures)
			{
				if (Value->getNativeHandle() == Command.TextureID())
				{
					Texture = Value;
					break;
				}
			}
		}

		for (size_t I = 0; I < Command.IndexCount(); I++)
		{
			uint32_t Index = Indices[I + Command.IndexOffset()];
			const OctaneGUI::Vertex& Vertex = Vertices[Index + Command.VertexOffset()];
			OctaneGUI::Vector2 Position = Vertex.Position;
			OctaneGUI::Vector2 TexCoords = Vertex.TexCoords;
			OctaneGUI::Color Color = Vertex.Col;

			if (Texture != nullptr)
			{
				// Convert from normalized coordintaes to pixel space.
				TexCoords.X *= Texture->getSize().x;
				TexCoords.Y *= Texture->getSize().y;
			}

			Array[I].position = sf::Vector2f(Position.X, Position.Y);
			Array[I].texCoords = sf::Vector2f(TexCoords.X, TexCoords.Y);
			Array[I].color = sf::Color(Color.R, Color.G, Color.B, Color.A);
		}

		sf::RenderStates RenderStates;
		RenderStates.texture = Texture;

		const OctaneGUI::Rect Clip = Command.Clip();
		if (!Clip.IsZero())
		{
			const sf::Vector2f Size((float)Item.Renderer->getSize().x, (float)Item.Renderer->getSize().y);
			const OctaneGUI::Vector2 ClipSize = Clip.GetSize();
			sf::View View(sf::FloatRect(Clip.Min.X, Clip.Min.Y, ClipSize.X, ClipSize.Y));
			View.setViewport(sf::FloatRect(Clip.Min.X / Size.x, Clip.Min.Y / Size.y, ClipSize.X / Size.x, ClipSize.Y / Size.y));
			Item.Renderer->setView(View);
		}

		Item.Renderer->draw(Array, RenderStates);
		Item.Renderer->setView(sf::View(sf::FloatRect(0.0f, 0.0f, Item.Renderer->getSize().x, Item.Renderer->getSize().y)));
	}

	Item.Renderer->display();
}

uint32_t OnLoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
	sf::Texture* Texture = new sf::Texture();
	Texture->setSmooth(true);
	Texture->create(Width, Height);
	Texture->update(Data.data(), Width, Height, 0, 0);
	Textures.push_back(Texture);
	return Texture->getNativeHandle();
}

void OnExit()
{
	for (sf::Texture* Texture : Textures)
	{
		delete Texture;
	}

	Textures.clear();

	for (auto Iter : Windows)
	{
		Container& Item = Iter.second;
		Item.Renderer->close();
		delete Item.Renderer;
	}

	Windows.clear();
}

std::u32string OnGetClipboardContents()
{
	std::u32string Result;

	for (const sf::Uint32 Ch : sf::Clipboard::getString().toUtf32())
	{
		Result += Ch;
	}

	return Result;
}

void Initialize(OctaneGUI::Application& Application)
{
	Application
		.SetOnCreateWindow(OnCreateWindow)
		.SetOnDestroyWindow(OnDestroyWindow)
		.SetOnPaint(OnPaint)
		.SetOnEvent(OnEvent)
		.SetOnLoadTexture(OnLoadTexture)
		.SetOnExit(OnExit)
		.SetOnGetClipboardContents(OnGetClipboardContents);
}

}
