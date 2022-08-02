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

#include "ComboBox.h"
#include "../Icons.h"
#include "../Json.h"
#include "../String.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneGUI
{

ComboBox::ComboBox(Window* InWindow)
    : HorizontalContainer(InWindow)
{
    SetSpacing({ 0.0f, 0.0f });

    m_Input = AddControl<TextInput>();
    m_Input->SetReadOnly(true);

    m_Button = AddControl<ImageButton>();
    m_Button
        ->SetTexture(InWindow->GetIcons()->GetTexture())
        .SetUVs(InWindow->GetIcons()->GetUVs(Icons::Type::ArrowDown))
        .SetOnPressed([this](const Button&)
            {
                if (GetWindow()->GetPopup() == m_List)
                {
                    GetWindow()->ClosePopup();
                }
                else
                {
                    // Update the list size before opening the popup.
                    Update();
                    GetWindow()->SetPopup(m_List);
                }
            })
        .SetExpand(Expand::Height);

    // Setup the color to use by default for the image button.
    OnThemeLoaded();

    m_List = std::make_shared<ListBox>(InWindow);
    m_List
        ->SetOnSelect([this](int Index, std::weak_ptr<Control> Item) -> void
            {
                if (GetWindow()->GetContainer() == m_List)
                {
                    GetWindow()->ClosePopup();
                }

                if (Item.expired())
                {
                    return;
                }

                std::shared_ptr<Text> TextItem = std::dynamic_pointer_cast<Text>(Item.lock());
                std::u32string ItemText;
                if (TextItem)
                {
                    ItemText = TextItem->GetText();
                }
                else
                {
                    ItemText = String::ToUTF32(std::string("Item ") + std::to_string(Index));
                }

                SetSelected(ItemText.c_str());

                if (m_OnSelected)
                {
                    m_OnSelected(ItemText);
                }
            })
        .SetParent(this)
        ->SetPosition({ 0.0f, m_Input->GetSize().Y });
}

ComboBox& ComboBox::SetExpand(Expand InExpand)
{
    Control::SetExpand(InExpand);

    if (InExpand == Expand::Width || InExpand == Expand::Both)
    {
        m_Input->SetExpand(Expand::Width);
    }
    else
    {
        m_Input->SetExpand(Expand::None);
    }

    return *this;
}

ComboBox& ComboBox::SetSelected(const char32_t* InText)
{
    m_Input->SetText(InText);
    return *this;
}

std::shared_ptr<Text> ComboBox::AddItem(const char* Item)
{
    std::shared_ptr<Text> Result = m_List->AddItem<Text>();
    Result->SetText(Item);
    return Result;
}

void ComboBox::Close()
{
    if (IsOpen())
    {
        GetWindow()->ClosePopup();
    }
}

bool ComboBox::IsOpen() const
{
    return GetWindow()->GetPopup() == m_List;
}

ComboBox& ComboBox::SetOnSelected(OnSelectedSignature&& Fn)
{
    m_OnSelected = std::move(Fn);
    return *this;
}

void ComboBox::Update()
{
    const Vector2 ContentSize = m_List->ListSize();
    m_List->SetSize({ GetSize().X, std::min<float>(ContentSize.Y, 200.0f) });
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

    SetExpand(GetExpand());

    OnThemeLoaded();
}

void ComboBox::OnThemeLoaded()
{
    m_Button->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
    m_Button->OnThemeLoaded();
}

}
