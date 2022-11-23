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

#include "Table.h"
#include "../Assert.h"
#include "../Json.h"
#include "../String.h"
#include "HorizontalContainer.h"
#include "Splitter.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

//
// TableCell
//

class TableCell : public Container
{
    CLASS(TableCell)

public:
    TableCell(Window* InWindow)
        : Container(InWindow)
    {
        SetClip(true);
    }

    virtual Vector2 DesiredSize() const override
    {
        return { GetSize().X, ChildrenSize().Y };
    }
};

//
// TableRow
//

class TableRow : public HorizontalContainer
{
    CLASS(TableRow)

public:
    TableRow(Window* InWindow)
        : HorizontalContainer(InWindow)
    {
        SetSpacing({});
        SetExpand(Expand::Width);
    }

    std::shared_ptr<Container> AddCell()
    {
        if (!m_Cells.empty())
        {
            Cell& Back = m_Cells.back();
            Back.Separator = AddControl<Container>();
            Back.Separator->SetExpand(Expand::Height);
        }

        std::shared_ptr<TableCell> Result = AddControl<TableCell>();
        m_Cells.push_back({ Result, nullptr });
        return Result;
    }

    TableRow& SetCellSize(size_t Index, float Width, float SeparatorWidth)
    {
        const Cell& Item = GetCell(Index);

        const Vector2 ContentsSize = Item.Contents->ChildrenSize();
        Item.Contents->SetSize({ Width, ContentsSize.Y });

        if (Item.Separator)
        {
            Item.Separator->SetSize({ SeparatorWidth, 0.0f });
        }

        return *this;
    }

    size_t Cells() const
    {
        return m_Cells.size();
    }

    std::shared_ptr<Container> GetCellContainer(size_t Index) const
    {
        return GetCell(Index).Contents;
    }

private:
    struct Cell
    {
    public:
        std::shared_ptr<TableCell> Contents { nullptr };
        std::shared_ptr<Container> Separator { nullptr };
    };

    const Cell& GetCell(size_t Index) const
    {
        Assert(Index < m_Cells.size(), "Invalid column given %zu! Number of columns: %zu.\n", Index, m_Cells.size());
        return m_Cells[Index];
    }

    std::vector<Cell> m_Cells {};
};

//
// TableRows
//

class TableRows : public VerticalContainer
{
    CLASS(TableRows)

public:
    TableRows(Window* InWindow)
        : VerticalContainer(InWindow)
    {
        SetSpacing({});
        SetExpand(Expand::Width);
    }

    std::shared_ptr<TableRow> AddRow()
    {
        std::shared_ptr<TableRow> Result = AddControl<TableRow>();
        return Result;
    }

    std::shared_ptr<TableRow> Row(size_t Index) const
    {
        Assert(Index < NumControls(), "Invalid row given %zu! Number of rows: %zu.\n", Index, NumControls());
        return std::static_pointer_cast<TableRow>(Get(Index));
    }

    size_t Rows() const
    {
        return NumControls();
    }
};

//
// Table
//

Table::Table(Window* InWindow)
    : Container(InWindow)
{
    m_Contents = AddControl<VerticalContainer>();
    m_Contents
        ->SetSpacing({})
        ->SetExpand(Expand::Both);

    m_Header = m_Contents->AddControl<Splitter>();
    m_Header
        ->SetOrientation(Orientation::Vertical)
        .SetOnResized([this](Splitter&) -> void
            {
                SyncSize();
            })
        .SetExpand(Expand::Width);
    
    m_Rows = m_Contents->AddControl<TableRows>();
}

Table& Table::AddColumn(const char32_t* Label)
{
    const std::shared_ptr<Container>& Column = m_Header->AddContainer();
    const std::shared_ptr<Text> TextComponent = Column
        ->SetClip(true)
        .AddControl<Text>();
    TextComponent->SetText(Label);
    return *this;
}

size_t Table::Columns() const
{
    return m_Header->Count();
}

Table& Table::AddRow()
{
    std::shared_ptr<TableRow> Row = m_Rows->AddRow();

    for (size_t I = 0; I < m_Header->Count(); I++)
    {
        Row->AddCell();
    }

    return *this;
}

size_t Table::Rows() const
{
    return m_Rows->NumControls();
}

std::shared_ptr<Container> Table::Cell(size_t Row, size_t Column) const
{
    std::shared_ptr<TableRow> RowContainer = m_Rows->Row(Row);
    return RowContainer->GetCellContainer(Column);
}

Vector2 Table::DesiredSize() const
{
    return m_Contents->DesiredSize();
}

void Table::OnLoad(const Json& Root)
{
    Container::OnLoad(Root);

    const Json& Columns = Root["Header"];
    Columns.ForEach([this](const Json& Item) -> void
        {
            AddColumn(String::ToUTF32(Item["Label"].String()).c_str());
        });
    
    const Json& Rows = Root["Rows"];
    for (unsigned int RowIdx = 0; RowIdx < Rows.Count(); RowIdx++)
    {
        AddRow();

        const Json& Row = Rows[RowIdx];
        const Json& Columns = Row["Columns"];
        for (unsigned int ColumnIdx = 0; ColumnIdx < Columns.Count(); ColumnIdx++)
        {
            const Json& Column = Columns[ColumnIdx];
            std::shared_ptr<Container> Cell = this->Cell(RowIdx, ColumnIdx);
            Cell->OnLoad(Column);
        }
    }

    SyncSize();
}

void Table::SyncSize()
{
    const Vector2 SplitterSize { m_Header->SplitterSize() };

    for (size_t RowIdx = 0; RowIdx < m_Rows->Rows(); RowIdx++)
    {
        std::shared_ptr<TableRow> Row = m_Rows->Row(RowIdx);
        for (size_t ColumnIdx = 0; ColumnIdx < Row->Cells(); ColumnIdx++)
        {
            const std::shared_ptr<Container>& Header = m_Header->GetSplit(ColumnIdx);
            Row->SetCellSize(ColumnIdx, Header->GetSize().X, SplitterSize.X);
        }
    }
}

}
