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

#include "Font.h"
#include "Rect.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "External/stb/stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "External/stb/stb_truetype.h"
#include "Texture.h"

#include <cmath>
#include <fstream>

namespace OctaneUI
{

Font::Glyph::Glyph()
{
}

std::shared_ptr<Font> Font::Create(const char* Path, float Size)
{
	std::shared_ptr<Font> Result = std::make_shared<Font>();

	if (!Result->Load(Path, Size))
	{
		return nullptr;
	}

	return Result;
}

Font::Font()
{
}

Font::~Font()
{
}

// TODO: Do our own rect packing so that we can avoid a loop that just increases the texture size.
bool LoadFont(const std::vector<char>& FontData, float FontSize, std::vector<uint8_t>& Data, const Vector2& Size, std::vector<stbtt_packedchar>& Glyphs)
{
	Data.clear();
	Data.resize((int)Size.X * (int)Size.Y);

	stbtt_pack_context PackContext;
	if (stbtt_PackBegin(&PackContext, Data.data(), (int)Size.X, (int)Size.Y, 0, 1, nullptr) == 0)
	{
		return false;
	}

	Glyphs.clear();
	Glyphs.resize(96);
	int Result = stbtt_PackFontRange(&PackContext, (uint8_t*)FontData.data(), 0, FontSize, 32, Glyphs.size(), Glyphs.data());

	stbtt_PackEnd(&PackContext);

	return Result > 0;
}

bool Font::Load(const char* Path, float Size)
{
	std::ifstream Stream;
	Stream.open(Path, std::ios_base::in | std::ios_base::binary);
	if (!Stream.is_open())
	{
		return false;
	}

	Stream.seekg(0, std::ios_base::end);
	size_t FileSize = Stream.tellg();
	Stream.seekg(0, std::ios_base::beg);

	std::vector<char> Buffer;
	Buffer.resize(FileSize);

	Stream.read(&Buffer[0], Buffer.size());
	Stream.close();

	float LineGap;
	stbtt_GetScaledFontVMetrics((uint8_t*)Buffer.data(), 0, Size, &m_Ascent, &m_Descent, &LineGap);

	m_Size = Size;
	m_Path = Path;

	const float TextureBaseSize = 128.0f;
	Vector2 TextureSize;
	std::vector<uint8_t> Texture;
	std::vector<stbtt_packedchar> Chars;
	do
	{
		TextureSize = { TextureSize.X + TextureBaseSize, TextureSize.Y + TextureBaseSize };
	}
	while (!LoadFont(Buffer, Size, Texture, TextureSize, Chars));

	// Convert data to RGBA32
	std::vector<uint8_t> RGBA32;
	RGBA32.resize((int)TextureSize.X * (int)TextureSize.Y * 4);
	size_t Index = 0;
	for (size_t I = 0; I < Texture.size(); I++)
	{
		RGBA32[Index] = 255;
		RGBA32[Index + 1] = 255;
		RGBA32[Index + 2] = 255;
		RGBA32[Index + 3] = Texture[I];
		Index += 4;
	}

	m_Texture = Texture::Load(RGBA32, (uint32_t)TextureSize.X, (uint32_t)TextureSize.Y);
	if (!m_Texture)
	{
		return false;
	}

	for (const stbtt_packedchar& Char : Chars)
	{
		Glyph Item;
		Item.m_Min = Vector2(Char.x0, Char.y0);
		Item.m_Max = Vector2(Char.x1, Char.y1);
		Item.m_Offset = Vector2(Char.xoff, Char.yoff);
		Item.m_Advance = Vector2(Char.xadvance, 0.0f);
		m_Glyphs.push_back(Item);
	}

	m_SpaceSize = Measure(" ");

	return true;
}

bool Font::Draw(int32_t Char, Vector2& Position, Rect& Vertices, Rect& TexCoords) const
{
	if (Char < 0 || Char >= m_Glyphs.size())
	{
		return false;
	}

	const Glyph& Item = m_Glyphs[Char];
	const Vector2 ItemSize = Item.m_Max - Item.m_Min;
	const Vector2 InvertedSize = m_Texture->GetSize().Invert();

	int X = (int)floor(Position.X + Item.m_Offset.X + 0.5f);
	int Y = (int)floor(Position.Y + Item.m_Offset.Y + m_Ascent + 0.5f);

	Vertices.Min = Vector2((float)X, (float)Y);
	Vertices.Max = Vertices.Min + ItemSize;

	TexCoords.Min = Item.m_Min * InvertedSize;
	TexCoords.Max = Item.m_Max * InvertedSize;

	Position += Item.m_Advance;

	return true;
}

Vector2 Font::Measure(const std::string& Text) const
{
	Vector2 Result;

	for (char Ch : Text)
	{
		const Vector2 Size = Measure(Ch);
		Result.X += Size.X;
		Result.Y = std::max<float>(Result.Y, Size.Y);
	}

	return Result;
}

Vector2 Font::Measure(const std::string& Text, int& Lines) const
{
	Vector2 Result;
	Lines = 1;

	Vector2 LineSize;
	for (char Ch : Text)
	{
		if (Ch == '\n')
		{
			Lines++;
			Result.X = std::max<float>(Result.X, LineSize.X);
			Result.Y += LineSize.Y;
			LineSize = { 0.0f, 0.0f };
		}

		const Vector2 Size = Measure(Ch);
		LineSize.X += Size.X;
		LineSize.Y = std::max<float>(LineSize.Y, Size.Y);
	}

	Result.X = std::max<float>(Result.X, LineSize.X);
	Result.Y += LineSize.Y;

	return Result;
}

Vector2 Font::Measure(char Ch) const
{
	// TODO: Refactor how we match the character index with the glyph in the array.
	int32_t CodePoint = Ch - 32;
	return Measure(CodePoint);
}

Vector2 Font::Measure(int32_t CodePoint) const
{
	Vector2 Result;

	Vector2 Position;
	Rect Vertices, TexCoords;
	Draw(CodePoint, Position, Vertices, TexCoords);
	Result.X = Position.X;
	Result.Y = std::max<float>(Result.Y, Vertices.Height());

	return Result;
}

uint32_t Font::ID() const
{
	if (!m_Texture)
	{
		return 0;
	}

	return m_Texture->GetID();
}

float Font::Size() const
{
	return m_Size;
}

float Font::Ascent() const
{
	return m_Ascent;
}

float Font::Descent() const
{
	return m_Descent;
}

Vector2 Font::SpaceSize() const
{
	return m_SpaceSize;
}

const char* Font::Path() const
{
	return m_Path.c_str();
}

}
