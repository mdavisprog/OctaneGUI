/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

#include "TextureCache.h"
#include "Texture.h"

#include <cstring>

namespace OctaneGUI
{

TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
    m_Cache.clear();
}

std::shared_ptr<Texture> TextureCache::Load(const char* Path)
{
    if (m_Cache.find(Path) == m_Cache.end() && std::strlen(Path) > 0)
    {
        std::shared_ptr<Texture> Result = Texture::Load(Path);

        if (Result && Result->IsValid())
        {
            m_Cache[Path] = Result;
        }
    }

    return m_Cache[Path];
}

std::shared_ptr<Texture> TextureCache::LoadSVG(const char* Path, uint32_t Width, uint32_t Height)
{
    std::shared_ptr<Texture>& Result = m_Cache[Path];
    if (!Result)
    {
        Result = Texture::LoadSVG(Path, Width, Height);
    }

    return m_Cache[Path];
}

const TextureCache::CacheMap& TextureCache::Cache() const
{
    return m_Cache;
}

}
