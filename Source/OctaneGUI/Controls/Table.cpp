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
#include "HorizontalContainer.h"
#include "Text.h"
#include "VerticalContainer.h"

#include <cassert>

namespace OctaneGUI
{

//
// Table
//

Table::Table(Window* InWindow)
	: Container(InWindow)
{
	m_Row = AddControl<HorizontalContainer>();
	m_Row->SetSpacing({ 0.0f, 0.0f });
}

Table& Table::SetColumns(int Columns)
{
	if (m_Row->Controls().size() == Columns)
	{
		return *this;
	}

	// TODO: Look into only remove/adding necessary controls.
	m_Row->ClearControls();
	for (int I = 0; I < Columns; I++)
	{
		m_Row->AddControl<TableColumn>()->SetOnInvalidate([this](Control* Focus, InvalidateType Type) -> void
			{
				if (Type != InvalidateType::Paint)
				{
					Invalidate(this, Type);
				}
				else
				{
					HandleInvalidate(Focus, Type);
				}
			});
	}

	return *this;
}

int Table::Columns() const
{
	return m_Row->Controls().size();
}

const std::shared_ptr<Container>& Table::Column(int Index) const
{
	assert(Columns() > 0);
	assert(Index >= 0 && Index < Columns());

	std::shared_ptr<TableColumn> Column = std::static_pointer_cast<TableColumn>(m_Row->Controls()[Index]);
	return Column->Body();
}

Table& Table::SetHeader(int Index, const char* Text)
{
	assert(Columns() > 0);
	assert(Index >= 0 && Index < Columns());

	std::shared_ptr<TableColumn> Column = std::static_pointer_cast<TableColumn>(m_Row->Controls()[Index]);
	Column->SetHeader(Text);
	return *this;
}

Vector2 Table::DesiredSize() const
{
	return m_Row->DesiredSize();
}

//
// TableColumn
//

Table::TableColumn::TableColumn(Window* InWindow)
	: Container(InWindow)
{
	std::shared_ptr<BoxContainer> m_Outer = AddControl<VerticalContainer>();
	m_Outer->SetSpacing({ 0.0f, 0.0f });

	std::shared_ptr<BoxContainer> HeaderOuter = m_Outer->AddControl<HorizontalContainer>();
	HeaderOuter
		->SetGrow(Grow::Center)
		->SetExpand(Expand::Width);
	m_Header = HeaderOuter->AddControl<Text>();

	m_Body = m_Outer->AddControl<VerticalContainer>();
}

Table::TableColumn& Table::TableColumn::SetHeader(const char* Header)
{
	m_Header->SetText(Header);
	return *this;
}

const std::shared_ptr<Container>& Table::TableColumn::Body() const
{
	return m_Body;
}

Vector2 Table::TableColumn::DesiredSize() const
{
	if (m_Body->Controls().size() == 0)
	{
		return m_Header->GetSize();
	}

	return m_Body->DesiredSize();
}

}
