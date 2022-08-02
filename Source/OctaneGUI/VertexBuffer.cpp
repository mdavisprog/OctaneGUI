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

#include "VertexBuffer.h"

namespace OctaneGUI
{

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::AddVertex(const Vector2& Point, const Color& Col)
{
    m_Vertices.emplace_back(Point, Col);
}

void VertexBuffer::AddVertex(const Vector2& Point, const Vector2& TexCoords, const Color& Col)
{
    m_Vertices.emplace_back(Point, TexCoords, Col);
}

void VertexBuffer::AddVertices(const std::vector<Vector2>& Points, const Color& Col)
{
    const size_t Index = m_Vertices.size();
    m_Vertices.resize(m_Vertices.size() + Points.size());

    for (size_t I = 0; I < Points.size(); I++)
    {
        m_Vertices[Index + I].Position = Points[I];
        m_Vertices[Index + I].Col = Col;
    }
}

void VertexBuffer::AddIndex(uint32_t Index)
{
    m_Indices.push_back(Index);
}

const std::vector<Vertex>& VertexBuffer::GetVertices() const
{
    return m_Vertices;
}

const std::vector<uint32_t>& VertexBuffer::GetIndices() const
{
    return m_Indices;
}

uint32_t VertexBuffer::GetVertexCount() const
{
    return m_Vertices.size();
}

uint32_t VertexBuffer::GetIndexCount() const
{
    return m_Indices.size();
}

DrawCommand& VertexBuffer::PushCommand(uint32_t IndexCount, uint32_t TextureID, Rect Clip)
{
    m_Commands.emplace_back((uint32_t)m_Vertices.size(), (uint32_t)m_Indices.size(), IndexCount, TextureID, Clip);
    return m_Commands.back();
}

const std::vector<DrawCommand>& VertexBuffer::Commands() const
{
    return m_Commands;
}

}
