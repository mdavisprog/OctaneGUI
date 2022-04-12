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

#include "ImageButton.h"
#include "../Json.h"
#include "../ThemeProperties.h"
#include "Image.h"

namespace OctaneGUI
{

ImageButton::ImageButton(Window* InWindow)
	: Button(InWindow)
{
	m_Image = std::make_shared<Image>(InWindow);
	m_Image->SetParent(this);
}

ImageButton& ImageButton::SetTexture(const char* Path)
{
	m_Image->SetTexture(Path);
	UpdateSize();
	return *this;
}

ImageButton& ImageButton::SetTexture(const std::shared_ptr<Texture>& InTexture)
{
	m_Image->SetTexture(InTexture);
	UpdateSize();
	return *this;
}

ImageButton& ImageButton::SetUVs(const Rect& UVs)
{
	m_Image->SetUVs(UVs);
	UpdateSize();
	return *this;
}

ImageButton& ImageButton::SetTint(const Color& Tint)
{
	m_Image->SetTint(Tint);
	Invalidate();
	return *this;
}

void ImageButton::OnPaint(Paint& Brush) const
{
	Button::OnPaint(Brush);

	m_Image->OnPaint(Brush);
}

void ImageButton::Update()
{
	OnThemeLoaded();
	UpdateImagePosition(IsPressed());
}

void ImageButton::OnLoad(const Json& Root)
{
	Button::OnLoad(Root);

	m_Image->OnLoad(Root["Image"]);
	UpdateSize();

	if (GetProperty(ThemeProperties::ImageButton).ToColor() != m_Image->Tint())
	{
		SetProperty(ThemeProperties::ImageButton, m_Image->Tint());
	}
}

void ImageButton::OnSave(Json& Root) const
{
	Button::OnSave(Root);

	Json ImageRoot(Json::Type::Object);
	m_Image->OnSave(ImageRoot);
	Root["Image"] = std::move(ImageRoot);
}

void ImageButton::OnThemeLoaded()
{
	m_Image->SetTint(IsDisabled()
			? GetProperty(ThemeProperties::Text_Disabled).ToColor()
			: GetProperty(ThemeProperties::ImageButton).ToColor());
}

void ImageButton::OnPressed()
{
	UpdateImagePosition(true);
}

void ImageButton::OnReleased()
{
	UpdateImagePosition(false);
}

void ImageButton::UpdateImagePosition(bool Pressed)
{
	Vector2 Position = GetSize() * 0.5f - m_Image->GetSize() * 0.5f;

	if (GetProperty(ThemeProperties::Button_3D).Bool() && Pressed)
	{
		Position += Vector2(1.0f, 1.0f);
	}

	m_Image->SetPosition(Position);
}

void ImageButton::UpdateSize()
{
	Vector2 Padding = GetProperty(ThemeProperties::Button_Padding).Vector();
	SetSize(m_Image->GetSize() + Padding * 2.0f);
}

}
