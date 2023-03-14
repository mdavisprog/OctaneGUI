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

#pragma once

#include "Vector2.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace OctaneGUI
{

struct Rect;
class Texture;

class Font
{
public:
    struct Range
    {
        uint32_t Min { 0 };
        uint32_t Max { 0 };
    };

    static Range BasicLatin;
    static Range Latin1Supplement;
    static Range LatinExtended1;
    static Range LatinExtended2;

    struct Glyph
    {
    public:
        Glyph();

        Vector2 Min {};
        Vector2 Max {};
        Vector2 Offset {};
        Vector2 Offset2 {};
        Vector2 Advance {};
    };

    static std::shared_ptr<Font> Create(const char* Path, float Size, const std::vector<Range>& Ranges = { BasicLatin, Latin1Supplement });
    static void SetTabSize(int TabSize);
    static int TabSize();

    Font();
    ~Font();

    bool Load(const char* Path, float Size, const std::vector<Range>& Ranges);
    bool Draw(uint32_t Char, Vector2& Position, Rect& Vertices, Rect& TexCoords) const;
    Vector2 Measure(const std::u32string_view& Text) const;
    Vector2 Measure(const std::u32string_view& Text, int& Lines) const;
    Vector2 Measure(const std::u32string_view& Text, int& Lines, float Wrap) const;
    Vector2 Measure(char Ch) const;
    Vector2 Measure(uint32_t CodePoint) const;

    uint32_t ID() const;
    float Size() const;
    float Ascent() const;
    float Descent() const;
    Vector2 SpaceSize() const;
    const char* Path() const;
    const std::shared_ptr<Texture>& GetTexture() const;

private:
    typedef std::unordered_map<unsigned int, Glyph> GlyphMap;

    const Glyph& GetGlyph(uint32_t CodePoint) const;

    static int s_TabSize;

    GlyphMap m_Glyphs;
    float m_Size { 0.0f };
    float m_Ascent { 0.0f };
    float m_Descent { 0.0f };
    Vector2 m_SpaceSize {};
    std::shared_ptr<Texture> m_Texture { nullptr };
    std::string m_Path {};
};

}
