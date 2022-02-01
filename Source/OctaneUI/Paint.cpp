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

#include "Application.h"
#include "Font.h"
#include "Paint.h"
#include "Rect.h"
#include "Texture.h"
#include "Theme.h"

namespace OctaneUI
{

Paint::Paint()
	: m_Theme(nullptr)
{
}

Paint::Paint(const std::shared_ptr<Theme>& InTheme)
	: m_Theme(InTheme)
{
}

Paint::~Paint()
{
}

void Paint::Line(const Vector2& Start, const Vector2& End, const Color& Col, float Thickness)
{
	VertexBuffer& Buffer = CreateBuffer();
	AddLine(Start, End, Col, Thickness, Buffer);
}

void Paint::Rectangle(const Rect& Bounds, const Color& Col)
{
	VertexBuffer& Buffer = CreateBuffer();
	AddTriangles(Bounds, Col, Buffer);
}

void Paint::RectangleOutline(const Rect& Bounds, const Color& Col, float Thickness)
{
	const Vector2 TopRight(Bounds.Min + Vector2(Bounds.GetSize().X, 0.0f));
	const Vector2 BottomLeft(Bounds.Min + Vector2(0.0f, Bounds.GetSize().Y));

	VertexBuffer& Buffer = CreateBuffer();
	AddLine(Bounds.Min, TopRight, Col, Thickness, Buffer);
	AddLine(TopRight, Bounds.Max, Col, Thickness, Buffer);
	AddLine(Bounds.Max, BottomLeft, Col, Thickness, Buffer);
	AddLine(BottomLeft, Bounds.Min, Col, Thickness, Buffer);
}

void Paint::Text(const Vector2& Position, const std::string& Contents, const Color& Col)
{
	std::shared_ptr<Font> ThemeFont = m_Theme ? m_Theme->GetFont() : nullptr;
	if (!ThemeFont)
	{
		return;
	}

	VertexBuffer& Buffer = CreateBuffer();
	Buffer.SetTextureID(ThemeFont->GetID());

	Vector2 Pos = Position;
	for (char Char : Contents)
	{
		Rect Vertices;
		Rect TexCoords;

		ThemeFont->Draw((int32_t)Char - 32, Pos, Vertices, TexCoords);
		AddTriangles(Vertices, TexCoords, Col, Buffer);
		Buffer.IncOffset(4);
	}
}

void Paint::Image(const Rect& Bounds, const Rect& TexCoords, const std::shared_ptr<Texture>& InTexture, const Color& Col)
{
	if (!InTexture)
	{
		return;
	}

	VertexBuffer& Buffer = CreateBuffer();
	Buffer.SetTextureID(InTexture->GetID());

	AddTriangles(Bounds, TexCoords, Col, Buffer);
}

void Paint::PushClip(const Rect& Bounds, const Vector2& Offset)
{
	m_ClipStack.emplace_back(Bounds, Offset);
}

void Paint::PopClip()
{
	m_ClipStack.pop_back();
}

const std::vector<VertexBuffer>& Paint::GetBuffers() const
{
	return m_Buffers;
}

std::shared_ptr<Theme> Paint::GetTheme() const
{
	return m_Theme;
}

void Paint::AddLine(const Vector2& Start, const Vector2& End, const Color& Col, float Thickness, VertexBuffer& Buffer) const
{
	const Vector2 Direction = (End - Start).Unit();
	const float HalfThickness = Thickness * 0.5f;

	Buffer.AddVertex(Start + Vector2(-Direction.Y, Direction.X) * HalfThickness, Col);
	Buffer.AddVertex(End + Vector2(-Direction.Y, Direction.X) * HalfThickness, Col);
	Buffer.AddVertex(End + Vector2(Direction.Y, -Direction.X) * HalfThickness, Col);
	Buffer.AddVertex(Start + Vector2(Direction.Y, -Direction.X) * HalfThickness, Col);

	AddTriangleIndices(Buffer);

	Buffer.IncOffset(4);
}

void Paint::AddTriangles(const Rect& Vertices, const Color& Col, VertexBuffer& Buffer) const
{
	Buffer.AddVertex(Vertices.Min, Col);
	Buffer.AddVertex(Vector2(Vertices.Max.X, Vertices.Min.Y), Col);
	Buffer.AddVertex(Vertices.Max, Col);
	Buffer.AddVertex(Vector2(Vertices.Min.X, Vertices.Max.Y), Col);

	AddTriangleIndices(Buffer);
}

void Paint::AddTriangles(const Rect& Vertices, const Rect& TexCoords, const Color& Col, VertexBuffer& Buffer) const
{
	Buffer.AddVertex(Vertices.Min, TexCoords.Min, Col);
	Buffer.AddVertex(Vector2(Vertices.Max.X, Vertices.Min.Y), Vector2(TexCoords.Max.X, TexCoords.Min.Y), Col);
	Buffer.AddVertex(Vertices.Max, TexCoords.Max, Col);
	Buffer.AddVertex(Vector2(Vertices.Min.X, Vertices.Max.Y), Vector2(TexCoords.Min.X, TexCoords.Max.Y), Col);

	AddTriangleIndices(Buffer);
}

void Paint::AddTriangleIndices(VertexBuffer& Buffer) const
{
	const uint32_t Offset = Buffer.GetOffset();

	Buffer.AddIndex(Offset);
	Buffer.AddIndex(Offset + 1);
	Buffer.AddIndex(Offset + 2);
	Buffer.AddIndex(Offset);
	Buffer.AddIndex(Offset + 2);
	Buffer.AddIndex(Offset + 3);
}

VertexBuffer& Paint::CreateBuffer()
{
	m_Buffers.emplace_back();
	VertexBuffer& Buffer = m_Buffers[m_Buffers.size() - 1];

	if (!m_ClipStack.empty())
	{
		Buffer.SetClip(m_ClipStack.back());
	}

	return Buffer;
}

}
