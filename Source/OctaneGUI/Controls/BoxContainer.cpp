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

#include "BoxContainer.h"
#include "../Json.h"
#include "../String.h"

namespace OctaneGUI
{

static Grow ToGrow(const std::string& Value, Grow Default = Grow::Begin)
{
    const std::string Lower = String::ToLower(Value);
    if (Lower == "center")
    {
        return Grow::Center;
    }
    else if (Lower == "end")
    {
        return Grow::End;
    }

    return Default;
}

const char* ToString(Grow Type)
{
    switch (Type)
    {
    case Grow::Center: return "Center";
    case Grow::End: return "End";
    case Grow::Begin:
    default: break;
    }

    return "Begin";
}

BoxContainer::BoxContainer(Orientation Orient, Window* InWindow)
    : Container(InWindow)
    , m_Orient(Orient)
{
}

BoxContainer* BoxContainer::SetGrow(Grow Direction)
{
    m_Grow = Direction;
    Invalidate(InvalidateType::Layout);
    return this;
}

Grow BoxContainer::GrowDirection() const
{
    return m_Grow;
}

BoxContainer* BoxContainer::SetSpacing(const Vector2& Spacing)
{
    m_Spacing = Spacing;
    return this;
}

Vector2 BoxContainer::Spacing() const
{
    return m_Spacing;
}

BoxContainer& BoxContainer::SetIgnoreDesiredSize(bool IgnoreDesiredSize)
{
    m_IgnoreDesiredSize = IgnoreDesiredSize;
    return *this;
}

bool BoxContainer::ShouldIgnoreDesiredSize() const
{
    return m_IgnoreDesiredSize;
}

Vector2 BoxContainer::DesiredSize() const
{
    if (ShouldIgnoreDesiredSize())
    {
        return Container::DesiredSize();
    }

    Vector2 Result;

    for (const std::shared_ptr<Control>& Item : Controls())
    {
        Vector2 Size = Item->GetSize();
        const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
        if (ItemContainer)
        {
            Size = ItemContainer->DesiredSize();
        }

        if (m_Orient == Orientation::Horizontal)
        {
            Result.X += Size.X + (Item != Controls().back() ? m_Spacing.X : 0.0f);
            Result.Y = std::max<float>(Result.Y, Size.Y);
        }
        else
        {
            Result.X = std::max<float>(Result.X, Size.X);
            Result.Y += Size.Y + (Item != Controls().back() ? m_Spacing.Y : 0.0f);
        }
    }

    return Result;
}

void BoxContainer::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    m_Grow = ToGrow(Root["Grow"].String(), m_Grow);
    m_Spacing = Variant(Root["Spacing"]).Vector(m_Spacing);
}

void BoxContainer::OnSave(Json& Root) const
{
    Container::OnSave(Root);

    Root["Grow"] = ToString(m_Grow);
    Root["Spacing"] = Vector2::ToJson(m_Spacing);
}

void BoxContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
    const bool IsHorizontal = m_Orient == Orientation::Horizontal;

    // 1. The first step is to determine the total available size by checking for each control's expansion type
    //	  and shrinking the size if there are any controls that do not have an expansion type. Once this is
    //	  determined, then a partition size is calculated to be given to each expanded control.
    //	  The total amount of spacing that will be occupied will need to be subtracted from the available size as well.
    int ExpandW = 0;
    int ExpandH = 0;
    const Vector2 TotalSpacing = m_Spacing * (float)(Controls.size() > 0 ? Controls.size() - 1 : 0);
    Vector2 AvailableSize = GetSize() - TotalSpacing;
    std::unordered_map<Control*, Vector2> DesiredSizes;
    for (const std::shared_ptr<Control>& Item : Controls)
    {
        Vector2 Size = Item->GetSize();

        const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
        if (ItemContainer)
        {
            Size = ItemContainer->DesiredSize();
        }

        DesiredSizes[Item.get()] = Size;

        switch (Item->GetExpand())
        {
        case Expand::Both:
            ExpandW++;
            ExpandH++;
            break;
        case Expand::Width:
        {
            if (IsHorizontal)
            {
                ExpandW++;
            }
            else
            {
                AvailableSize.Y -= Size.Y;
            }
        }
        break;
        case Expand::Height:
        {
            if (IsHorizontal)
            {
                AvailableSize.X -= Size.X;
            }
            else
            {
                ExpandH++;
            }
        }
        break;
        case Expand::None:
        default:
            if (IsHorizontal)
            {
                AvailableSize.X -= Size.X;
            }
            else
            {
                AvailableSize.Y -= Size.Y;
            }
            break;
        }
    }

    ExpandW = std::max<int>(ExpandW, 1);
    ExpandH = std::max<int>(ExpandH, 1);
    Vector2 PartitionSize = IsHorizontal
        ? Vector2(AvailableSize.X / (float)ExpandW, AvailableSize.Y)
        : Vector2(AvailableSize.X, AvailableSize.Y / (float)ExpandH);

    // 2. Resize each control by setting each control's size by the partition size if it has an expansion
    //	  Type. The available size is then reduced by this partition size for the next control to use.
    //	  TODO: Mayby check if AvailableSize is zero and size any left over controls to zero?
    Vector2 TotalSize = TotalSpacing;
    AvailableSize = GetSize();
    for (const std::shared_ptr<Control>& Item : Controls)
    {
        Vector2 Size = DesiredSizes[Item.get()];

        switch (Item->GetExpand())
        {
        case Expand::Both:
        {
            if (IsHorizontal)
            {
                Size.X = PartitionSize.X;
                Size.Y = GetSize().Y;
            }
            else
            {
                Size.X = GetSize().X;
                Size.Y = PartitionSize.Y;
            }
        }
        break;
        case Expand::Width:
        {
            if (IsHorizontal)
            {
                Size.X = PartitionSize.X;
            }
            else
            {
                Size.X = GetSize().X;
            }
        }
        break;
        case Expand::Height:
        {
            if (IsHorizontal)
            {
                Size.Y = GetSize().Y;
            }
            else
            {
                Size.Y = PartitionSize.Y;
            }
        }
        break;
        case Expand::None:
        default: break;
        }

        Item->SetSize(Size);

        if (IsHorizontal)
        {
            TotalSize.X += Size.X;
            TotalSize.Y = std::max<float>(TotalSize.Y, Size.Y);
            AvailableSize.X -= Size.X;
        }
        else
        {
            TotalSize.X = std::max<float>(TotalSize.X, Size.X);
            TotalSize.Y += Size.Y;
            AvailableSize.Y -= Size.Y;
        }
    }

    // 3. Position each control based on this container's grow direction.
    Vector2 Offset;
    switch (m_Grow)
    {
    case Grow::Center:
    {
        if (IsHorizontal)
        {
            Offset.X = GetSize().X * 0.5f - TotalSize.X * 0.5f;
        }
        else
        {
            Offset.Y = GetSize().Y * 0.5f - TotalSize.Y * 0.5f;
        }
    }
    break;
    case Grow::End:
    {
        if (IsHorizontal)
        {
            Offset.X = GetSize().X - TotalSize.X;
        }
        else
        {
            Offset.Y = GetSize().Y - TotalSize.Y;
        }
    }
    break;
    case Grow::Begin:
    default: break;
    }

    for (const std::shared_ptr<Control>& Item : Controls)
    {
        Vector2 Size = Item->GetSize();

        Item->SetPosition(Offset);

        if (IsHorizontal)
        {
            Offset.X += Size.X + m_Spacing.X;
        }
        else
        {
            Offset.Y += Size.Y + m_Spacing.Y;
        }
    }
}

}
