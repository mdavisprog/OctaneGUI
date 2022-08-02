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

#include "DrawCommand.h"

namespace OctaneGUI
{

DrawCommand::DrawCommand(uint32_t VertexOffset, uint32_t IndexOffset, uint32_t IndexCount, uint32_t TextureID, Rect Clip)
    : m_VertexOffset(VertexOffset)
    , m_IndexOffset(IndexOffset)
    , m_IndexCount(IndexCount)
    , m_TextureID(TextureID)
    , m_Clip(Clip)
{
}

uint32_t DrawCommand::VertexOffset() const
{
    return m_VertexOffset;
}

uint32_t DrawCommand::IndexOffset() const
{
    return m_IndexOffset;
}

uint32_t DrawCommand::IndexCount() const
{
    return m_IndexCount;
}

uint32_t DrawCommand::TextureID() const
{
    return m_TextureID;
}

Rect DrawCommand::Clip() const
{
    return m_Clip;
}

DrawCommand::DrawCommand()
    : m_VertexOffset(0)
    , m_IndexOffset(0)
    , m_IndexCount(0)
    , m_TextureID(0)
    , m_Clip()
{
}

DrawCommand::~DrawCommand()
{
}

}
