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

#include "VertexBuffer.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace OctaneUI
{

struct Rect;
class Texture;
class Theme;

class Paint
{
public:
	Paint();
	Paint(const std::shared_ptr<Theme>& InTheme);
	~Paint();

	void Line(const Vector2& Start, const Vector2& End, const Color& Col, float Thickness = 1.0f);
	void Rectangle(const Rect& Bounds, const Color& Col);
	void RectangleOutline(const Rect& Bounds, const Color& Col, float Thickness = 1.0f);
	void Text(const Vector2& Position, const std::string& Contents, const Color& Col);
	void Image(const Rect& Bounds, const Rect& TexCoords, const std::shared_ptr<Texture>& InTexture, const Color& Col);

	void PushClip(const Rect& Bounds);
	void PopClip();

	const VertexBuffer& GetBuffer() const;
	std::shared_ptr<Theme> GetTheme() const;

private:
	void AddLine(const Vector2& Start, const Vector2& End, const Color& Col, float Thickness, uint32_t IndexOffset = 0);
	void AddTriangles(const Rect& Vertices, const Color& Col, uint32_t IndexOffset = 0);
	void AddTriangles(const Rect& Vertices, const Rect& TexCoords, const Color& Col, uint32_t IndexOffset = 0);
	void AddTriangleIndices(uint32_t Offset);
	DrawCommand& PushCommand(uint32_t IndexCount, uint32_t TextureID);

	std::shared_ptr<Theme> m_Theme;
	std::vector<Rect> m_ClipStack;
	VertexBuffer m_Buffer;
};

}
