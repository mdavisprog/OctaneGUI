/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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
#include "../Assert.h"
#include "../Icons.h"
#include "../Json.h"
#include "../String.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "ScrollableContainer.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneGUI
{

ComboBox::ComboBox(Window* InWindow)
    : HorizontalContainer(InWindow)
{
    SetSpacing({ 0.0f, 0.0f });

    m_Input = AddControl<TextInput>();
    m_Input
        ->SetReadOnly(true)
        .SetMultiline(false);

    const Vector2 InputSize = m_Input->GetSize();
    m_Button = AddControl<ImageButton>();
    m_Button
        ->SetTexture(InWindow->GetIcons()->GetTexture())
        .SetUVs(InWindow->GetIcons()->GetUVs(Icons::Type::ArrowDown))
        .SetOnPressed([this](Button&)
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
        .SetSize({ InputSize.Y, InputSize.Y })
        .SetExpand(Expand::Height);

    m_List = std::make_shared<ListBox>(InWindow);
    m_List
        ->SetOnSelect([this](int Index, std::weak_ptr<Control>) -> void
            {
                if (GetWindow()->GetContainer() == m_List)
                {
                    GetWindow()->ClosePopup();
                }

                if (Index == SelectedIndex())
                {
                    return;
                }

                SetSelectedIndex(Index);

                if (m_OnSelected)
                {
                    m_OnSelected(m_Input->GetText());
                }
            })
        .SetParent(this)
        .SetPosition({ 0.0f, m_Input->GetSize().Y });

    OnThemeLoaded();
}

ComboBox& ComboBox::SetExpandCB(Expand InExpand)
{
    SetExpand(InExpand);

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

ComboBox& ComboBox::SetWidth(float Width)
{
    m_Input->SetSize({ Width, m_Input->GetSize().Y });
    return *this;
}

ComboBox& ComboBox::SetSelected(const char32_t* InText)
{
    m_Input->SetText(InText);
    return *this;
}

ComboBox& ComboBox::SetSelectedIndex(int Index)
{
    Assert(Index >= 0 && Index < m_List->Count(), "Index out of range! Index: %d Count: %d", Index, m_List->Count());
    m_SelectedIndex = Index;
    std::shared_ptr<Text> TextItem = std::dynamic_pointer_cast<Text>(m_List->Item((size_t)Index));
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
    return *this;
}

ComboBox& ComboBox::Clear()
{
    m_List->ClearItems();
    m_SelectedIndex = -1;
    return *this;
}

std::shared_ptr<Text> ComboBox::AddItem(const char* Item)
{
    return AddItem(String::ToUTF32(Item).c_str());
}

std::shared_ptr<Text> ComboBox::AddItem(const char32_t* Item)
{
    std::shared_ptr<Text> Result = m_List->AddItem<Text>();
    Result->SetText(Item);
    return Result;
}

int ComboBox::SelectedIndex() const
{
    return m_SelectedIndex;
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
    Vector2 ContentSize = m_List->ListSize();

    // TODO: Should this logic be applied to all ScrollableViewControls?
    const float AlwaysPaint = m_List->Scrollable()->GetProperty(ThemeProperties::ScrollBar_AlwaysPaint).Bool();
    if (AlwaysPaint)
    {
        const float SBSize = m_List->Scrollable()->GetProperty(ThemeProperties::ScrollBar_Size).Float();
        ContentSize += Vector2(SBSize, SBSize);
    }

    m_List->SetSize({ GetSize().X, std::min<float>(ContentSize.Y, 200.0f) });
}

void ComboBox::OnLoad(const Json& Root)
{
    Json List = Root;
    List["Controls"] = Json();

    Container::OnLoad(List);

    const Json& Items = Root["Items"];
    for (unsigned int I = 0; I < Items.Count(); I++)
    {
        const Json& Item = Items[I];

        if (Item.IsString())
        {
            AddItem(Item.String());
        }
    }

    SetExpandCB(GetExpand());

    OnThemeLoaded();
}

void ComboBox::OnThemeLoaded()
{
    HorizontalContainer::OnThemeLoaded();

    m_Button->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
    m_Button->OnThemeLoaded();

    m_List->OnThemeLoaded();
}

}
