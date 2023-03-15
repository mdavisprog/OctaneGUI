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

#include "Image.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"
#include "../Texture.h"
#include "../TextureCache.h"
#include "../Variant.h"
#include "../Window.h"

namespace OctaneGUI
{

Image::Image(Window* InWindow)
    : Control(InWindow)
{
}

Image::~Image()
{
}

Image& Image::SetTexture(const char* Path)
{
    if (Path == nullptr)
    {
        m_Texture = nullptr;
        return *this;
    }

    if (!m_Texture)
    {
        m_Texture = GetWindow()->GetTextureCache().Load(Path);

        if (m_Texture)
        {
            SetUVs(Rect(Vector2::Zero, m_Texture->GetSize()));
        }
    }

    return *this;
}

Image& Image::SetTexture(const std::shared_ptr<Texture>& InTexture)
{
    m_Texture = InTexture;
    if (m_Texture)
    {
        SetUVs({ Vector2::Zero, m_Texture->GetSize() });
    }
    else
    {
        SetUVs({});
    }
    return *this;
}

Image& Image::SetUVs(const Rect& UVs)
{
    m_UVs = UVs;
    SetSize(m_UVs.GetSize());
    return *this;
}

Image& Image::SetTint(const Color& Tint)
{
    m_Tint = Tint;
    return *this;
}

Color Image::Tint() const
{
    return m_Tint;
}

void Image::OnPaint(Paint& Brush) const
{
    if (!m_Texture)
    {
        return;
    }

    PROFILER_SAMPLE_GROUP("Image::OnPaint");

    const Vector2 Size = m_Texture->GetSize();
    const Rect TexCoords(
        m_UVs.Min / Size,
        m_UVs.Max / Size);

    Brush.Image(GetAbsoluteBounds(), TexCoords, m_Texture, m_Tint);
}

void Image::OnLoad(const Json& Root)
{
    Control::OnLoad(Root);

    SetTexture(Root["Texture"].String());
    m_Tint = Variant(Root["Tint"]).ToColor(Color::White);

    const Json& UVs = Root["UVs"];
    if (!UVs.IsNull())
    {
        const Rect Value(
            UVs["Left"].Number(),
            UVs["Top"].Number(),
            UVs["Right"].Number(),
            UVs["Bottom"].Number());

        SetUVs(Value);
    }
}

void Image::OnSave(Json& Root) const
{
    Control::OnSave(Root);

    Root["Texture"] = m_Texture->Path();

    Json UVs(Json::Type::Object);
    UVs["Left"] = m_UVs.Min.X;
    UVs["Top"] = m_UVs.Min.Y;
    UVs["Right"] = m_UVs.Max.X;
    UVs["Bottom"] = m_UVs.Max.Y;
    Root["UVs"] = std::move(UVs);

    Root["Tint"] = Color::ToJson(m_Tint);
}

bool Image::IsFixedSize() const
{
    return true;
}

}
