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

#include "ControlList.h"
#include "../Assert.h"
#include "Control.h"

namespace OctaneGUI
{

ControlList::ControlList()
{
}

ControlList::~ControlList()
{
}

void ControlList::AddControl(const std::shared_ptr<Control>& InControl)
{
	if (!InControl || !InControl->HasID())
	{
		return;
	}

	Assert(!Contains(InControl->GetFullID().c_str()), "Control with ID '%s' already exists!", InControl->GetFullID().c_str());
	m_Controls[InControl->GetFullID()] = InControl;
}

void ControlList::AddControls(const std::vector<std::shared_ptr<Control>>& Controls)
{
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		if (Item->HasID())
		{
			AddControl(Item);
		}
	}
}

bool ControlList::Contains(const char* ID) const
{
	return m_Controls.find(ID) != m_Controls.end();
}

std::weak_ptr<Control> ControlList::Get(const char* ID) const
{
	if (!Contains(ID))
	{
		std::string List;
		for (const std::pair<std::string, std::weak_ptr<Control>>& Item : m_Controls)
		{
			List += Item.first + "\n";
		}
		Assert(Contains(ID), "Control with ID '%s' does not exist! Below is a list of existing IDs:\n%s", ID, List.c_str());
	}
	return m_Controls.at(ID);
}

std::vector<std::string> ControlList::IDs() const
{
	std::vector<std::string> Result;

	for (Map::const_iterator It = m_Controls.begin(); It != m_Controls.end(); ++It)
	{
		Result.push_back(It->first);
	}

	return Result;
}

}
