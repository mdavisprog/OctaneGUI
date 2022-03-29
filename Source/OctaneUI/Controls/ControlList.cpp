#include "Control.h"
#include "ControlList.h"

namespace OctaneUI
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

	m_Controls[InControl->GetFullID()] = InControl;
}

bool ControlList::Contains(const char* ID) const
{
	return m_Controls.find(ID) != m_Controls.end();
}

}
