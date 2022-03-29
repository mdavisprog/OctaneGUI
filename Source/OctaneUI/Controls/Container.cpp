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
#include "../Paint.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "Container.h"
#include "HorizontalContainer.h"
#include "Image.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "MarginContainer.h"
#include "Panel.h"
#include "ScrollableContainer.h"
#include "Separator.h"
#include "Text.h"
#include "TextButton.h"
#include "TextInput.h"
#include "TextSelectable.h"
#include "VerticalContainer.h"

#include <algorithm>

namespace OctaneUI
{

Container::Container(Window* InWindow)
	: Control(InWindow)
{
}

Container::~Container()
{
	m_Controls.clear();
}

std::shared_ptr<Control> Container::CreateControl(const std::string& Type)
{
	std::shared_ptr<Control> Result;

	if (Type == Text::TypeName()) { Result = AddControl<Text>(); }
	else if (Type == TextButton::TypeName()) { Result = AddControl<TextButton>(); }
	else if (Type == TextSelectable::TypeName()) { Result = AddControl<TextSelectable>(); }
	else if (Type == TextInput::TypeName()) { Result = AddControl<TextInput>(); }
	else if (Type == CheckBox::TypeName()) { Result = AddControl<CheckBox>(); }
	else if (Type == ComboBox::TypeName()) { Result = AddControl<ComboBox>(); }
	else if (Type == Image::TypeName()) { Result = AddControl<Image>(); }
	else if (Type == ImageButton::TypeName()) { Result = AddControl<ImageButton>(); }
	else if (Type == ListBox::TypeName()) { Result = AddControl<ListBox>(); }
	else if (Type == Panel::TypeName()) { Result = AddControl<Panel>(); }
	else if (Type == Separator::TypeName()) { Result = AddControl<Separator>(); }
	else if (Type == Container::TypeName()) { Result = AddControl<Container>(); }
	else if (Type == HorizontalContainer::TypeName()) { Result = AddControl<HorizontalContainer>(); }
	else if (Type == MarginContainer::TypeName()) { Result = AddControl<MarginContainer>(); }
	else if (Type == ScrollableContainer::TypeName()) { Result = AddControl<ScrollableContainer>(); }
	else if (Type == VerticalContainer::TypeName()) { Result = AddControl<VerticalContainer>(); }

	return Result;
}

Container* Container::InsertControl(const std::shared_ptr<Control>& Item, int Position)
{
	if (HasControl(Item))
	{
		return this;
	}

	Item->SetParent(this);
	Item->SetOnInvalidate([this](Control* Focus, InvalidateType Type)
	{
		if (m_InLayout && Type != InvalidateType::Paint)
		{
			return;
		}

		Container* FocusContainer = dynamic_cast<Container*>(Focus);
		if (FocusContainer == nullptr && (Type == InvalidateType::Layout || Type == InvalidateType::Both))
		{
			// If Focus is a control but is requesting to have a layout update, the owning container should
			// notify the listeners that it should have it's layout updated. The control will still be passed
			// up in case of a repaint request.
			Invalidate(Type);
		}

		Invalidate(Focus, Type);
	});

	if (Position >= 0)
	{
		m_Controls.insert(m_Controls.begin() + Position, Item);
	}
	else
	{
		m_Controls.push_back(Item);
	}

	Invalidate(Item.get(), InvalidateType::Paint);
	Invalidate(InvalidateType::Layout);

	return this;
}

Container* Container::RemoveControl(const std::shared_ptr<Control>& Item)
{
	auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
	if (Iter != m_Controls.end())
	{
		m_Controls.erase(Iter);
		Invalidate(InvalidateType::Both);
	}

	return this;
}

bool Container::HasControl(const std::shared_ptr<Control>& Item) const
{
	std::vector<std::shared_ptr<Control>> AllControls;
	GetControls(AllControls);
	auto Iter = std::find(AllControls.begin(), AllControls.end(), Item);
	return Iter != AllControls.end();
}

void Container::ClearControls()
{
	m_Controls.clear();
	Invalidate(InvalidateType::Both);
}

Container* Container::Layout()
{
	m_InLayout = true;

	PlaceControls(m_Controls);
	
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		const std::shared_ptr<Container> Child = std::dynamic_pointer_cast<Container>(Item);
		if (Child)
		{
			Child->Layout();
		}
	}

	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		Item->Update();
	}

	m_InLayout = false;

	return this;
}

void Container::InvalidateLayout()
{
	Invalidate(InvalidateType::Layout);
}

std::weak_ptr<Control> Container::GetControl(const Vector2& Point) const
{
	std::weak_ptr<Control> Result;

	for (int I = (int)m_Controls.size() - 1; I >= 0; I--)
	{
		const std::shared_ptr<Control>& Item = m_Controls[I];

		const std::shared_ptr<Container> ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			Result = ItemContainer->GetControl(Point);
		}
		else if (Item->Contains(Point))
		{
			Result = Item;
		}

		if (!Result.expired())
		{
			break;
		}
	}

	return Result;
}

void Container::GetControls(std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		Controls.push_back(Item);

		const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			ItemContainer->GetControls(Controls);
		}
	}
}

const std::vector<std::shared_ptr<Control>>& Container::Controls() const
{
	return m_Controls;
}

Vector2 Container::DesiredSize() const
{
	return GetSize();
}

void Container::OnPaint(Paint& Brush) const
{
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		Item->OnPaint(Brush);
	}
}

void Container::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	const Json& Controls = Root["Controls"];

	for (int I = 0; I < Controls.Count(); I++)
	{
		const Json& Item = Controls[I];

		std::string Type = Item["Type"].String();
		std::shared_ptr<Control> NewControl = CreateControl(Type);
		if (NewControl)
		{
			NewControl->OnLoad(Item);
		}
	}
}

void Container::OnThemeLoaded()
{
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		Item->OnThemeLoaded();
	}
}

void Container::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Vector2 ItemSize = Item->GetSize();
		const std::shared_ptr<Container> ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			ItemSize = ItemContainer->DesiredSize();
		}
		Expand Direction = Item->GetExpand();

		switch (Direction)
		{
		case Expand::Both: ItemSize = GetSize(); break;
		case Expand::Width: ItemSize.X = GetSize().X; break;
		case Expand::Height: ItemSize.Y = GetSize().Y; break;
		case Expand::None:
		default: break;
		}

		Item->SetSize(ItemSize);
	}
}

}
