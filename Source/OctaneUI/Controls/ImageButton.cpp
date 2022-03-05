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

#include "../Json.h"
#include "../Theme.h"
#include "Image.h"
#include "ImageButton.h"

namespace OctaneUI
{

ImageButton::ImageButton(Window* InWindow)
	: Button(InWindow)
{
	m_Image = std::make_shared<Image>(InWindow);
	m_Image->SetParent(this);
}

ImageButton* ImageButton::SetTexture(const char* Path)
{
	m_Image->SetTexture(Path);
	UpdateSize();
	return this;
}

ImageButton* ImageButton::SetTexture(const std::shared_ptr<Texture>& InTexture)
{
	m_Image->SetTexture(InTexture);
	UpdateSize();
	return this;
}

void ImageButton::OnPaint(Paint& Brush) const
{
	Button::OnPaint(Brush);

	m_Image->OnPaint(Brush);
}

void ImageButton::Update()
{
	m_Image
		->SetTint(IsDisabled() ? GetTheme()->GetColor(Theme::Colors::Text_Disabled) : Color::White)
		->SetPosition(GetSize() * 0.5f - m_Image->GetSize() * 0.5f);
}

void ImageButton::OnLoad(const Json& Root)
{
	Button::OnLoad(Root);

	m_Image->OnLoad(Root["Image"]);
	UpdateSize();
}

void ImageButton::UpdateSize()
{
	Vector2 Padding = GetTheme()->GetConstant(Theme::Vector2Constants::Button_Padding);
	SetSize(m_Image->GetSize() + Padding * 2.0f);
}

}
