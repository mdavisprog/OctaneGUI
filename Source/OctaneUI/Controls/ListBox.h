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

#include "Container.h"

namespace OctaneUI
{

class ListBoxInteraction;
class Panel;
class ScrollableContainer;
class VerticalContainer;

class ListBox : public Container
{
	CLASS(ListBox)

public:
	typedef std::function<void(int, std::weak_ptr<Control>)> OnSelectSignature;

	ListBox(Window* InWindow);

	template <typename T, typename... TArgs>
	std::shared_ptr<T> AddItem(TArgs... Args)
	{
		std::shared_ptr<T> Result = std::make_shared<T>(GetWindow(), Args...);
		InsertItem(Result);
		return Result;
	}

	int Index() const;
	ListBox* SetOnSelect(OnSelectSignature Fn);
	Vector2 ListSize() const;

	virtual void OnLoad(const Json& Root) override;
	virtual void OnPaint(Paint& Brush) const override;

private:
	using Container::AddControl;
	using Container::InsertControl;

	void InsertItem(const std::shared_ptr<Control>& Item);
	void PaintItem(Paint& Brush, const std::shared_ptr<Control>& Item) const;

	std::shared_ptr<Panel> m_Panel { nullptr };
	std::shared_ptr<ScrollableContainer> m_Scrollable { nullptr };
	std::shared_ptr<VerticalContainer> m_List { nullptr };
	std::shared_ptr<ListBoxInteraction> m_Interaction { nullptr };
	OnSelectSignature m_OnSelect { nullptr };
};

}
