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

#pragma once

#include "../Color.h"
#include "Control.h"

namespace OctaneGUI
{

class Texture;

/// @brief Control that displays an image.
///
/// An Image is displayed through a loaded Texture object. The texture set can
/// be a standalone texture, or UV coordinates can be defined to only show
/// part of a texture. The tint of the image can also be set to change the color.
class Image : public Control
{
    CLASS(Image)

public:
    Image(Window* InWindow);
    virtual ~Image();

    Image& SetTexture(const char* Path);
    Image& SetTexture(const std::shared_ptr<Texture>& InTexture);
    Image& SetUVs(const Rect& UVs);

    Image& SetTint(const Color& Tint);
    Color Tint() const;

    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;

protected:
    virtual bool IsFixedSize() const override;

private:
    std::shared_ptr<Texture> m_Texture { nullptr };
    Rect m_UVs {};
    Color m_Tint { Color::White };
};

}
