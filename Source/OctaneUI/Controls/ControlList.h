#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace OctaneUI
{

class Button;
class Checkbox;
class Container;
class Control;
class Image;
class Menu;
class MenuBar;
class MenuItem;
class Text;
class TextInput;
class TextSelectable;

class ControlList
{
public:
	ControlList();
	~ControlList();

	void AddControl(const std::shared_ptr<Control>& InControl);

	std::shared_ptr<Button> ToButton(const char* ID) const;
	std::shared_ptr<Checkbox> ToCheckbox(const char* ID) const;
	std::shared_ptr<Container> ToContainer(const char* ID) const;
	std::shared_ptr<Control> ToControl(const char* ID) const;
	std::shared_ptr<Image> ToImage(const char* ID) const;
	std::shared_ptr<Menu> ToMenu(const char* ID) const;
	std::shared_ptr<MenuBar> ToMenuBar(const char* ID) const;
	std::shared_ptr<MenuItem> ToMenuItem(const char* ID) const;
	std::shared_ptr<Text> ToText(const char* ID) const;
	std::shared_ptr<TextInput> ToTextInput(const char* ID) const;
	std::shared_ptr<TextSelectable> ToTextSelectable(const char* ID) const;

private:
	typedef std::unordered_map<std::string, std::shared_ptr<Control>> Map;

	Map m_Controls;
};

}
