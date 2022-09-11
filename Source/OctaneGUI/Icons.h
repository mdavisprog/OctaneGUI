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
#include <memory>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Texture;

class Icons
{
public:
    enum class Type : uint8_t
    {
        ArrowLeft,
        ArrowRight,
        ArrowUp,
        ArrowDown,
        Check,
        Expand,
        Collapse,
        Plus,
        Minus,
        Max
    };

    struct Definition
    {
    public:
        std::string Name {};
        std::string FileName {};
    };

    static const char* ToString(Type InType);
    static Type ToType(const std::string& Name);

    Icons();
    ~Icons();

    void Initialize();
    void Initialize(const std::vector<Definition>& Definitions, const Vector2& IconSize);
    std::shared_ptr<Texture> GetTexture() const;
    Rect GetUVs(Type InType) const;
    Rect GetUVsNormalized(Type InType) const;

private:
    std::shared_ptr<Texture> m_Texture;
    Rect m_UVs[(uint32_t)Type::Max];
};

}
