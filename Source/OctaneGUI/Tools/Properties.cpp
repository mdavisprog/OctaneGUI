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
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/Text.h"
#include "../Controls/VerticalContainer.h"
#include "../Json.h"

#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

Properties::Properties(Window* InWindow)
	: Container(InWindow)
{
	AddControl<Panel>()->SetExpand(Expand::Both);

	std::shared_ptr<ScrollableContainer> Scrollable = AddControl<ScrollableContainer>();
	m_List = Scrollable->AddControl<VerticalContainer>();
	m_List->SetExpand(Expand::Both);

	SetSize({ 150.0f, 0.0f });
	SetExpand(Expand::Height);
}

void Properties::Parse(const Json& Root)
{
	m_List->ClearControls();

	Root.ForEach([this](const std::string& Key, const Json& Value) -> void
		{
			std::stringstream Stream;
			Stream << Key
				   << ": "
				   << Value.ToString();

			m_List->AddControl<Text>()->SetText(Stream.str().c_str());
		});
}

}
}
