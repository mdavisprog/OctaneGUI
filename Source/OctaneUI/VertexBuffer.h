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

#pragma once

#include "DrawCommand.h"
#include "Vertex.h"

#include <vector>

namespace OctaneUI
{

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void AddVertex(const Vector2& Point, const Color& Col);
	void AddVertex(const Vector2& Point, const Vector2& TexCoords, const Color& Col);
	void AddIndex(uint32_t Index);

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;

	uint32_t GetVertexCount() const;
	uint32_t GetIndexCount() const;

	DrawCommand& PushCommand(uint32_t IndexCount, uint32_t TextureID, Rect Clip);
	const std::vector<DrawCommand>& Commands() const;

private:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<DrawCommand> m_Commands;
};

}
