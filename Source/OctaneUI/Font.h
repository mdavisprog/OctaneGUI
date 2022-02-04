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

#include "Vector2.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace OctaneUI
{

struct Rect;
class Texture;

class Font
{
public:
	struct Glyph
	{
	public:
		Glyph();

		Vector2 m_Min;
		Vector2 m_Max;
		Vector2 m_Offset;
		Vector2 m_Advance;
	};

	Font();
	~Font();

	bool Load(const char* Path, float Size);
	bool Draw(int32_t Char, Vector2& Position, Rect& Vertices, Rect& TexCoords, bool NormalizeTexCoords = false) const;
	Vector2 Measure(const std::string& Text) const;
	Vector2 Measure(char Ch) const;
	Vector2 Measure(int32_t CodePoint) const;

	uint32_t GetID() const;
	float GetSize() const;
	Vector2 GetSpaceSize() const;

private:
	std::vector<Glyph> m_Glyphs;
	float m_Size;
	float m_Ascent;
	float m_Descent;
	Vector2 m_SpaceSize;
	std::shared_ptr<Texture> m_Texture;
};

}
