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

#include "GroupBox.h"
#include "../Font.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Theme.h"
#include "MarginContainer.h"
#include "Text.h"

namespace OctaneGUI
{

#define MARGIN 12.0f

GroupBox::GroupBox(Window* InWindow)
    : Container(InWindow)
{
    m_Text = AddControl<Text>();
    m_Text->SetPosition({ 8.0f, 0.0f });

    m_Margins = AddControl<MarginContainer>();
    m_Margins->SetMargins({ MARGIN, TopMargin(), MARGIN, MARGIN });
}

Vector2 GroupBox::DesiredSize() const
{
    Vector2 Result = m_Margins->DesiredSize();
    const float LeftOffset = m_Text->GetPosition().X;
    const float TextWidth = m_Text->GetSize().X + LeftOffset;
    if (Result.X < TextWidth)
    {
        Result.X = TextWidth + (LeftOffset * 2.0f);
    }
    return Result;
}

void GroupBox::OnPaint(Paint& Brush) const
{
    const float Offset = m_Text->GetSize().Y * 0.5f;
    Rect OutlineBounds = GetAbsoluteBounds();
    OutlineBounds.Min.Y += Offset;
    const Color Outline = GetProperty(ThemeProperties::PanelOutline).ToColor();
    Brush.RectangleOutline(OutlineBounds, Outline);
    Brush.Rectangle(m_Text->GetAbsoluteBounds(), GetProperty(ThemeProperties::Panel).ToColor());
    Container::OnPaint(Brush);
}

void GroupBox::OnLoad(const Json& Root)
{
    Json Copy = Root;
    Copy["Controls"] = Json();

    Container::OnLoad(Copy);

    m_Text->SetText(Root["Text"].String());

    Copy["ID"] = Json();
    Copy["Controls"] = std::move(Root["Controls"]);

    Rect Margins = Rect::FromJson(Root["Margins"], m_Margins->Margins());
    Margins.Min.Y = TopMargin();
    Copy["Margins"] = Rect::ToJson(Margins);

    m_Margins->OnLoad(Copy);
    m_Margins->SetExpand(Expand::Both);
}

void GroupBox::OnThemeLoaded()
{
    Container::OnThemeLoaded();

    Rect Margins = m_Margins->Margins();
    Margins.Min.Y = TopMargin();
    m_Margins->SetMargins(Margins);
}

float GroupBox::TopMargin() const
{
    return m_Text->GetFont()->Size() * 1.4f;
}

}
