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

#include <cmath>

#define PI 3.14159265358979323846f

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

void Paint::Text(const std::shared_ptr<Font>& InFont, const Vector2& Position, const std::u32string_view& Contents, const Color& Col)
{
	Textf(InFont, Position, Contents, { { 0, Contents.length(), Col } });
}

void Paint::Textf(const std::shared_ptr<Font>& InFont, const Vector2& Position, const std::u32string_view& Contents, const std::vector<TextSpan>& Spans)
{
	if (Contents.empty())
	{
		return;
	}

	std::vector<std::u32string_view> Views;
	for (const TextSpan& Item : Spans)
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
	for (size_t I = 0; I < Spans.size(); I++)
	{
		const TextSpan& Span = Spans[I];
		const std::u32string_view& View = Views[I];
		GatherGlyphs(InFont, Pos, Position, View, GlyphRects, GlyphUVs);
		GlyphColors.insert(GlyphColors.end(), View.size(), Span.TextColor);
	}

	AddTriangles(GlyphRects, GlyphUVs, GlyphColors, InFont->ID());
}

void Paint::TextWrapped(const std::shared_ptr<Font>& InFont, const Vector2& Position, const std::u32string_view& Contents, const std::vector<TextSpan>& Spans, float Width)
{
	if (Contents.empty())
	{
		return;
	}

	std::vector<Rect> GlyphRects;
	std::vector<Rect> GlyphUVs;
	std::vector<Color> GlyphColors;

	size_t Start = 0;
	Vector2 Pos = Position;
	for (const TextSpan& Span : Spans)
	{
		std::vector<std::u32string_view> Views;
		Start = Span.Start;
		for (size_t Index = Span.Start; Index <= Span.End; Index++)
		{
			const char32_t& Char = Contents[Index];
			if (std::isspace(Char) || Index >= Span.End)
			{
				Index = std::min<size_t>(Index + 1, Span.End);
				size_t Count = Index - Start;
				const std::u32string_view View(&Contents[Start], Count);

				std::vector<Rect> Rects;
				std::vector<Rect> UVs;
				GatherGlyphs(InFont, Pos, Position, View, Rects, UVs, false);
				GlyphColors.insert(GlyphColors.end(), Count, Span.TextColor);

				float CurrentWidth = Pos.X - Position.X;
				if (CurrentWidth > Width)
				{
					Rects.clear();
					UVs.clear();
					Pos.X = Position.X;
					Pos.Y += InFont->Size();
					GatherGlyphs(InFont, Pos, Position, View, Rects, UVs, false);
				}

				GlyphRects.insert(GlyphRects.end(), Rects.begin(), Rects.end());
				GlyphUVs.insert(GlyphUVs.end(), UVs.begin(), UVs.end());
				Start = Index;
			}
		}
	}

	AddTriangles(GlyphRects, GlyphUVs, GlyphColors, InFont->ID());
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

std::vector<Vector2> GetPoints(const Vector2& Center, float Radius, int Steps)
{
	const float Delta = (2.0f * PI) / Steps;

	std::vector<Vector2> Result;
	Result.resize(Steps);

	for (int I = 0; I < Steps; I++)
	{
		const float Angle = I * Delta;
		Result[I] = { Center.X + (std::cos(Angle) * Radius), Center.Y + (std::sin(Angle) * Radius) };
	}

	return std::move(Result);
}

void Paint::Circle(const Vector2& Center, float Radius, const Color& Tint, int Steps)
{
	std::vector<Vector2> Vertices = GetPoints(Center, Radius, Steps);

	PushCommand(3 * Steps, 0);

	uint32_t Offset = 0;
	for (size_t I = 0; I < Vertices.size(); I++)
	{
		size_t J = I + 1 >= Vertices.size() ? 0 : I + 1;
		m_Buffer.AddVertex(Center, Tint);
		m_Buffer.AddVertex(Vertices[I], Tint);
		m_Buffer.AddVertex(Vertices[J], Tint);

		m_Buffer.AddIndex(Offset);
		m_Buffer.AddIndex(Offset + 1);
		m_Buffer.AddIndex(Offset + 2);
		Offset += 3;
	}
}

void Paint::CircleOutline(const Vector2& Center, float Radius, const Color& Tint, float Thickness, int Steps)
{
	std::vector<Vector2> Vertices = GetPoints(Center, Radius, Steps);

	Vector2 Start = Vertices[0];
	for (size_t I = 1; I < Vertices.size(); I++)
	{
		const Vector2& End = Vertices[I];
		Line(Start, End, Tint, Thickness);
		Start = End;
	}

	Line(Start, Vertices.front(), Tint, Thickness);
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

void Paint::AddTriangles(const std::vector<Rect>& Rects, const std::vector<Rect>& UVs, const std::vector<Color>& Colors, uint32_t TextureID)
{
	if (Rects.empty() || UVs.empty() || Colors.empty())
	{
		return;
	}

	PushCommand(6 * Rects.size(), TextureID);

	uint32_t Offset = 0;
	for (size_t I = 0; I < Rects.size(); I++)
	{
		const Rect& Vertices = Rects[I];
		const Rect& TexCoords = UVs[I];
		const Color& Color_ = Colors[I];

		AddTriangles(Vertices, TexCoords, Color_, Offset);
		Offset += 4;
	}
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

void Paint::GatherGlyphs(const std::shared_ptr<Font>& InFont, Vector2& Position, const Vector2& Origin, const std::u32string_view& Contents, std::vector<Rect>& Rects, std::vector<Rect>& UVs, bool ShouldClip)
{
	const Rect Clip = !m_ClipStack.empty() ? m_ClipStack.back() : Rect();
	for (char32_t Char : Contents)
	{
		if (Char == '\n')
		{
			Position.X = Origin.X;
			Position.Y += InFont->Size();
			continue;
		}

		if (!Clip.IsZero())
		{
			// Don't check for < Clip.Min.X since size of glyph is not known here.
			if (ShouldClip && (Position.X > Clip.Max.X || Position.Y > Clip.Max.Y || Position.Y + InFont->Size() < Clip.Min.Y))
			{
				continue;
			}
		}

		Rect Vertices;
		Rect TexCoords;

		InFont->Draw((uint32_t)Char, Position, Vertices, TexCoords);

		if (!(ShouldClip && IsClipped(Vertices)))
		{
			Rects.push_back(Vertices);
			UVs.push_back(TexCoords);
		}
	}
}

}
