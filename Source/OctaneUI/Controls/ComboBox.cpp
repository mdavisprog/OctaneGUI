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

#include "../Icons.h"
#include "../Json.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ComboBox.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneUI
{

ComboBox::ComboBox(Window* InWindow)
	: HorizontalContainer(InWindow)
{
	SetSpacing({0.0f, 0.0f});

	m_Input = AddControl<TextInput>();
	m_Input->SetReadOnly(true);

	m_Button = AddControl<ImageButton>();
	m_Button
		->SetTexture(InWindow->GetIcons()->GetTexture())
		.SetUVs(InWindow->GetIcons()->GetUVs(Icons::Type::ArrowDown))
		.SetOnPressed([this](const Button&)
		{
			if (m_State == State::Closed)
			{
				m_State = State::Open;
				GetWindow()->SetPopup(m_List, [this](const Container& Focus) -> void
				{
					if (m_Button->IsPressed())
					{
						m_State = State::Closing;
					}
					else
					{
						m_State = State::Closed;
					}
				});
			}
			else
			{
				m_State = State::Closed;
			}
		})
		.SetExpand(Expand::Height);
	
	m_List = std::make_shared<ListBox>(InWindow);
	m_List
		->SetOnSelect([this](int Index, std::weak_ptr<Control> Item) -> void
		{
			if (Item.expired())
			{
				return;
			}

			std::shared_ptr<Text> TextItem = std::dynamic_pointer_cast<Text>(Item.lock());
			if (TextItem)
			{
				m_Input->SetText(TextItem->GetText());
			}
			else
			{
				m_Input->SetText((std::string("Item ") + std::to_string(Index)).c_str());
			}
		})
		->SetParent(this)
		->SetPosition({0.0f, m_Input->GetSize().Y});
}

std::shared_ptr<Text> ComboBox::AddItem(const char* Item)
{
	std::shared_ptr<Text> Result = m_List->AddItem<Text>();
	Result->SetText(Item);
	return Result;
}

void ComboBox::Update()
{
	const Vector2 ContentSize = m_List->ListSize();
	m_List->SetSize({GetSize().X, std::min<float>(ContentSize.Y, 200.0f)});
}

void ComboBox::OnLoad(const Json& Root)
{
	Json List = Root;
	List["Controls"] = Json();

	Container::OnLoad(List);

	const Json& Items = Root["Items"];
	for (int I = 0; I < Items.Count(); I++)
	{
		const Json& Item = Items[I];

		if (Item.IsString())
		{
			AddItem(Item.String());
		}
	}
}

}
