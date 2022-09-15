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

#include "Tree.h"
#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../String.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "HorizontalContainer.h"
#include "Image.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

#define TOGGLE_SIZE 16.0f

class TreeItem : public HorizontalContainer
{
    CLASS(TreeItem)

public:
    typedef std::function<void(bool)> OnHoveredSignature;

    TreeItem(Window* InWindow)
        : HorizontalContainer(InWindow)
    {
        m_Toggle = AddControl<Image>();
        m_Text = AddControl<Text>();

        m_Toggle
            ->SetTexture(InWindow->GetIcons()->GetTexture())
            .SetUVs({});

        ClearHoveredColors();
        UpdateToggleSize();
    }

    Tree* Owner() const
    {
        return static_cast<Tree*>(GetParent());
    }

    TreeItem& SetText(const char* Text)
    {
        SetText(String::ToUTF32(Text).c_str());
        return *this;
    }

    TreeItem& SetText(const char32_t* Text)
    {
        m_Text->SetText(Text);
        Invalidate();
        return *this;
    }

    const char32_t* GetText() const
    {
        return m_Text->GetText();
    }

    Rect TextBounds() const
    {
        return m_Text->GetAbsoluteBounds();
    }

    Vector2 TextOffset() const
    {
        return m_Text->GetPosition();
    }

    TreeItem& SetToggle(bool Expand)
    {
        const Rect UVs = GetUVs(Expand);
        m_Toggle->SetUVs(UVs);
        UpdateToggleSize();
        return *this;
    }

    TreeItem& SetHoveredColors()
    {
        const Variant& Color = GetProperty(ThemeProperties::TextSelectable_Text_Hovered);
        m_Text->SetProperty(ThemeProperties::Text, Color);
        if (GetProperty(ThemeProperties::Tree_Highlight_Row).Bool())
        {
            m_Toggle->SetTint(Color.ToColor());
        }
        return *this;
    }

    TreeItem& ClearHoveredColors()
    {
        m_Text->ClearProperty(ThemeProperties::Text);
        m_Toggle->SetTint(GetProperty(ThemeProperties::Tree_Toggle_Icon).ToColor());
        return *this;
    }

    TreeItem& Select()
    {
        if (m_OnSelected)
        {
            m_OnSelected();
        }

        return *this;
    }

    TreeItem& SetOnToggle(OnEmptySignature&& Fn)
    {
        m_OnToggle = std::move(Fn);
        return *this;
    }

    TreeItem& SetOnHovered(OnHoveredSignature&& Fn)
    {
        m_OnHovered = std::move(Fn);
        return *this;
    }

    TreeItem& SetOnSelected(OnEmptySignature&& Fn)
    {
        m_OnSelected = std::move(Fn);
        return *this;
    }

    virtual void OnSave(Json& Root) const override
    {
        Control::OnSave(Root);

        Json Toggle(Json::Type::Object);
        m_Toggle->OnSave(Toggle);
        Root["Toggle"] = std::move(Toggle);

        Json TextRoot(Json::Type::Object);
        m_Text->OnSave(TextRoot);
        Root["Text"] = std::move(TextRoot);
    }

    virtual void OnPaint(Paint& Brush) const override
    {
        HorizontalContainer::OnPaint(Brush);

        if (Owner()->HasChildren() && GetProperty(ThemeProperties::Tree_Classic_Icons).Bool())
        {
            const Vector2 Scale { m_Toggle->GetSize() * 0.20f };
            Brush.RectangleOutline(m_Toggle->GetAbsoluteBounds().Shrink(Scale), m_Toggle->Tint());
        }
    }

    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override
    {
        if (m_Toggle->Contains(Position))
        {
            if (m_OnToggle)
            {
                m_OnToggle();
            }
        }
        else
        {
            Select();
        }

        return true;
    }

    virtual void OnMouseEnter() override
    {
        if (m_OnHovered)
        {
            m_OnHovered(true);
        }
    }

    virtual void OnMouseLeave() override
    {
        if (m_OnHovered)
        {
            m_OnHovered(false);
        }
    }

    virtual void OnThemeLoaded() override
    {
        HorizontalContainer::OnThemeLoaded();
        ClearHoveredColors();
        UpdateToggleSize();
    }

private:
    TreeItem& UpdateToggleSize()
    {
        const Vector2 Size = m_Text->GetSize();
        m_Toggle->SetSize({ Size.Y, Size.Y });
        return *this;
    }

    Rect GetUVs(bool Expand) const
    {
        Rect Result;

        std::shared_ptr<Icons> Icons = GetWindow()->GetIcons();
        if (GetProperty(ThemeProperties::Tree_Classic_Icons).Bool())
        {
            Result = Expand ? Icons->GetUVs(Icons::Type::Minus) : Icons->GetUVs(Icons::Type::Plus);
        }
        else
        {
            Result = Expand ? Icons->GetUVs(Icons::Type::Expand) : Icons->GetUVs(Icons::Type::Collapse);
        }

        return Result;
    }

    std::shared_ptr<Image> m_Toggle { nullptr };
    std::shared_ptr<Text> m_Text { nullptr };

    OnEmptySignature m_OnToggle { nullptr };
    OnHoveredSignature m_OnHovered { nullptr };
    OnEmptySignature m_OnSelected { nullptr };
};

Tree::Tree(Window* InWindow)
    : Container(InWindow)
{
    m_Item = AddControl<TreeItem>();
    m_Item
        ->SetOnToggle([this]() -> void
            {
                SetExpanded(!m_Expand);
            })
        .SetOnHovered([this](bool Hovered) -> void
            {
                if (m_OnHoveredItem)
                {
                    m_OnHoveredItem(Hovered, m_Item);
                }
                else
                {
                    SetHovered(Hovered, m_Item);
                }
            })
        .SetOnSelected([this]() -> void
            {
                if (m_OnItemSelected)
                {
                    m_OnItemSelected(m_Item);
                }
                else
                {
                    SetSelected(m_Item);
                }
            });
}

std::shared_ptr<Tree> Tree::AddChild(const char* Text)
{
    return AddChild(String::ToUTF32(Text).c_str());
}

std::shared_ptr<Tree> Tree::AddChild(const char32_t* Text)
{
    if (!m_List)
    {
        m_List = AddControl<VerticalContainer>();
        m_List->SetSpacing({ 0.0f, 0.0f });

        m_Item->SetToggle(m_Expand);

        if (!m_Expand)
        {
            RemoveControl(m_List);
        }
    }

    std::shared_ptr<Tree> Result = m_List->AddControl<Tree>();
    Result
        ->SetText(Text)
        .SetParentTree(TShare<Tree>())
        .SetOnHovered([this](bool Hovered, const std::shared_ptr<TreeItem>& Item) -> void
            {
                if (m_OnHoveredItem)
                {
                    m_OnHoveredItem(Hovered, Item);
                }
                else
                {
                    SetHovered(Hovered, Item);
                }
            })
        .SetOnSelected([this](const std::shared_ptr<TreeItem>& Item) -> void
            {
                if (m_OnItemSelected)
                {
                    m_OnItemSelected(Item);
                }
                else
                {
                    SetSelected(Item);
                }
            })
        .SetOnToggled([this](Tree& Item) -> void
            {
                if (m_OnToggled)
                {
                    m_OnToggled(Item);
                }
            })
        .SetOnInvalidate([this](std::shared_ptr<Control> Focus, InvalidateType Type) -> void
            {
                HandleInvalidate(Focus, Type);

                if (IsInLayout() && Type != InvalidateType::Paint)
                {
                    return;
                }

                InvalidateLayout();
            });

    InvalidateLayout();
    return Result;
}

Tree& Tree::SetText(const char* Text)
{
    m_Item->SetText(Text);
    return *this;
}

Tree& Tree::SetText(const char32_t* Text)
{
    m_Item->SetText(Text);
    return *this;
}

const char32_t* Tree::GetText() const
{
    return m_Item->GetText();
}

Tree& Tree::SetExpanded(bool Expand)
{
    if (m_Expand == Expand)
    {
        return *this;
    }

    if (!m_List)
    {
        return *this;
    }

    m_Expand = Expand;
    m_Item->SetToggle(m_Expand);

    if (m_List)
    {
        if (m_Expand)
        {
            if (!HasControl(m_List))
            {
                InsertControl(m_List);
                m_List
                    ->Layout()
                    ->SetPosition({ m_Item->TextOffset().X, m_Item->GetSize().Y });
            }
        }
        else
        {
            RemoveControl(m_List);
        }
    }

    if (m_OnToggled)
    {
        m_OnToggled(*this);
    }

    return *this;
}

Tree& Tree::SetExpandedAll(bool Expand)
{
    SetExpanded(Expand);

    for (const std::shared_ptr<Tree>& Child : Children())
    {
        Child->SetExpandedAll(Expand);
    }

    return *this;
}

bool Tree::IsExpanded() const
{
    return m_Expand;
}

Tree& Tree::SetSelected(bool Selected)
{
    m_Item->Select();
    return *this;
}

Tree& Tree::SetRowSelect(bool RowSelect)
{
    m_RowSelect = RowSelect;
    return *this;
}

bool Tree::ShouldRowSelect() const
{
    return m_RowSelect;
}

Tree& Tree::SetMetaData(void* MetaData)
{
    m_MetaData = MetaData;
    return *this;
}

void* Tree::MetaData() const
{
    return m_MetaData;
}

Tree& Tree::SetOnSelected(OnTreeSignature&& Fn)
{
    m_OnSelected = std::move(Fn);
    return *this;
}

Tree& Tree::SetOnHovered(OnTreeSignature&& Fn)
{
    m_OnHovered = std::move(Fn);
    return *this;
}

Tree& Tree::SetOnToggled(OnTreeSignature&& Fn)
{
    m_OnToggled = std::move(Fn);
    return *this;
}

Tree& Tree::ClearChildren()
{
    RemoveControl(m_List);
    m_List = nullptr;
    Invalidate();
    return *this;
}

bool Tree::HasChildren() const
{
    return m_List && m_List->Controls().size() > 0;
}

std::vector<std::shared_ptr<Tree>> Tree::Children() const
{
    std::vector<std::shared_ptr<Tree>> Result;

    if (!m_List)
    {
        return Result;
    }

    for (const std::shared_ptr<Control>& Item : m_List->Controls())
    {
        Result.push_back(std::static_pointer_cast<Tree>(Item));
    }

    return std::move(Result);
}

const Tree& Tree::ForEachChild(OnTreeSignature Callback) const
{
    if (!Callback || !m_List || !m_Expand)
    {
        return *this;
    }

    for (const std::shared_ptr<Control>& Item : m_List->Controls())
    {
        const std::shared_ptr<Tree>& Child = std::static_pointer_cast<Tree>(Item);
        Callback(*Child.get());
    }

    return *this;
}

std::shared_ptr<Tree> Tree::GetChild(const char32_t* Text) const
{
    std::shared_ptr<Tree> Result { nullptr };

    if (!m_List)
    {
        return Result;
    }

    for (const std::shared_ptr<Control>& Item : m_List->Controls())
    {
        const std::shared_ptr<Tree>& Child = std::static_pointer_cast<Tree>(Item);

        if (std::u32string(Child->GetText()) == Text)
        {
            return Child;
        }
    }

    return Result;
}

const std::weak_ptr<Tree>& Tree::ParentTree() const
{
    return m_ParentTree;
}

std::weak_ptr<Control> Tree::GetControl(const Vector2& Point) const
{
    return GetControl(Point, GetAbsoluteBounds());
}

Vector2 Tree::DesiredSize() const
{
    Vector2 Result = m_Item->GetSize();

    if (m_List && m_Expand)
    {
        const Vector2 Size = m_List->DesiredSize();
        Result.X = std::max<float>(Result.X, m_List->GetPosition().X + Size.X);
        Result.Y += Size.Y;
    }

    return Result;
}

void Tree::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    SetText(Root["Text"].String());
    SetRowSelect(Root["RowSelect"].Boolean());

    const Json& Items = Root["Items"];
    Items.ForEach([this](const Json& Item) -> void
        {
            std::shared_ptr<Tree> Child = AddChild("");
            Child->OnLoad(Item);
        });
}

void Tree::OnSave(Json& Root) const
{
    Container::OnSave(Root);

    if (!m_Selected.expired())
    {
        Root["Selected"] = m_Selected.lock()->GetText();
    }

    Root["Expand"] = m_Expand;
    Root["RowSelect"] = m_RowSelect;
    Root["HasMetaData"] = m_MetaData != nullptr;
}

void Tree::OnPaint(Paint& Brush) const
{
    std::shared_ptr<TreeItem> Hovered = m_Hovered.lock();
    std::shared_ptr<TreeItem> Selected = m_Selected.lock();

    if (Hovered && Hovered != Selected)
    {
        PaintSelection(Brush, Hovered);
    }

    if (Selected && !IsHidden(Selected))
    {
        PaintSelection(Brush, Selected);
    }

    Container::OnPaint(Brush);
}

void Tree::OnThemeLoaded()
{
    Container::OnThemeLoaded();

    if (m_List)
    {
        m_Item->SetToggle(m_Expand);
    }

    if (!m_Selected.expired())
    {
        m_Selected.lock()->SetHoveredColors();
    }
}

Tree& Tree::SetOnHovered(OnHoveredTreeItemSignature&& Fn)
{
    m_OnHoveredItem = std::move(Fn);
    return *this;
}

Tree& Tree::SetOnSelected(OnTreeItemSignature&& Fn)
{
    m_OnItemSelected = std::move(Fn);
    return *this;
}

std::weak_ptr<Control> Tree::GetControl(const Vector2& Point, const Rect& RootBounds) const
{
    const Vector2 Min = { RootBounds.Min.X, m_Item->GetAbsolutePosition().Y };
    const Vector2 Max = { RootBounds.Max.X, m_Item->GetAbsolutePosition().Y + m_Item->GetSize().Y };
    const Rect Bounds = { Min, Max };

    if (Bounds.Contains(Point))
    {
        return m_Item;
    }

    std::weak_ptr<Control> Result;
    if (!m_List || !m_Expand)
    {
        return Result;
    }

    for (const std::shared_ptr<Control>& Child : m_List->Controls())
    {
        const std::shared_ptr<Tree>& ChildTree = std::static_pointer_cast<Tree>(Child);
        Result = ChildTree->GetControl(Point, RootBounds);

        if (!Result.expired())
        {
            break;
        }
    }

    return Result;
}

void Tree::SetHovered(bool Hovered, const std::shared_ptr<TreeItem>& Item)
{
    std::shared_ptr<TreeItem> Hover = m_Hovered.lock();
    std::shared_ptr<TreeItem> Select = m_Selected.lock();

    if (Hover == Item)
    {
        if (!Hovered)
        {
            if (Select != Item)
            {
                Item->ClearHoveredColors();
            }
            m_Hovered.reset();
            Invalidate();
        }

        return;
    }

    if (Hovered)
    {
        Item->SetHoveredColors();
        m_Hovered = Item;

        if (m_OnHovered)
        {
            m_OnHovered(*Item->Owner());
        }

        Invalidate();
    }
}

void Tree::SetSelected(const std::shared_ptr<TreeItem>& Item)
{
    // This function should be called on the root tree.
    // It should be safe to call m_OnSelected here for outstide controls.
    std::shared_ptr<TreeItem> Select = m_Selected.lock();

    if (Select == Item)
    {
        RowSelect(Item);
        return;
    }

    if (Select)
    {
        Select->ClearHoveredColors();
    }

    m_Selected = Item;

    if (Item)
    {
        Item->SetHoveredColors();
        RowSelect(Item);

        if (m_OnSelected)
        {
            Tree* Parent = static_cast<Tree*>(Item->GetParent());
            m_OnSelected(*Parent);
        }
    }

    Invalidate();
}

void Tree::PaintSelection(Paint& Brush, const std::shared_ptr<TreeItem>& Item) const
{
    if (GetProperty(ThemeProperties::Tree_Highlight_Row).Bool())
    {
        const Vector2 Position = Item->GetAbsolutePosition();
        const Vector2 Min = { GetAbsolutePosition().X, Position.Y };
        const Vector2 Max = { GetAbsoluteBounds().Max.X, Position.Y + Item->GetSize().Y };
        Brush.Rectangle({ Min, Max }, GetProperty(ThemeProperties::TextSelectable_Hovered).ToColor());
    }
    else
    {
        Brush.Rectangle(Item->TextBounds(), GetProperty(ThemeProperties::TextSelectable_Hovered).ToColor());
    }
}

bool Tree::IsHidden(const std::shared_ptr<TreeItem>& Item) const
{
    if (!m_List)
    {
        return false;
    }

    if (m_List->HasControlRecurse(Item))
    {
        return !m_Expand;
    }

    // TODO: Look into seeing if this can be optimized/removed since a recursive check
    // is already done above.
    for (const std::shared_ptr<Control>& Child : m_List->Controls())
    {
        const std::shared_ptr<Tree>& ChildTree = std::static_pointer_cast<Tree>(Child);

        if (ChildTree->IsHidden(Item))
        {
            return true;
        }
    }

    return false;
}

void Tree::RowSelect(const std::shared_ptr<TreeItem>& Item) const
{
    if (!m_RowSelect)
    {
        return;
    }

    Tree* Parent = static_cast<Tree*>(Item->GetParent());
    if (Parent != nullptr)
    {
        Parent->SetExpanded(!Parent->IsExpanded());
    }
}

Tree& Tree::SetParentTree(const std::weak_ptr<Tree> Parent)
{
    m_ParentTree = Parent;
    return *this;
}

}
