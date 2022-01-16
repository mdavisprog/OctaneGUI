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

#include "Interface.h"
#include "SFML/Graphics.hpp"
#include "OctaneUI/OctaneUI.h"

#include <unordered_map>

namespace SFML
{

class Container
{
public:
	Container()
	{}

	~Container()
	{}

	sf::RenderWindow* Renderer;
	sf::RenderTexture* BackBuffer;
};

std::unordered_map<OctaneUI::Window*, Container> Windows;
std::vector<sf::Texture*> Textures;

OctaneUI::Keyboard::Key GetKeyCode(sf::Keyboard::Key Key)
{
	switch (Key)
	{
	case sf::Keyboard::Backspace: return OctaneUI::Keyboard::Key::Backspace;
	case sf::Keyboard::Left: return OctaneUI::Keyboard::Key::Left;
	case sf::Keyboard::Right: return OctaneUI::Keyboard::Key::Right;
	default: break;
	}

	return OctaneUI::Keyboard::Key::None;
}

OctaneUI::Mouse::Button GetMouseButton(sf::Mouse::Button Button)
{
	switch (Button)
	{
	case sf::Mouse::Button::Right: return OctaneUI::Mouse::Button::Right;
	case sf::Mouse::Button::Middle: return OctaneUI::Mouse::Button::Middle;
	default: break;
	}

	return OctaneUI::Mouse::Button::Left;
}

void OnCreateWindow(OctaneUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		OctaneUI::Vector2 Size = Window->GetSize();

		Container& Item = Windows[Window];
		Item.Renderer = new sf::RenderWindow(sf::VideoMode((int)Size.X, (int)Size.Y), Window->GetTitle());
		Item.Renderer->setFramerateLimit(0);
		Item.Renderer->setVerticalSyncEnabled(false);

		Item.BackBuffer = new sf::RenderTexture();
		Item.BackBuffer->create((uint32_t)Size.X, (uint32_t)Size.Y);
	}
}

void OnDestroyWindow(OctaneUI::Window* Window)
{
	auto Iter = Windows.find(Window);
	if (Iter == Windows.end())
	{
		return;
	}

	Container& Item = Windows[Window];
	Item.Renderer->close();

	delete Item.Renderer;
	delete Item.BackBuffer;
	Windows.erase(Iter);
}

OctaneUI::Event OnEvent(OctaneUI::Window* Window)
{
	if (Windows.find(Window) == Windows.end())
	{
		return OctaneUI::Event(OctaneUI::Event::Type::None);
	}

	Container& Item = Windows[Window];
	sf::Event Event;
	if (Item.Renderer->pollEvent(Event))
	{
		switch (Event.type)
		{
		case sf::Event::Closed:
			return OctaneUI::Event(OctaneUI::Event::Type::WindowClosed);
		
		case sf::Event::KeyPressed:
			return OctaneUI::Event(
				OctaneUI::Event::Type::KeyPressed,
				OctaneUI::Event::Key(GetKeyCode(Event.key.code))
			);
		
		case sf::Event::KeyReleased:
			return OctaneUI::Event(
				OctaneUI::Event::Type::KeyReleased,
				OctaneUI::Event::Key(GetKeyCode(Event.key.code))
			);
		
		case sf::Event::MouseMoved:
			return OctaneUI::Event(OctaneUI::Event::MouseMove((float)Event.mouseMove.x, (float)Event.mouseMove.y));
		
		case sf::Event::MouseButtonPressed:
			return OctaneUI::Event(
				OctaneUI::Event::Type::MousePressed,
				OctaneUI::Event::MouseButton(GetMouseButton(Event.mouseButton.button), (float)Event.mouseButton.x, (float)Event.mouseButton.y)
			);
		
		case sf::Event::MouseButtonReleased:
			return OctaneUI::Event(
				OctaneUI::Event::Type::MouseReleased,
				OctaneUI::Event::MouseButton(GetMouseButton(Event.mouseButton.button), (float)Event.mouseButton.x, (float)Event.mouseButton.y)
			);
		
		case sf::Event::TextEntered:
			return OctaneUI::Event(OctaneUI::Event::Text(Event.text.unicode));
		
		case sf::Event::Resized:
			Item.Renderer->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)Event.size.width, (float)Event.size.height)));
			return OctaneUI::Event(OctaneUI::Event::WindowResized((float)Event.size.width, (float)Event.size.height));

		default: break;
		}
	}

	return OctaneUI::Event(OctaneUI::Event::Type::None);
}

void OnPaint(OctaneUI::Window* Window, const std::vector<OctaneUI::VertexBuffer>& Buffers)
{
	if (Windows.find(Window) == Windows.end())
	{
		return;
	}

	Container& Item = Windows[Window];

	Item.Renderer->clear();
	for (const OctaneUI::VertexBuffer& Buffer : Buffers)
	{
		const std::vector<OctaneUI::Vertex>& Vertices = Buffer.GetVertices();
		const std::vector<uint32_t>& Indices = Buffer.GetIndices();
		sf::VertexArray Array(sf::Triangles, Indices.size());

		for (size_t I = 0; I < Indices.size(); I++)
		{
			uint32_t Index = Indices[I];
			const OctaneUI::Vertex& Vertex = Vertices[Index];
			OctaneUI::Vector2 Position = Vertex.Position;
			OctaneUI::Vector2 TexCoords = Vertex.TexCoords;
			OctaneUI::Color Color = Vertex.Col;
			Array[I].position = sf::Vector2f(Position.X, Position.Y);
			Array[I].texCoords = sf::Vector2f(TexCoords.X, TexCoords.Y);
			Array[I].color = sf::Color(Color.R, Color.G, Color.B, Color.A);
		}

		sf::RenderStates RenderStates;

		if (Buffer.GetTextureID() > 0)
		{
			for (const sf::Texture* Texture : Textures)
			{
				if (Texture->getNativeHandle() == Buffer.GetTextureID())
				{
					RenderStates.texture = Texture;
					break;
				}
			}
		}

		const OctaneUI::Rect Clip = Buffer.GetClip();
		const OctaneUI::Vector2 ClipSize = Clip.GetSize();
		if (!ClipSize.IsZero())
		{
			sf::RenderTexture* BackBuffer = Item.BackBuffer;

			BackBuffer->clear(sf::Color::Transparent);
			BackBuffer->draw(Array, RenderStates);
			BackBuffer->display();

			sf::Sprite Sprite(BackBuffer->getTexture());
			Sprite.setTextureRect(sf::IntRect((int)Clip.Min.X, (int)Clip.Min.Y, (int)ClipSize.X, (int)ClipSize.Y));
			Sprite.setPosition(Clip.Min.X, Clip.Min.Y);
			Item.Renderer->draw(Sprite, RenderStates);
		}
		else
		{
			Item.Renderer->draw(Array, RenderStates);
		}
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
		delete Item.BackBuffer;
	}

	Windows.clear();
}

void Initialize(OctaneUI::Application& Application)
{
	Application
		.SetOnCreateWindow(OnCreateWindow)
		.SetOnDestroyWindow(OnDestroyWindow)
		.SetOnPaint(OnPaint)
		.SetOnEvent(OnEvent)
		.SetOnLoadTexture(OnLoadTexture)
		.SetOnExit(OnExit);
}

}
