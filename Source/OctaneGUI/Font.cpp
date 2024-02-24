/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

namespace OctaneGUI
{

Font::Range Font::BasicLatin { 0x20, 0x7F };
Font::Range Font::Latin1Supplement { 0xA0, 0xFF };
Font::Range Font::LatinExtended1 { 0x100, 0x17F };
Font::Range Font::LatinExtended2 { 0x180, 0x24F };

Font::Glyph::Glyph()
{
}

std::shared_ptr<Font> Font::Create(const char* Path, float Size, const std::vector<Range>& Ranges)
{
    std::shared_ptr<Font> Result = std::make_shared<Font>();

    if (!Result->Load(Path, Size, Ranges))
    {
        return nullptr;
    }

    return Result;
}

void Font::SetTabSize(int TabSize)
{
    s_TabSize = TabSize;
}

int Font::TabSize()
{
    return s_TabSize;
}

Font::Font()
{
}

Font::~Font()
{
}

void IncreaseSize(Vector2& TextureSize, float Delta)
{
    if (TextureSize.Y < TextureSize.X)
    {
        TextureSize.Y = TextureSize.X;
    }
    else
    {
        TextureSize.X += Delta;
    }
}

bool Font::Load(const char* Path, float Size, const std::vector<Range>& Ranges)
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

    // 1. Initialize the font data from the input stream.
    Vector2 TextureSize { 128.0f, 128.0f };
    const uint8_t* Data = (uint8_t*)Buffer.data();
    stbtt_fontinfo Info {};
    stbtt_InitFont(&Info, Data, stbtt_GetFontOffsetForIndex(Data, 0));

    // 2. Begin our pack context. This initializes the stbtt_pack_context struct
    stbtt_pack_context PackContext {};
    stbtt_PackBegin(&PackContext, nullptr, (int)TextureSize.X, (int)TextureSize.Y, 0, 1, nullptr);

    // 3. Gather all rects to be rendered based on the desired character ranges.
    std::vector<stbtt_packedchar> Chars;
    std::vector<stbrp_rect> AllRects;
    std::vector<stbtt_pack_range> PackRanges;
    for (const Range& Range_ : Ranges)
    {
        PackRanges.push_back({});
        stbtt_pack_range& PackRange = PackRanges.back();
        PackRange.font_size = Size;
        PackRange.first_unicode_codepoint_in_range = Range_.Min;
        PackRange.num_chars = Range_.Max - Range_.Min + 1; // Include the final character 'Max'
        PackRange.array_of_unicode_codepoints = nullptr;

        Chars.resize(Chars.size() + PackRange.num_chars);

        std::vector<stbrp_rect> Rects;
        Rects.resize(PackRange.num_chars);

        stbtt_PackFontRangesGatherRects(&PackContext, &Info, &PackRange, 1, Rects.data());
        AllRects.insert(AllRects.end(), Rects.begin(), Rects.end());

        // Turn on skipping missing codepoints after the first range.
        stbtt_PackSetSkipMissingCodepoints(&PackContext, 1);
    }

    // 4. Determine an appropriate texture size to render all of the glyphs into.
    int Success = 0;
    do
    {
        stbrp_context Context {};
        std::vector<stbrp_node> Nodes;
        Nodes.resize((int)TextureSize.X - PackContext.padding);
        stbrp_init_target(&Context, (int)TextureSize.X - PackContext.padding, (int)TextureSize.Y - PackContext.padding, Nodes.data(), (int)Nodes.size());
        Success = stbrp_pack_rects(&Context, AllRects.data(), (int)AllRects.size());
        if (Success == 0)
        {
            IncreaseSize(TextureSize, 128.0f);
        }
    }
    while (Success == 0);

    // Reset to not skip when rendering each rect.
    stbtt_PackSetSkipMissingCodepoints(&PackContext, 0);

    std::vector<uint8_t> Texture;
    Texture.resize((int)TextureSize.X * (int)TextureSize.Y);

    PackContext.width = (int)TextureSize.X;
    PackContext.height = (int)TextureSize.Y;
    PackContext.pixels = Texture.data();
    PackContext.stride_in_bytes = PackContext.width;

    // Need to update the pointers for chardata_for_range
    size_t Offset = 0;
    for (stbtt_pack_range& Range_ : PackRanges)
    {
        Range_.chardata_for_range = &Chars[Offset];
        Offset += Range_.num_chars;
    }

    // 5. Perform the render.
    Success = stbtt_PackFontRangesRenderIntoRects(&PackContext, &Info, PackRanges.data(), (int)PackRanges.size(), AllRects.data());

    stbtt_PackEnd(&PackContext);

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

    // 6. Map each character rect to a glyph object.
    Index = 0;
    for (const Range& Range_ : Ranges)
    {
        for (unsigned int Codepoint = Range_.Min; Codepoint <= Range_.Max; Codepoint++)
        {
            const stbtt_packedchar& PackedChar = Chars[Index++];
            Glyph& Item = m_Glyphs[Codepoint];
            Item.Min = { (float)PackedChar.x0, (float)PackedChar.y0 };
            Item.Max = { (float)PackedChar.x1, (float)PackedChar.y1 };
            Item.Offset = { (float)PackedChar.xoff, (float)PackedChar.yoff };
            Item.Offset2 = { (float)PackedChar.xoff2, (float)PackedChar.yoff2 };
            Item.Advance = { PackedChar.xadvance, 0.0f };
        }
    }

    m_SpaceSize = Measure(U" ");

    return true;
}

bool Font::Draw(uint32_t Char, Vector2& Position, Rect& Vertices, Rect& TexCoords) const
{
    const bool IsTab = Char == '\t';
    if (IsTab)
    {
        Char = ' ';
    }

    const Glyph& Item = GetGlyph(Char);
    const Vector2 InvertedSize = m_Texture->GetSize().Invert();
    const Vector2 DiffOffset = Item.Offset2 - Item.Offset;

    int X = (int)floor(Position.X + Item.Offset.X + 0.5f);
    int Y = (int)floor(Position.Y + Item.Offset.Y + m_Ascent + 0.5f);

    Vertices.Min = Vector2((float)X, (float)Y);
    Vertices.Max = Vertices.Min + DiffOffset;

    TexCoords.Min = Item.Min * InvertedSize;
    TexCoords.Max = Item.Max * InvertedSize;

    Position += Item.Advance;

    if (IsTab)
    {
        // Need to subtract one as a space offset has already been added above.
        const float TabSize = (float)(s_TabSize - 1);
        Vertices.Max += DiffOffset * Vector2(TabSize, 0.0);
        Position.X += Item.Advance.X * TabSize;
    }

    return true;
}

Vector2 Font::Measure(const std::u32string_view& Text) const
{
    Vector2 Result;

    for (char32_t Ch : Text)
    {
        // Should ignore newlines here. If newlines are desired, then Measure(const u32string&, int&) should be used.
        if (Ch == '\n')
        {
            continue;
        }

        const Vector2 Size = Measure(Ch);
        Result.X += Size.X;
        Result.Y = std::max<float>(Result.Y, Size.Y);
    }

    return Result;
}

Vector2 Font::Measure(const std::u32string_view& Text, int& Lines) const
{
    Vector2 Result;
    Lines = 1;

    Vector2 LineSize;
    for (char32_t Ch : Text)
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

Vector2 Font::Measure(const std::u32string_view& Text, int& Lines, float Wrap) const
{
    Vector2 Result;
    Lines = 1;

    Vector2 LineSize;
    size_t Start = 0;
    for (size_t I = 0; I < Text.length(); I++)
    {
        const char32_t Ch = Text[I];

        if (std::isspace(Ch) || I >= Text.length() - 1)
        {
            I = std::min<size_t>(I + 1, Text.length());
            const std::u32string_view View { &Text[Start], I - Start };
            const Vector2 Size = Measure(View);

            if (LineSize.X + Size.X > Wrap || Ch == '\n')
            {
                Lines++;
                Result.X = std::max<float>(Result.X, LineSize.X);
                Result.Y += LineSize.Y;
                LineSize = Size;
            }
            else
            {
                LineSize.X += Size.X;
                LineSize.Y = std::max<float>(LineSize.Y, Size.Y);
            }

            Start = I;
        }
    }

    Result.X = std::max<float>(Result.X, LineSize.X);
    Result.Y += LineSize.Y;

    return Result;
}

Vector2 Font::Measure(char Ch) const
{
    return Measure((uint32_t)Ch);
}

Vector2 Font::Measure(uint32_t CodePoint) const
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

const std::shared_ptr<Texture>& Font::GetTexture() const
{
    return m_Texture;
}

const Font::Glyph& Font::GetGlyph(uint32_t CodePoint) const
{
    if (m_Glyphs.find(CodePoint) == m_Glyphs.end())
    {
        // TODO: Currently, we are hardcoding the missing character glyph to this character.
        // Should come up with a more generic solution.
        CodePoint = 127;
    }

    return m_Glyphs.at(CodePoint);
}

int Font::s_TabSize { 4 };

}
