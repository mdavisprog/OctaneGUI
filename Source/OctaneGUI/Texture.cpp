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

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"

#if _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4244 4456 4702)
#endif

#include "External/nanosvg/nanosvg.h"
#include "External/nanosvg/nanosvgrast.h"

#if _MSC_VER
    #pragma warning(pop)
#endif

#include "External/stb/stb_image.h"
#include "FileSystem.h"

#include <cstring>

namespace OctaneGUI
{

std::vector<uint8_t> RasterSVG(NSVGimage* Image, float Width, float Height)
{
    std::vector<uint8_t> Result;

    if (Image)
    {
        const float ScaleW = (float)Width / Image->width;
        const float ScaleH = (float)Height / Image->height;
        const float Scale = std::max<float>(ScaleW, ScaleH);

        Result.resize((size_t)(Width * Height * 4));

        NSVGrasterizer* Raster = nsvgCreateRasterizer();
        nsvgRasterize(Raster, Image, 0.0f, 0.0f, Scale, Result.data(), (int)Width, (int)Height, (int)(Width * 4));
        nsvgDeleteRasterizer(Raster);
        nsvgDelete(Image);
    }

    return Result;
}

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
            Result->m_Size.X = (float)Width;
            Result->m_Size.Y = (float)Height;
        }
    }

    return Result;
}

std::shared_ptr<Texture> Texture::Load(const char* Path)
{
    std::shared_ptr<Texture> Result { nullptr };

    const std::string Extension = FileSystem::Extension(Path);
    if (Extension == ".png")
    {
        Result = LoadPNG(Path);
    }
    else if (Extension == ".svg")
    {
        Result = LoadSVG(Path);
    }

    return Result;
}

std::shared_ptr<Texture> Texture::LoadPNG(const char* Path)
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
        Result->m_Path = Path;
    }

    return Result;
}

std::shared_ptr<Texture> Texture::LoadSVG(const char* Path)
{
    std::shared_ptr<Texture> Result { nullptr };

    NSVGimage* Image = nsvgParseFromFile(Path, "px", 96);
    if (Image != nullptr)
    {
        const std::vector<uint8_t> Data = RasterSVG(Image, Image->width, Image->height);
        Result = Load(Data, (uint32_t)Image->width, (uint32_t)Image->height);
    }

    return Result;
}

std::shared_ptr<Texture> Texture::LoadSVG(const char* Path, uint32_t Width, uint32_t Height)
{
    std::shared_ptr<Texture> Result { nullptr };

    NSVGimage* Image = nsvgParseFromFile(Path, "px", 96);
    if (Image != nullptr)
    {
        const std::vector<uint8_t> Data = RasterSVG(Image, (float)Width, (float)Height);
        Result = Load(Data, Width, Height);
    }

    return Result;
}

std::vector<uint8_t> Texture::LoadSVGData(const char* Path, uint32_t Width, uint32_t Height)
{
    std::vector<uint8_t> Result;

    NSVGimage* Image = nsvgParseFromFile(Path, "px", 96);
    if (Image != nullptr)
    {
        Result = RasterSVG(Image, (float)Width, (float)Height);
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

const char* Texture::Path() const
{
    return m_Path.c_str();
}

}
