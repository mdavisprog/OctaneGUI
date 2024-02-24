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

#include "TextButton.h"
#include "../Json.h"
#include "Text.h"

namespace OctaneGUI
{

TextButton::TextButton(Window* InWindow)
    : Button(InWindow)
{
    m_Text = std::make_shared<Text>(InWindow);
    m_Text->SetParent(this);
}

Button* TextButton::SetText(const char* InText)
{
    m_Text->SetText(InText);
    UpdateSize();
    return this;
}

const char32_t* TextButton::GetText() const
{
    return m_Text->GetText();
}

void TextButton::SetWindow(Window* InWindow)
{
    Control::SetWindow(InWindow);
    m_Text->SetWindow(InWindow);
}

void TextButton::OnPaint(Paint& Brush) const
{
    Button::OnPaint(Brush);

    m_Text->OnPaint(Brush);
}

void TextButton::Update()
{
    if (IsDisabled())
    {
        m_Text->SetProperty(ThemeProperties::Text, GetProperty(ThemeProperties::Text_Disabled));
    }
    else
    {
        m_Text->ClearProperty(ThemeProperties::Text);
    }

    UpdateTextPosition(IsPressed());
}

void TextButton::OnLoad(const Json& Root)
{
    Button::OnLoad(Root);

    m_Text->OnLoad(Root["Text"]);
    UpdateSize();
}

void TextButton::OnSave(Json& Root) const
{
    Button::OnSave(Root);

    Json TextRoot(Json::Type::Object);
    m_Text->OnSave(TextRoot);
    Root["Text"] = std::move(TextRoot);
}

void TextButton::OnThemeLoaded()
{
    Button::OnThemeLoaded();
    m_Text->OnThemeLoaded();
    UpdateSize();
}

void TextButton::OnPressed()
{
    UpdateTextPosition(true);
}

void TextButton::OnReleased()
{
    UpdateTextPosition(false);
}

void TextButton::UpdateTextPosition(bool Pressed)
{
    const Vector2 Size = GetSize() * 0.5f;
    const Vector2 TextSize = m_Text->GetSize() * 0.5f;
    Vector2 Position = Size - TextSize;

    if (GetProperty(ThemeProperties::Button_3D).Bool() && Pressed)
    {
        Position += Vector2(1.0f, 1.0f);
    }

    m_Text->SetPosition(Position);
}

void TextButton::UpdateSize()
{
    Vector2 Padding = GetProperty(ThemeProperties::Button_Padding).Vector() * RenderScale();
    Vector2 Size = m_Text->GetSize() + Padding * 2.0f;
    SetSize(Size);
}

}
