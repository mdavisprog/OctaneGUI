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

#include "Variant.h"

#include <unordered_map>

namespace OctaneUI
{

class ThemeProperties
{
public:
	enum Property
	{
		// Colors
		Text,
		Text_Disabled,
		Button,
		Button_Highlight_3D,
		Button_Shadow_3D,
		Button_Hovered,
		Button_Pressed,
		Check,
		ImageButton,
		TextSelectable_Text_Hovered,
		TextSelectable_Hovered,
		TextInput_Background,
		TextInput_FocusedOutline,
		TextInput_Cursor,
		TextInput_Selection,
		Panel,
		PanelOutline,
		Separator,
		ScrollBar,
		ScrollBar_Handle,
		ScrollBar_HandleHovered,

		// Floats
		Separator_Thickness,
		Separator_Margins,
		Menu_RightPadding,
		ScrollBar_Size,
		ScrollBar_HandleMinSize,
		FontSize,

		// Vector2
		Button_Padding,
		TextSelectable_Padding,
		TextSelectable_Offset,
		Menu_Margins,
		MenuBar_Padding,

		// Flags
		Button_3D,
		TextInput_3D,
		ScrollBar_3D,

		// Strings
		FontPath,

		Max
	};

	ThemeProperties();

	bool Has(Property Index) const;
	void Clear(Property Index);

	Variant& operator[](Property Index);
	const Variant& operator[](Property Index) const;

private:
	std::unordered_map<Property, Variant> m_Properties {};
};

}
