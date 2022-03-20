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
#include "Json.h"
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

Theme& Theme::SetOnThemeLoaded(OnEmptySignature Fn)
{
	m_OnThemeLoaded = Fn;
	return *this;
}

const Variant& Theme::Get(ThemeProperties::Property Index) const
{
	return m_Properties[Index];
}

void Theme::Load(const Json& Root)
{
	Set(ThemeProperties::Text, Root["Text"]);
	Set(ThemeProperties::Text_Disabled, Root["Text_Disabled"]);
	Set(ThemeProperties::Button, Root["Button"]);
	Set(ThemeProperties::Button_Highlight_3D, Root["Button_Highlight_3D"]);
	Set(ThemeProperties::Button_Shadow_3D, Root["Button_Shadow_3D"]);
	Set(ThemeProperties::Button_Hovered, Root["Button_Hovered"]);
	Set(ThemeProperties::Button_Pressed, Root["Button_Pressed"]);
	Set(ThemeProperties::Check, Root["Check"]);
	Set(ThemeProperties::TextSelectable_Text_Hovered, Root["TextSelectable_Text_Hovered"]);
	Set(ThemeProperties::TextSelectable_Hovered, Root["TextSelectable_Hovered"]);
	Set(ThemeProperties::TextInput_Background, Root["TextInput_Background"]);
	Set(ThemeProperties::TextInput_FocusedOutline, Root["TextInput_FocusedOutline"]);
	Set(ThemeProperties::TextInput_Cursor, Root["TextInput_Cursor"]);
	Set(ThemeProperties::TextInput_Selection, Root["TextInput_Selection"]);
	Set(ThemeProperties::Panel, Root["Panel"]);
	Set(ThemeProperties::PanelOutline, Root["PanelOutline"]);
	Set(ThemeProperties::Separator, Root["Separator"]);
	Set(ThemeProperties::ScrollBar, Root["ScrollBar"]);
	Set(ThemeProperties::ScrollBar_Handle, Root["ScrollBar_Handle"]);
	Set(ThemeProperties::ScrollBar_HandleHovered, Root["ScrollBar_HandleHovered"]);

	Set(ThemeProperties::Separator_Thickness, Root["Separator_Thickness"]);
	Set(ThemeProperties::Separator_Margins, Root["Separator_Margins"]);
	Set(ThemeProperties::Menu_RightPadding, Root["Menu_RightPadding"]);
	Set(ThemeProperties::ScrollBar_Size, Root["ScrollBar_Size"]);
	Set(ThemeProperties::ScrollBar_HandleMinSize, Root["ScrollBar_HandleMinSize"]);

	Set(ThemeProperties::Button_Padding, Root["Button_Padding"]);
	Set(ThemeProperties::TextSelectable_Padding, Root["TextSelectable_Padding"]);
	Set(ThemeProperties::TextSelectable_Offset, Root["TextSelectable_Offset"]);
	Set(ThemeProperties::Menu_Margins, Root["Menu_Margins"]);
	Set(ThemeProperties::MenuBar_Padding, Root["MenuBar_Padding"]);

	Set(ThemeProperties::Button_3D, Root["Button_3D"]);
	Set(ThemeProperties::Checkbox_3D, Root["Checkbox_3D"]);

	if (m_OnThemeLoaded)
	{
		m_OnThemeLoaded();
	}
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
	m_Properties[ThemeProperties::Check] = Color(255, 255, 255, 255);
	m_Properties[ThemeProperties::TextSelectable_Text_Hovered] = Color(255, 255, 255, 255);
	m_Properties[ThemeProperties::TextSelectable_Hovered] = Color(48, 63, 169, 255);
	m_Properties[ThemeProperties::TextInput_Background] = Color(14, 14, 14, 255);
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
	m_Properties[ThemeProperties::Checkbox_3D] = false;
}

void Theme::Set(ThemeProperties::Property Property, const Variant& Value)
{
	if (!Value.IsNull())
	{
		m_Properties[Property] = Value;
	}
}

}
