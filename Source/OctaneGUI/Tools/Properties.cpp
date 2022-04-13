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

#include "Properties.h"
#include "../Controls/HorizontalContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/Text.h"
#include "../Controls/VerticalContainer.h"
#include "../Json.h"

namespace OctaneGUI
{
namespace Tools
{

Properties::Properties(Window* InWindow)
	: Container(InWindow)
{
	AddControl<Panel>()->SetExpand(Expand::Both);

	std::shared_ptr<ScrollableContainer> Scrollable = AddControl<ScrollableContainer>();
	std::shared_ptr<HorizontalContainer> Row = Scrollable->AddControl<HorizontalContainer>();
	Row->SetSpacing({ 8.0f, 0.0f });
	m_KeyList = Row->AddControl<VerticalContainer>();
	m_ValueList = Row->AddControl<VerticalContainer>();

	SetSize({ 150.0f, 0.0f });
	SetExpand(Expand::Height);
}

void Properties::Parse(const Json& Root)
{
	m_KeyList->ClearControls();
	m_ValueList->ClearControls();

	Root.ForEach([this](const std::string& Key, const Json& Value) -> void
		{
			Parse(Key, Value);
		});
}

void Properties::Parse(const std::string& Key, const Json& Value, int Indent)
{
	const std::string IndentStr(Indent, ' ');
	m_KeyList->AddControl<Text>()->SetText((IndentStr + Key).c_str());
	if (Value.IsObject())
	{
		m_ValueList->AddControl<Text>()->SetText(" ");

		Value.ForEach([this, Indent](const std::string& Key, const Json& Value) -> void
			{
				Parse(Key, Value, Indent + 4);
			});
	}
	else
	{
		m_ValueList->AddControl<Text>()->SetText(Value.ToString().c_str());
	}
}

}
}
