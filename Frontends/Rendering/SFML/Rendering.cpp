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

#include "../Rendering.h"
#include "../../Windowing/SFML/Interface.h"
#include "OctaneGUI/OctaneGUI.h"
#include "SFML/Graphics.hpp"

#include <vector>

namespace Rendering
{

std::vector<std::unique_ptr<sf::Texture>> g_Textures;

void Initialize()
{
}

void CreateRenderer(OctaneGUI::Window* Window)
{
}

void DestroyRenderer(OctaneGUI::Window* Window)
{
}

void Paint(OctaneGUI::Window* Window, const OctaneGUI::VertexBuffer& Buffer)
{
    const std::shared_ptr<sf::RenderWindow>& RenderWindow = Windowing::Get(Window);

    RenderWindow->clear();

    const std::vector<OctaneGUI::Vertex>& Vertices = Buffer.GetVertices();
    const std::vector<uint32_t>& Indices = Buffer.GetIndices();

    for (const OctaneGUI::DrawCommand& Command : Buffer.Commands())
    {
        sf::VertexArray Array(sf::Triangles, Command.IndexCount());
        sf::Texture* Texture = nullptr;

        if (Command.TextureID() > 0)
        {
            for (const std::unique_ptr<sf::Texture>& Value : g_Textures)
            {
                if (Value->getNativeHandle() == Command.TextureID())
                {
                    Texture = Value.get();
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

        const sf::Vector2u WindowSize = RenderWindow->getSize();
        const OctaneGUI::Rect Clip = Command.Clip();
        if (!Clip.IsZero())
        {
            const sf::Vector2f Size((float)WindowSize.x, (float)WindowSize.y);
            const OctaneGUI::Vector2 ClipSize = Clip.GetSize();
            sf::View View(sf::FloatRect(Clip.Min.X, Clip.Min.Y, ClipSize.X, ClipSize.Y));
            View.setViewport(sf::FloatRect(Clip.Min.X / Size.x, Clip.Min.Y / Size.y, ClipSize.X / Size.x, ClipSize.Y / Size.y));
            RenderWindow->setView(View);
        }

        RenderWindow->draw(Array, RenderStates);
        RenderWindow->setView(sf::View(sf::FloatRect(0.0f, 0.0f, WindowSize.x, WindowSize.y)));
    }

    RenderWindow->display();
}

uint32_t LoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
    std::unique_ptr<sf::Texture> Texture = std::unique_ptr<sf::Texture>(new sf::Texture());
    Texture->setSmooth(true);
    Texture->create(Width, Height);
    Texture->update(Data.data(), Width, Height, 0, 0);
    const uint32_t Result = Texture->getNativeHandle();
    g_Textures.push_back(std::move(Texture));
    return Result;
}

void Exit()
{
    g_Textures.clear();
}

}
