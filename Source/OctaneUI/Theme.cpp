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

namespace OctaneUI
{

Theme::ColorOverride::ColorOverride(const std::shared_ptr<Theme>& InTheme, Colors Key, Color Value)
	: m_Theme(InTheme)
	, m_Key(Key)
{
	if (m_Theme)
	{
		m_Restore = m_Theme->GetColor(m_Key);
		m_Theme->m_Colors[m_Key] = Value;
	}
}

Theme::ColorOverride::~ColorOverride()
{
	if (m_Theme)
	{
		m_Theme->m_Colors[m_Key] = m_Restore;
	}
}

Theme::FloatOverride::FloatOverride(const std::shared_ptr<Theme>& InTheme, FloatConstants Key, float Value)
	: m_Theme(InTheme)
	, m_Key(Key)
{
	if (m_Theme)
	{
		m_Restore = m_Theme->GetConstant(m_Key);
		m_Theme->m_FloatConstants[m_Key] = Value;
	}
}

Theme::FloatOverride::~FloatOverride()
{
	if (m_Theme)
	{
		m_Theme->m_FloatConstants[m_Key] = m_Restore;
	}
}

Theme::Vector2Override::Vector2Override(const std::shared_ptr<Theme>& InTheme, Vector2Constants Key, const Vector2& Value)
	: m_Theme(InTheme)
	, m_Key(Key)
{
	if (m_Theme)
	{
		m_Restore = m_Theme->GetConstant(m_Key);
		m_Theme->m_Vector2Constants[m_Key] = Value;
	}
}

Theme::Vector2Override::~Vector2Override()
{
	if (m_Theme)
	{
		m_Theme->m_Vector2Constants[m_Key] = m_Restore;
	}
}

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

Color Theme::GetColor(Colors Index) const
{
	if (m_Colors.find(Index) == m_Colors.end())
	{
		return Color(0, 0, 0, 255);
	}

	return m_Colors.at(Index);
}

float Theme::GetConstant(FloatConstants Index) const
{
	if (m_FloatConstants.find(Index) == m_FloatConstants.end())
	{
		return 0.0f;
	}

	return m_FloatConstants.at(Index);
}

Vector2 Theme::GetConstant(Vector2Constants Index) const
{
	if (m_Vector2Constants.find(Index) == m_Vector2Constants.end())
	{
		return Vector2();
	}

	return m_Vector2Constants.at(Index);
}

void Theme::InitializeDefault()
{
	m_Colors[Colors::Text] = Color(255, 255, 255, 255);
	m_Colors[Colors::Text_Disabled] = Color(105, 105, 105, 255);
	m_Colors[Colors::Button] = Color(48, 48, 48, 255);
	m_Colors[Colors::Button_Hovered] = Color(48, 63, 169, 255);
	m_Colors[Colors::Button_Pressed] = Color(98, 125, 152, 255);
	m_Colors[Colors::TextSelectable_Hovered] = Color(48, 63, 169, 255);
	m_Colors[Colors::TextInput_Background] = Color(64, 64, 64, 255);
	m_Colors[Colors::TextInput_FocusedOutline] = Color(0, 0, 255, 255);
	m_Colors[Colors::TextInput_Cursor] = Color(255, 255, 255, 255);
	m_Colors[Colors::TextInput_Selection] = Color(72, 101, 129, 130);
	m_Colors[Colors::Panel] = Color(33, 33, 33, 255);
	m_Colors[Colors::PanelOutline] = Color(0, 0, 0, 255);
	m_Colors[Colors::Separator] = Color(48, 48, 48, 255);
	m_Colors[Colors::ScrollBar] = Color(48, 48, 48, 255);
	m_Colors[Colors::ScrollBar_Handle] = Color(64, 64, 64, 255);
	m_Colors[Colors::ScrollBar_HandleHovered] = Color(96, 96, 96, 255);

	m_FloatConstants[FloatConstants::Separator_Thickness] = 1.5f;
	m_FloatConstants[FloatConstants::Separator_Margins] = 8.0f;
	m_FloatConstants[FloatConstants::Menu_RightPadding] = 60.0f;
	m_FloatConstants[FloatConstants::ScrollBar_Size] = 15.0f;
	m_FloatConstants[FloatConstants::ScrollBar_HandleMinSize] = 10.0f;

	m_Vector2Constants[Vector2Constants::Button_Padding] = Vector2(12.0f, 6.0f);
	m_Vector2Constants[Vector2Constants::TextSelectable_Padding] = Vector2(8.0f, 6.0f);
	m_Vector2Constants[Vector2Constants::TextSelectable_Offset] = Vector2(8.0f, 0.0f);
	m_Vector2Constants[Vector2Constants::Menu_Margins] = Vector2(0.0f, 8.0f);
	m_Vector2Constants[Vector2Constants::MenuBar_Padding] = Vector2(0.0f, 6.0f);
}

}
