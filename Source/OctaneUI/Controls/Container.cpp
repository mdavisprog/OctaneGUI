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
#include "../Theme.h"
#include "Checkbox.h"
#include "Container.h"
#include "HorizontalContainer.h"
#include "Image.h"
#include "ImageButton.h"
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
	, m_UpdateLayout(false)
{
}

Container::~Container()
{
	m_Controls.clear();
}

std::shared_ptr<Text> Container::AddText(const char* InText)
{
	std::shared_ptr<Text> Result = AddControl<Text>();
	Result->SetText(InText);
	return Result;
}

std::shared_ptr<TextInput> Container::AddTextInput()
{
	std::shared_ptr<TextInput> Result = AddControl<TextInput>();
	return Result;
}

std::shared_ptr<TextSelectable> Container::AddTextSelectable(const char* InText)
{
	std::shared_ptr<TextSelectable> Result = AddControl<TextSelectable>();
	Result->SetText(InText);
	return Result;
}

std::shared_ptr<Button> Container::AddButton(const char* InText)
{
	std::shared_ptr<TextButton> Result = AddControl<TextButton>();
	Result->SetText(InText);
	return Result;
}

std::shared_ptr<Checkbox> Container::AddCheckbox(const char* InText)
{
	std::shared_ptr<Checkbox> Result = AddControl<Checkbox>();
	Result->SetText(InText);
	return Result;
}

std::shared_ptr<Image> Container::AddImage(const char* Path)
{
	std::shared_ptr<Image> Result = AddControl<Image>();
	Result->SetTexture(Path);
	return Result;
}

std::shared_ptr<Panel> Container::AddPanel()
{
	return AddControl<Panel>();
}

bool Container::ShouldUpdateLayout() const
{
	return m_UpdateLayout;
}

std::shared_ptr<Container> Container::AddContainer()
{
	return AddControl<Container>();
}

std::shared_ptr<HorizontalContainer> Container::AddHorizontalContainer()
{
	return AddControl<HorizontalContainer>();
}

std::shared_ptr<VerticalContainer> Container::AddVerticalContainer()
{
	return AddControl<VerticalContainer>();
}

std::shared_ptr<Control> Container::CreateControl(const std::string& Type)
{
	std::shared_ptr<Control> Result;

	if (Type == Text::TypeName()) { Result = AddControl<Text>(); }
	else if (Type == TextButton::TypeName()) { Result = AddControl<TextButton>(); }
	else if (Type == TextSelectable::TypeName()) { Result = AddControl<TextSelectable>(); }
	else if (Type == TextInput::TypeName()) { Result = AddControl<TextInput>(); }
	else if (Type == Checkbox::TypeName()) { Result = AddControl<Checkbox>(); }
	else if (Type == Image::TypeName()) { Result = AddControl<Image>(); }
	else if (Type == ImageButton::TypeName()) { Result = AddControl<ImageButton>(); }
	else if (Type == Panel::TypeName()) { Result = AddControl<Panel>(); }
	else if (Type == Separator::TypeName()) { Result = AddControl<Separator>(); }
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
	Item->SetOnInvalidate([=](Control* Focus, InvalidateType Type)
	{
		OnInvalidate(Focus, Type);
	});

	if (Position >= 0)
	{
		m_Controls.insert(m_Controls.begin() + Position, Item);
	}
	else
	{
		m_Controls.push_back(Item);
	}

	OnInvalidate(Item.get(), InvalidateType::Both);

	return this;
}

Container* Container::RemoveControl(const std::shared_ptr<Control>& Item)
{
	auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
	if (Iter != m_Controls.end())
	{
		m_Controls.erase(Iter);
		Invalidate();
	}

	return this;
}

bool Container::HasControl(const std::shared_ptr<Control>& Item) const
{
	auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
	return Iter != m_Controls.end();
}

void Container::ClearControls()
{
	m_Controls.clear();
	m_UpdateLayout = true;
	Invalidate(InvalidateType::Both);
}

Container* Container::Layout()
{
	if (m_UpdateLayout)
	{
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

		m_UpdateLayout = false;
	}

	return this;
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

void Container::InvalidateLayout()
{
	m_UpdateLayout = true;
	Invalidate(InvalidateType::Layout);
}

Vector2 Container::GetPotentialSize(int& ExpandedControls) const
{
	Vector2 Result = GetSize();

	ExpandedControls = 0;
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		const Vector2 Size = Item->GetSize();

		switch (Item->GetExpand())
		{
		case Expand::Both:
		case Expand::Width:
		case Expand::Height: ExpandedControls++; break;
		case Expand::None:
		default: Result -= Size; break;
		}
	}

	ExpandedControls = std::max<int>(ExpandedControls, 1);

	return Result;
}

void Container::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Vector2 ItemSize = Item->GetSize();
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

void Container::OnInvalidate(Control* Focus, InvalidateType Type)
{
	m_UpdateLayout = Type == InvalidateType::Layout || Type == InvalidateType::Both || m_UpdateLayout;
	Invalidate(Type);
}

}
