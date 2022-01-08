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

#include "Color.h"
#include "Icons.h"
#include "Texture.h"

namespace OctaneUI
{

Icons::Icons()
	: m_Texture(nullptr)
{
}

Icons::~Icons()
{
}

void Icons::Initialize()
{
	if (m_Texture)
	{
		return;
	}

	const uint32_t Width = 23;
	const uint32_t Height = 12;
	const char* Data =
	"x                    x "
	"x.x                 x.x"
	"x..x               x.x "
	"x...x             x.x  "
	"x....x           x.x   "
	"x.....x         x.x    "
	"x......x x     x.x     "
	"x.....x x.x   x.x      "
	"x....x   x.x x.x       "
	"x...x     x.x.x        "
	"x..x       x.x         "
	"x.x         x          "
	"x                      ";

	std::vector<uint8_t> Buffer;
	Buffer.resize(Width * Height * 4);
	uint32_t Index = 0;
	for (uint32_t Y = 0; Y < Height; Y++)
	{
		for (uint32_t X = 0; X < Width; X++)
		{
			const uint32_t Pos = X + Y * Width;
			const char Ch = Data[Pos];
			Color Col;

			switch (Ch)
			{
			case 'x': Col = Color(0, 0, 0, 255); break;
			case '.': Col = Color(255, 255, 255, 255); break;
			default: break;
			}

			Buffer[Index] = Col.R;
			Buffer[Index + 1] = Col.G;
			Buffer[Index + 2] = Col.B;
			Buffer[Index + 3] = Col.A;
			Index += 4;
		}
	}

	m_Texture = Texture::Load(Buffer, Width, Height);

	m_UVs[(int)Type::Expand] = Rect(0.0f, 0.0f, 8.0f, 12.0f);
	m_UVs[(int)Type::Check] = Rect(8.0f, 0.0f, 23.0f, 12.0f);
}

std::shared_ptr<Texture> Icons::GetTexture() const
{
	return m_Texture;
}

Rect Icons::GetUVs(Type InType) const
{
	return m_UVs[(int)InType];
}

}
