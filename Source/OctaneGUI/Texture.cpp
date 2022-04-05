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

#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "External/stb/stb_image.h"

#include <cstring>

namespace OctaneGUI
{

Texture::OnLoadSignature Texture::s_OnLoad = nullptr;

void Texture::SetOnLoad(OnLoadSignature Fn)
{
	s_OnLoad = Fn;
}

std::shared_ptr<Texture> Texture::Load(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
	std::shared_ptr<Texture> Result;

	if (s_OnLoad)
	{
		uint32_t ID = s_OnLoad(Data, Width, Height);

		if (ID > 0)
		{
			Result = std::make_shared<Texture>();
			Result->m_ID = ID;
			Result->m_Size.X = Width;
			Result->m_Size.Y = Height;
		}
	}

	return Result;
}

std::shared_ptr<Texture> Texture::Load(const char* Path)
{
	std::shared_ptr<Texture> Result;

	int Width, Height, Channels;
	uint8_t* Data = stbi_load(Path, &Width, &Height, &Channels, 0);
	if (Data != nullptr)
	{
		std::vector<uint8_t> Buffer;
		Buffer.resize(Width * Height * Channels);
		std::memcpy(&Buffer[0], Data, Buffer.size());
		stbi_image_free(Data);

		Result = Load(Buffer, Width, Height);
	}

	return Result;
}

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::IsValid() const
{
	return m_ID != 0;
}

uint32_t Texture::GetID() const
{
	return m_ID;
}

Vector2 Texture::GetSize() const
{
	return m_Size;
}

}
