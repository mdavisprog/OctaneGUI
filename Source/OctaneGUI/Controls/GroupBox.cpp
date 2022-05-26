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
	m_Margins->SetPosition({ 0.0f, m_Text->GetFont()->Size() });
	m_Margins->SetMargins({ MARGIN, MARGIN, MARGIN, MARGIN });
}

Vector2 GroupBox::DesiredSize() const
{
	return m_Margins->DesiredSize() + m_Margins->GetPosition();
}

void GroupBox::OnPaint(Paint& Brush) const
{
	const Color Outline = GetProperty(ThemeProperties::PanelOutline).ToColor();
	Rect OutlineBounds = GetAbsoluteBounds().Move({ 0.0f, m_Text->GetSize().Y * 0.5f });
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

	const Rect Margins = Rect::FromJson(Root["Margins"], m_Margins->Margins());
	Copy["Margins"] = std::move(Rect::ToJson(Margins));

	m_Margins->OnLoad(Copy);
}

}
