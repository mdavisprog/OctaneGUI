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

#include "../Json.h"
#include "ListBox.h"
#include "Panel.h"
#include "ScrollableContainer.h"
#include "VerticalContainer.h"

namespace OctaneUI
{

ListBox::ListBox(Window* InWindow)
	: Container(InWindow)
{
	m_Panel = AddPanel();
	m_Panel->SetExpand(Expand::Both);

	m_Scrollable = AddControl<ScrollableContainer>();
	m_Scrollable->SetExpand(Expand::Both);
	
	m_List = m_Scrollable->AddControl<VerticalContainer>();

	SetSize({200.0f, 200.0f});
}

void ListBox::OnLoad(const Json& Root)
{
	Json Copy = Root;
	Copy["Controls"] = Json();

	Container::OnLoad(Copy);

	Json List;
	List["Controls"] = Root["Controls"];
	m_List->OnLoad(List);
}

}
