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

#include "Rect.h"

#include <cstdint>

namespace OctaneUI
{

class DrawCommand
{
public:
	DrawCommand(uint32_t VertexOffset, uint32_t IndexOffset, uint32_t IndexCount, uint32_t TextureID, Rect Clip);
	~DrawCommand();

	uint32_t VertexOffset() const;
	uint32_t IndexOffset() const;
	uint32_t IndexCount() const;
	uint32_t TextureID() const;
	Rect Clip() const;

private:
	DrawCommand();

	uint32_t m_VertexOffset;
	uint32_t m_IndexOffset;
	uint32_t m_IndexCount;
	uint32_t m_TextureID;
	Rect m_Clip;
};

}
