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

std::shared_ptr<Button> ControlList::ToButton(const char* ID) const
{
	return std::dynamic_pointer_cast<Button>(m_Controls.at(ID));
}

std::shared_ptr<Checkbox> ControlList::ToCheckbox(const char* ID) const
{
	return std::dynamic_pointer_cast<Checkbox>(m_Controls.at(ID));
}

std::shared_ptr<Container> ControlList::ToContainer(const char* ID) const
{
	return std::dynamic_pointer_cast<Container>(m_Controls.at(ID));
}

std::shared_ptr<Control> ControlList::ToControl(const char* ID) const
{
	return m_Controls.at(ID);
}

std::shared_ptr<Image> ControlList::ToImage(const char* ID) const
{
	return std::dynamic_pointer_cast<Image>(m_Controls.at(ID));
}

std::shared_ptr<Menu> ControlList::ToMenu(const char* ID) const
{
	return std::dynamic_pointer_cast<Menu>(m_Controls.at(ID));
}

std::shared_ptr<MenuBar> ControlList::ToMenuBar(const char* ID) const
{
	return std::dynamic_pointer_cast<MenuBar>(m_Controls.at(ID));
}

std::shared_ptr<MenuItem> ControlList::ToMenuItem(const char* ID) const
{
	return std::dynamic_pointer_cast<MenuItem>(m_Controls.at(ID));
}

std::shared_ptr<Text> ControlList::ToText(const char* ID) const
{
	return std::dynamic_pointer_cast<Text>(m_Controls.at(ID));
}

std::shared_ptr<TextInput> ControlList::ToTextInput(const char* ID) const
{
	return std::dynamic_pointer_cast<TextInput>(m_Controls.at(ID));
}

std::shared_ptr<TextSelectable> ControlList::ToTextSelectable(const char* ID) const
{
	return std::dynamic_pointer_cast<TextSelectable>(m_Controls.at(ID));
}

}
