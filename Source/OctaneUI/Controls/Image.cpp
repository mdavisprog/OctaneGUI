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

#include "../Paint.h"
#include "../Texture.h"
#include "Image.h"

namespace OctaneUI
{

Image::Image(Window* InWindow)
	: Control(InWindow)
	, m_Texture(nullptr)
{
}

Image::~Image()
{
}

Image* Image::SetTexture(const char* Path)
{
	if (!m_Texture)
	{
		m_Texture = Texture::Load(Path);

		if (m_Texture)
		{
			SetUVs(Rect(Vector2::Zero, m_Texture->GetSize()));
		}
	}

	return this;
}

Image* Image::SetTexture(const std::shared_ptr<Texture>& InTexture)
{
	m_Texture = InTexture;
	return this;
}

Image* Image::SetUVs(const Rect& UVs)
{
	m_UVs = UVs;
	SetSize(m_UVs.GetSize());
	return this;
}

const char* Image::GetType() const
{
	return "Image";
}

void Image::OnPaint(Paint& Brush) const
{
	if (!m_Texture)
	{
		return;
	}

	Brush.Image(GetAbsoluteBounds(), m_UVs, m_Texture, Color(255, 255, 255, 255));
}

bool Image::IsFixedSize() const
{
	return true;
}

}
