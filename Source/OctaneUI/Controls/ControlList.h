#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace OctaneUI
{
class Control;

class ControlList
{
public:
	ControlList();
	~ControlList();

	void AddControl(const std::shared_ptr<Control>& InControl);
	bool Contains(const char* ID) const;

	template<class T> std::shared_ptr<T> To(const char* ID) const
	{
		std::shared_ptr<T> Result;
		std::weak_ptr<Control> Item = m_Controls.at(ID);
		if (!Item.expired())
		{
			Result = std::dynamic_pointer_cast<T>(Item.lock());
		}

		return Result;
	}

private:
	typedef std::unordered_map<std::string, std::weak_ptr<Control>> Map;

	Map m_Controls;
};

}
