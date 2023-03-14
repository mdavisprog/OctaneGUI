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

#include "Container.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "ControlList.h"
#include "GroupBox.h"
#include "HorizontalContainer.h"
#include "Image.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "MarginContainer.h"
#include "Panel.h"
#include "RadioButton.h"
#include "ScrollableContainer.h"
#include "ScrollableViewControl.h"
#include "Separator.h"
#include "Spinner.h"
#include "Splitter.h"
#include "Table.h"
#include "Text.h"
#include "TextButton.h"
#include "TextEditor.h"
#include "TextInput.h"
#include "TextSelectable.h"
#include "Tree.h"
#include "VerticalContainer.h"
#include "WindowContainer.h"

#include <algorithm>
#include <cassert>

namespace OctaneGUI
{

Container::Container(Window* InWindow)
    : Control(InWindow)
{
    SetForwardKeyEvents(true);
}

Container::~Container()
{
    m_Controls.clear();
}

std::shared_ptr<Control> Container::CreateControl(const std::string& Type)
{
    std::shared_ptr<Control> Result;

    if (Type == CheckBox::TypeName())
    {
        Result = AddControl<CheckBox>();
    }
    else if (Type == ComboBox::TypeName())
    {
        Result = AddControl<ComboBox>();
    }
    else if (Type == Spinner::TypeName())
    {
        Result = AddControl<Spinner>();
    }
    else if (Type == Container::TypeName())
    {
        Result = AddControl<Container>();
    }
    else if (Type == GroupBox::TypeName())
    {
        Result = AddControl<GroupBox>();
    }
    else if (Type == HorizontalContainer::TypeName())
    {
        Result = AddControl<HorizontalContainer>();
    }
    else if (Type == Image::TypeName())
    {
        Result = AddControl<Image>();
    }
    else if (Type == ImageButton::TypeName())
    {
        Result = AddControl<ImageButton>();
    }
    else if (Type == ListBox::TypeName())
    {
        Result = AddControl<ListBox>();
    }
    else if (Type == MarginContainer::TypeName())
    {
        Result = AddControl<MarginContainer>();
    }
    else if (Type == Panel::TypeName())
    {
        Result = AddControl<Panel>();
    }
    else if (Type == RadioButton::TypeName())
    {
        Result = AddControl<RadioButton>();
    }
    else if (Type == ScrollableContainer::TypeName())
    {
        Result = AddControl<ScrollableContainer>();
    }
    else if (Type == ScrollableViewControl::TypeName())
    {
        Result = AddControl<ScrollableViewControl>();
    }
    else if (Type == Separator::TypeName())
    {
        Result = AddControl<Separator>();
    }
    else if (Type == Splitter::TypeName())
    {
        Result = AddControl<Splitter>();
    }
    else if (Type == Table::TypeName())
    {
        Result = AddControl<Table>();
    }
    else if (Type == Text::TypeName())
    {
        Result = AddControl<Text>();
    }
    else if (Type == TextButton::TypeName())
    {
        Result = AddControl<TextButton>();
    }
    else if (Type == TextEditor::TypeName())
    {
        Result = AddControl<TextEditor>();
    }
    else if (Type == TextInput::TypeName())
    {
        Result = AddControl<TextInput>();
    }
    else if (Type == TextSelectable::TypeName())
    {
        Result = AddControl<TextSelectable>();
    }
    else if (Type == Tree::TypeName())
    {
        Result = AddControl<Tree>();
    }
    else if (Type == VerticalContainer::TypeName())
    {
        Result = AddControl<VerticalContainer>();
    }
    else if (Type == WindowContainer::TypeName())
    {
        Result = AddControl<WindowContainer>();
    }

    return Result;
}

Container* Container::InsertControl(const std::shared_ptr<Control>& Item, int Position)
{
    if (HasControl(Item))
    {
        return this;
    }

    Item->SetParent(this);
    Item->SetOnInvalidate([this](std::shared_ptr<Control> Focus, InvalidateType Type)
        {
            HandleInvalidate(Focus, Type);
        });

    if (Position >= 0)
    {
        m_Controls.insert(m_Controls.begin() + Position, Item);
    }
    else
    {
        m_Controls.push_back(Item);
    }

    Invalidate(Item, InvalidateType::Paint);
    Invalidate(InvalidateType::Layout);
    OnInsertControl(Item);

    return this;
}

Container* Container::RemoveControl(const std::shared_ptr<Control>& Item)
{
    auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
    if (Iter != m_Controls.end())
    {
        m_Controls.erase(Iter);
        Invalidate(InvalidateType::Both);
        OnRemoveControl(Item);
    }

    return this;
}

bool Container::HasControl(const std::shared_ptr<Control>& Item) const
{
    return std::find(m_Controls.begin(), m_Controls.end(), Item) != m_Controls.end();
}

bool Container::HasControlRecurse(const std::shared_ptr<Control>& Item) const
{
    if (HasControl(Item))
    {
        return true;
    }

    for (const std::shared_ptr<Control>& Child : Controls())
    {
        const std::shared_ptr<Container>& ChildContainer = std::dynamic_pointer_cast<Container>(Child);
        if (ChildContainer && ChildContainer->HasControlRecurse(Item))
        {
            return true;
        }
    }

    return false;
}

void Container::ClearControls()
{
    m_Controls.clear();
    Invalidate(InvalidateType::Both);
}

size_t Container::NumControls() const
{
    return m_Controls.size();
}

const std::shared_ptr<Control>& Container::Get(size_t Index) const
{
    assert(Index < m_Controls.size());
    return m_Controls[Index];
}

Container& Container::SetClip(bool Clip)
{
    m_Clip = Clip;
    return *this;
}

bool Container::ShouldClip() const
{
    return m_Clip;
}

Container* Container::Layout()
{
    PROFILER_SAMPLE_GROUP((std::string(GetType()) + "::Layout").c_str());

    m_InLayout = true;

    {
        PROFILER_SAMPLE("PlaceControls");
        PlaceControls(m_Controls);
    }

    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        const std::shared_ptr<Container> Child = std::dynamic_pointer_cast<Container>(Item);
        if (Child)
        {
            Child->Layout();
        }
    }

    {
        PROFILER_SAMPLE("Update");
        for (const std::shared_ptr<Control>& Item : m_Controls)
        {
            Item->Update();
        }
    }

    OnLayoutComplete();

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

Vector2 Container::ChildrenSize() const
{
    Vector2 Result;

    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        Vector2 Size = Item->GetSize();

        const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
        if (ItemContainer)
        {
            Size = ItemContainer->DesiredSize();
            const Vector2 ChildrenSize = ItemContainer->ChildrenSize();
            Size.X = std::max<float>(Size.X, ChildrenSize.X);
            Size.Y = std::max<float>(Size.Y, ChildrenSize.Y);
        }

        Result.X = std::max<float>(Result.X, Size.X);
        Result.Y = std::max<float>(Result.Y, Size.Y);
    }

    return Result;
}

void Container::GetControlList(ControlList& List) const
{
    List.AddControls(Controls());

    for (const std::shared_ptr<Control>& Item : Controls())
    {
        const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
        if (ItemContainer)
        {
            ItemContainer->GetControlList(List);
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

void Container::SetWindow(Window* InWindow)
{
    Control::SetWindow(InWindow);

    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        Item->SetWindow(InWindow);
    }
}

void Container::OnPaint(Paint& Brush) const
{
    PROFILER_SAMPLE_GROUP((std::string(GetType()) + "::OnPaint").c_str());

    if (ShouldClip())
    {
        Brush.PushClip(GetAbsoluteBounds());
    }

    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        if (!Brush.IsClipped(Item->GetAbsoluteBounds()))
        {
            Item->OnPaint(Brush);
        }
    }

    if (ShouldClip())
    {
        Brush.PopClip();
    }
}

void Container::OnLoad(const Json& Root)
{
    Control::OnLoad(Root);

    const Json& Controls = Root["Controls"];

    for (unsigned int I = 0; I < Controls.Count(); I++)
    {
        const Json& Item = Controls[I];

        std::string Type = Item["Type"].String();
        std::shared_ptr<Control> NewControl = CreateControl(Type);
        if (NewControl)
        {
            NewControl->OnLoad(Item);
        }
    }

    SetClip(Root["Clip"].Boolean(ShouldClip()));
}

void Container::OnSave(Json& Root) const
{
    Control::OnSave(Root);

    if (NumControls() == 0)
    {
        return;
    }

    Json Controls { Json::Type::Array };
    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        Json ItemJson { Json::Type::Object };
        Item->OnSave(ItemJson);
        Controls.Push(std::move(ItemJson));
    }

    Root["Controls"] = Controls;
}

void Container::OnThemeLoaded()
{
    for (const std::shared_ptr<Control>& Item : m_Controls)
    {
        Item->OnThemeLoaded();
    }
}

bool Container::IsInLayout() const
{
    if (m_InLayout)
    {
        return true;
    }

    Container const* Parent = dynamic_cast<Container const*>(GetParent());
    if (Parent != nullptr)
    {
        return Parent->IsInLayout();
    }

    return false;
}

void Container::HandleInvalidate(std::shared_ptr<Control> Focus, InvalidateType Type)
{
    if (IsInLayout() && Type != InvalidateType::Paint)
    {
        return;
    }

    Invalidate(Focus, Type);
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

void Container::OnInsertControl(const std::shared_ptr<Control>&)
{
}

void Container::OnRemoveControl(const std::shared_ptr<Control>&)
{
}

void Container::OnLayoutComplete()
{
}

}
