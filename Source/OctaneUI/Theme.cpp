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

#include "Font.h"
#include "Theme.h"
#include "ThemeProperties.h"

namespace OctaneUI
{

Theme::Theme()
{
	InitializeDefault();
}

Theme::~Theme()
{
}

void Theme::SetFont(std::shared_ptr<Font> InFont)
{
	m_Font = InFont;
}

std::shared_ptr<Font> Theme::GetFont() const
{
	return m_Font;
}

const Variant& Theme::Get(ThemeProperties::Property Index) const
{
	return m_Properties[Index];
}

void Theme::InitializeDefault()
{
	m_Properties[ThemeProperties::Text] = Color(255, 255, 255, 255);
	m_Properties[ThemeProperties::Text_Disabled] = Color(105, 105, 105, 255);
	m_Properties[ThemeProperties::Button] = Color(48, 48, 48, 255);
	m_Properties[ThemeProperties::Button_Highlight_3D] = Color(117, 117, 117, 255);
	m_Properties[ThemeProperties::Button_Shadow_3D] = Color(28, 28, 28, 255);
	m_Properties[ThemeProperties::Button_Hovered] = Color(48, 63, 169, 255);
	m_Properties[ThemeProperties::Button_Pressed] = Color(98, 125, 152, 255);
	m_Properties[ThemeProperties::TextSelectable_Hovered] = Color(48, 63, 169, 255);
	m_Properties[ThemeProperties::TextInput_Background] = Color(34, 34, 34, 255);
	m_Properties[ThemeProperties::TextInput_FocusedOutline] = Color(0, 0, 255, 255);
	m_Properties[ThemeProperties::TextInput_Cursor] = Color(255, 255, 255, 255);
	m_Properties[ThemeProperties::TextInput_Selection] = Color(55, 74, 92, 255);
	m_Properties[ThemeProperties::Panel] = Color(33, 33, 33, 255);
	m_Properties[ThemeProperties::PanelOutline] = Color(0, 0, 0, 255);
	m_Properties[ThemeProperties::Separator] = Color(48, 48, 48, 255);
	m_Properties[ThemeProperties::ScrollBar] = Color(48, 48, 48, 255);
	m_Properties[ThemeProperties::ScrollBar_Handle] = Color(64, 64, 64, 255);
	m_Properties[ThemeProperties::ScrollBar_HandleHovered] = Color(96, 96, 96, 255);

	m_Properties[ThemeProperties::Separator_Thickness] = 1.5f;
	m_Properties[ThemeProperties::Separator_Margins] = 8.0f;
	m_Properties[ThemeProperties::Menu_RightPadding] = 60.0f;
	m_Properties[ThemeProperties::ScrollBar_Size] = 15.0f;
	m_Properties[ThemeProperties::ScrollBar_HandleMinSize] = 10.0f;

	m_Properties[ThemeProperties::Button_Padding] = Vector2(12.0f, 6.0f);
	m_Properties[ThemeProperties::TextSelectable_Padding] = Vector2(8.0f, 6.0f);
	m_Properties[ThemeProperties::TextSelectable_Offset] = Vector2(8.0f, 0.0f);
	m_Properties[ThemeProperties::Menu_Margins] = Vector2(0.0f, 8.0f);
	m_Properties[ThemeProperties::MenuBar_Padding] = Vector2(0.0f, 6.0f);

	m_Properties[ThemeProperties::Button_3D] = false;
}

}
