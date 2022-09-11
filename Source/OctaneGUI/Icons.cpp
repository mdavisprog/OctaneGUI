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

#include "Icons.h"
#include "Color.h"
#include "Texture.h"

#include <cstring>

namespace OctaneGUI
{

const char* Icons::ToString(Type InType)
{
    switch (InType)
    {
    case Type::ArrowLeft: return "ArrowLeft";
    case Type::ArrowRight: return "ArrowRight";
    case Type::ArrowUp: return "ArrowUp";
    case Type::ArrowDown: return "ArrowDown";
    case Type::Check: return "Check";
    case Type::Expand: return "Expand";
    case Type::Collapse: return "Collapse";
    case Type::Plus: return "Plus";
    case Type::Minus: return "Minus";
    default: break;
    }

    return "None";
}

Icons::Type Icons::ToType(const std::string& Name)
{
    for (int I = 0; I < (int)Type::Max; I++)
    {
        if (Name == ToString((Type)I))
        {
            return (Type)I;
        }
    }

    return Type::Max;
}

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

    const uint32_t Width = 102;
    const uint32_t Height = 13;
    const char* Data =
        "x               x               xx                                                                    "
        "x.x           x.x              x..x                                                                   "
        "x..x         x..x             x..x                                           ..        ..             "
        "x...x       x...x            x..x                                             ..       ..             "
        "x....x     x....x           x..x    xxxxxxxxxxxxx       x       ..       ..    ..      ..             "
        "x.....x   x.....x          x..x      x.........x       x.x       ..     ..      ..  ........ ........ "
        "x......x x......x  xx     x..x        x.......x       x...x       ..   ..        .. ........ ........ "
        "x.....x   x.....x x..x   x..x          x.....x       x.....x       .. ..        ..     ..             "
        "x....x     x....x  x..x x..x            x...x       x.......x       ...        ..      ..             "
        "x...x       x...x   x..x..x              x.x       x.........x       .        ..       ..             "
        "x..x         x..x    x...x                x       xxxxxxxxxxxxx              ..                       "
        "x.x           x.x     xxx                                                                             "
        "x               x                                                                                     "
        "                                                                                                      ";

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
            case 'x':
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

    m_UVs[(int)Type::ArrowRight] = { 0.0f, 0.0f, 8.0f, 13.0f };
    m_UVs[(int)Type::ArrowLeft] = { 9.0f, 0.0f, 17.0f, 13.0f };
    m_UVs[(int)Type::ArrowUp] = { 50.0f, 4.0f, 63.0f, 11.0f };
    m_UVs[(int)Type::ArrowDown] = { 36.0f, 4.0f, 49.0f, 11.0f };
    m_UVs[(int)Type::Check] = { 18.0f, 0.0f, 35.0f, 12.0f };
    m_UVs[(int)Type::Expand] = { 64.0f, 4.0f, 75.0f, 9.0f };
    m_UVs[(int)Type::Collapse] = { 77.0f, 2.0f, 83.0f, 11.0f };
    m_UVs[(int)Type::Plus] = { 84.0f, 2.0f, 92.0f, 10.0f };
    m_UVs[(int)Type::Minus] = { 93.0f, 5.0f, 101.0f, 7.0f };
}

void Icons::Initialize(const std::vector<Definition>& Definitions, const Vector2& IconSize)
{
    const size_t IconW = (size_t)IconSize.X;
    const size_t IconH = (size_t)IconSize.Y;
    const size_t Width = IconW;
    const size_t Height = IconH * (size_t)Type::Max;
    std::vector<uint8_t> Data;
    Data.resize(Width * Height * 4);

    size_t DataOffset = 0;
    Vector2 Offset;
    for (const Definition& Item : Definitions)
    {
        const std::vector<uint8_t> IconData = Texture::LoadSVGData(Item.FileName.c_str(), IconW, IconH);
        memcpy(&Data[DataOffset], IconData.data(), IconData.size());
        DataOffset += IconData.size();

        Type IconType = ToType(Item.Name);
        m_UVs[(int)IconType] = { Offset.X, Offset.Y, Offset.X + IconSize.X, Offset.Y + IconSize.Y };
        Offset.Y += IconSize.Y;
    }

    m_Texture = Texture::Load(Data, Width, Height);
}

std::shared_ptr<Texture> Icons::GetTexture() const
{
    return m_Texture;
}

Rect Icons::GetUVs(Type InType) const
{
    return m_UVs[(int)InType];
}

Rect Icons::GetUVsNormalized(Type InType) const
{
    const Rect& UVs = m_UVs[(int)InType];
    const Vector2 Size = m_Texture->GetSize();
    return Rect(
        UVs.Min / Size,
        UVs.Max / Size);
}

}
