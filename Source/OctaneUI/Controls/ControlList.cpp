#include "Button.h"
#include "Checkbox.h"
#include "Container.h"
#include "Control.h"
#include "ControlList.h"
#include "Image.h"
#include "Menu.h"
#include "MenuBar.h"
#include "MenuItem.h"
#include "Text.h"
#include "TextInput.h"
#include "TextSelectable.h"

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
