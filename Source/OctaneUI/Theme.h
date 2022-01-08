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

#pragma once

#include "Color.h"
#include "Vector2.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace OctaneUI
{

class Font;

class Theme
{
public:
	enum class Colors
	{
		Text,
		Button,
		Button_Hovered,
		Button_Pressed,
		TextSelectable_Hovered,
		Panel,
		PanelOutline,
		Separator
	};

	enum class FloatConstants
	{
		Separator_Thickness,
		Separator_Margins,
		Menu_RightPadding,
	};

	enum class Vector2Constants
	{
		Button_Padding,
		TextSelectable_Padding,
		TextSelectable_Offset,
		Menu_Margins,
		MenuBar_Padding
	};

	Theme();
	~Theme();

	void SetFont(std::shared_ptr<Font> InFont);
	std::shared_ptr<Font> GetFont() const;

	Color GetColor(Colors Index) const;
	float GetConstant(FloatConstants Index) const;
	Vector2 GetConstant(Vector2Constants Index) const;

private:
	typedef std::unordered_map<Colors, Color> ColorMap;
	typedef std::unordered_map<FloatConstants, float> FloatMap;
	typedef std::unordered_map<Vector2Constants, Vector2> Vector2Map;

	void InitializeDefault();

	std::shared_ptr<Font> m_Font;
	ColorMap m_Colors;
	FloatMap m_FloatConstants;
	Vector2Map m_Vector2Constants;
};

}
