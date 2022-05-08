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

#include "Paint.h"
#include "Application.h"
#include "Font.h"
#include "Rect.h"
#include "Texture.h"
#include "Theme.h"

#include <string_view>

namespace OctaneGUI
{

Paint::Paint()
	: m_Buffer()
	, m_Theme(nullptr)
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
	PushCommand(6, 0);
	AddLine(Start, End, Col, Thickness);
}

void Paint::Rectangle(const Rect& Bounds, const Color& Col)
{
	PushCommand(6, 0);
	AddTriangles(Bounds, Col);
}

void Paint::Rectangle3D(const Rect& Bounds, const Color& Base, const Color& Highlight, const Color& Shadow, bool Sunken)
{
	Rectangle(Bounds, Base);

	const float Thickness = Sunken ? 1.0f : 1.0f;
	const float HThickness = Thickness * 0.5f;

	const Vector2 Min = Bounds.Min + Vector2(HThickness, HThickness);
	Vector2 TR = Min + Vector2(Bounds.Width() - HThickness, 0.0f);
	Vector2 BL = Min + Vector2(0.0f, Bounds.Height() - HThickness);
	Line(Min, TR, Sunken ? Shadow : Highlight, Thickness);
	Line(Min, BL, Sunken ? Shadow : Highlight, Thickness);

	const Vector2 Max = Bounds.Max - Vector2(HThickness, HThickness);
	TR = Max - Vector2(0.0f, Bounds.Height() - HThickness);
	BL = Max - Vector2(Bounds.Width() - HThickness, 0.0f);
	Line(Max, TR, Sunken ? Highlight : Shadow, Thickness);
	Line(Max, BL, Sunken ? Highlight : Shadow, Thickness);
}

void Paint::RectangleOutline(const Rect& Bounds, const Color& Col, float Thickness)
{
	const Vector2 TopRight(Bounds.Min + Vector2(Bounds.GetSize().X, 0.0f));
	const Vector2 BottomLeft(Bounds.Min + Vector2(0.0f, Bounds.GetSize().Y));

	PushCommand(6 * 4, 0);
	AddLine(Bounds.Min, TopRight, Col, Thickness, 0);
	AddLine(TopRight, Bounds.Max, Col, Thickness, 4);
	AddLine(Bounds.Max, BottomLeft, Col, Thickness, 8);
	AddLine(BottomLeft, Bounds.Min, Col, Thickness, 12);
}

void Paint::Text(const std::shared_ptr<Font>& InFont, const Vector2& Position, const std::u32string& Contents, const Color& Col)
{
	if (Contents.empty())
	{
		return;
	}

	Rect Clip;
	if (!m_ClipStack.empty())
	{
		Clip = m_ClipStack.back();
	}

	std::vector<Rect> GlyphRects;
	std::vector<Rect> GlyphUVs;
	Vector2 Pos = Position;
	for (char32_t Char : Contents)
	{
		if (Char == '\n')
		{
			Pos.X = Position.X;
			Pos.Y += InFont->Size();
			continue;
		}

		if (!Clip.IsZero())
		{
			// Don't check for < Clip.Min.X since size of glyph is not known here.
			if (Pos.X > Clip.Max.X || Pos.Y > Clip.Max.Y || Pos.Y + InFont->Size() < Clip.Min.Y )
			{
				continue;
			}
		}

		Rect Vertices;
		Rect TexCoords;

		InFont->Draw((uint32_t)Char, Pos, Vertices, TexCoords);

		if (!IsClipped(Vertices))
		{
			GlyphRects.push_back(Vertices);
			GlyphUVs.push_back(TexCoords);
		}
	}

	if (GlyphRects.empty())
	{
		return;
	}

	PushCommand(6 * GlyphRects.size(), InFont->ID());

	uint32_t Offset = 0;
	for (size_t I = 0; I < GlyphRects.size(); I++)
	{
		const Rect& Vertices = GlyphRects[I];
		const Rect& TexCoords = GlyphUVs[I];

		AddTriangles(Vertices, TexCoords, Col, Offset);
		Offset += 4;
	}
}

void Paint::Textf(const std::shared_ptr<Font>& InFont, const Vector2& Position, const std::u32string& Contents, const std::vector<TextFormat>& Formats)
{
	if (Contents.empty())
	{
		return;
	}

	std::vector<std::u32string_view> Views;
	for (const TextFormat& Item : Formats)
	{
		Views.emplace_back(&Contents[Item.Start], Item.End - Item.Start);
	}

	Rect Clip;
	if (!m_ClipStack.empty())
	{
		Clip = m_ClipStack.back();
	}

	std::vector<Rect> GlyphRects;
	std::vector<Rect> GlyphUVs;
	std::vector<Color> GlyphColors;
	Vector2 Pos = Position;
	for (size_t I = 0; I < Formats.size(); I++)	
	{
		const TextFormat& Format = Formats[I];
		const std::u32string_view& View = Views[I];
		for (char32_t Char : View)
		{
			if (Char == '\n')
			{
				Pos.X = Position.X;
				Pos.Y += InFont->Size();
				continue;
			}

			if (!Clip.IsZero())
			{
				// Don't check for < Clip.Min.X since size of glyph is not known here.
				if (Pos.X > Clip.Max.X || Pos.Y > Clip.Max.Y || Pos.Y + InFont->Size() < Clip.Min.Y )
				{
					continue;
				}
			}

			Rect Vertices;
			Rect TexCoords;

			InFont->Draw((uint32_t)Char, Pos, Vertices, TexCoords);

			if (!IsClipped(Vertices))
			{
				GlyphRects.push_back(Vertices);
				GlyphUVs.push_back(TexCoords);
				GlyphColors.push_back(Format.TextColor);
			}
		}
	}

	if (GlyphRects.empty())
	{
		return;
	}

	PushCommand(6 * GlyphRects.size(), InFont->ID());

	uint32_t Offset = 0;
	for (size_t I = 0; I < GlyphRects.size(); I++)
	{
		const Rect& Vertices = GlyphRects[I];
		const Rect& TexCoords = GlyphUVs[I];
		const Color& TextColor = GlyphColors[I];

		AddTriangles(Vertices, TexCoords, TextColor, Offset);
		Offset += 4;
	}
}

void Paint::Image(const Rect& Bounds, const Rect& TexCoords, const std::shared_ptr<Texture>& InTexture, const Color& Col)
{
	if (!InTexture)
	{
		return;
	}

	PushCommand(6, InTexture->GetID());
	AddTriangles(Bounds, TexCoords, Col);
}

void Paint::PushClip(const Rect& Bounds)
{
	m_ClipStack.push_back(Bounds);
}

void Paint::PopClip()
{
	m_ClipStack.pop_back();
}

bool Paint::IsClipped(const Rect& Bounds) const
{
	if (m_ClipStack.empty())
	{
		return false;
	}

	const Rect Clip = m_ClipStack.back();

	return !(Clip.Intersects(Bounds) || Clip.Encompasses(Bounds));
}

const VertexBuffer& Paint::GetBuffer() const
{
	return m_Buffer;
}

std::shared_ptr<Theme> Paint::GetTheme() const
{
	return m_Theme;
}

void Paint::AddLine(const Vector2& Start, const Vector2& End, const Color& Col, float Thickness, uint32_t Offset)
{
	const Vector2 Direction = (End - Start).Unit();
	const float HalfThickness = Thickness * 0.5f;

	m_Buffer.AddVertex(Start + Vector2(-Direction.Y, Direction.X) * HalfThickness, Col);
	m_Buffer.AddVertex(End + Vector2(-Direction.Y, Direction.X) * HalfThickness, Col);
	m_Buffer.AddVertex(End + Vector2(Direction.Y, -Direction.X) * HalfThickness, Col);
	m_Buffer.AddVertex(Start + Vector2(Direction.Y, -Direction.X) * HalfThickness, Col);

	AddTriangleIndices(Offset);
}

void Paint::AddTriangles(const Rect& Vertices, const Color& Col, uint32_t Offset)
{
	m_Buffer.AddVertex(Vertices.Min, Col);
	m_Buffer.AddVertex(Vector2(Vertices.Max.X, Vertices.Min.Y), Col);
	m_Buffer.AddVertex(Vertices.Max, Col);
	m_Buffer.AddVertex(Vector2(Vertices.Min.X, Vertices.Max.Y), Col);

	AddTriangleIndices(Offset);
}

void Paint::AddTriangles(const Rect& Vertices, const Rect& TexCoords, const Color& Col, uint32_t Offset)
{
	m_Buffer.AddVertex(Vertices.Min, TexCoords.Min, Col);
	m_Buffer.AddVertex(Vector2(Vertices.Max.X, Vertices.Min.Y), Vector2(TexCoords.Max.X, TexCoords.Min.Y), Col);
	m_Buffer.AddVertex(Vertices.Max, TexCoords.Max, Col);
	m_Buffer.AddVertex(Vector2(Vertices.Min.X, Vertices.Max.Y), Vector2(TexCoords.Min.X, TexCoords.Max.Y), Col);

	AddTriangleIndices(Offset);
}

void Paint::AddTriangleIndices(uint32_t Offset)
{
	m_Buffer.AddIndex(Offset);
	m_Buffer.AddIndex(Offset + 1);
	m_Buffer.AddIndex(Offset + 2);
	m_Buffer.AddIndex(Offset);
	m_Buffer.AddIndex(Offset + 2);
	m_Buffer.AddIndex(Offset + 3);
}

DrawCommand& Paint::PushCommand(uint32_t IndexCount, uint32_t TextureID)
{
	return m_Buffer.PushCommand(IndexCount, TextureID, !m_ClipStack.empty() ? m_ClipStack.back() : Rect());
}

}
