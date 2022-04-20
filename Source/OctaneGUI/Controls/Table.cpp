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
#include "../Font.h"
#include "../Theme.h"
#include "HorizontalContainer.h"
#include "ScrollableViewControl.h"
#include "Text.h"
#include "VerticalContainer.h"

#include <cassert>

namespace OctaneGUI
{

//
// TableHeader
//

TableHeader::TableHeader(Window* InWindow)
	: Container(InWindow)
{
	SetExpand(Expand::Width);

	m_Row = AddControl<HorizontalContainer>();
	m_Row
		->SetSpacing({ 0.0f, 0.0f })
		->SetExpand(Expand::Width);
	
	m_Row->SetID("Row");
}

TableHeader& TableHeader::SetColumns(int Columns)
{
	if (m_Row->Controls().size() == Columns)
	{
		return *this;
	}

	// TODO: Look into only remove/adding necessary controls.
	m_Row->ClearControls();
	for (int I = 0; I < Columns; I++)
	{
		std::shared_ptr<BoxContainer> Column = m_Row->AddControl<HorizontalContainer>();
		Column
			->SetGrow(Grow::Center)
			->SetIgnoreDesiredSize(true)
			.SetExpand(Expand::Width);
		Column->AddControl<Text>();
	}

	SetSize({ 0.0f, GetTheme()->GetFont()->Size() });

	return *this;
}

int TableHeader::Columns() const
{
	return m_Row->Controls().size();
}

std::shared_ptr<Container> TableHeader::Column(int Index) const
{
	assert(Columns() > 0);
	assert(Index >= 0 && Index < Columns());

	std::shared_ptr<Container> Result = std::static_pointer_cast<Container>(m_Row->Controls()[Index]);
	return Result;
}

TableHeader& TableHeader::SetHeader(int Column, const char* Header)
{
	std::shared_ptr<Container> HeaderColumn = this->Column(Column);
	std::shared_ptr<Text> HeaderLabel = std::static_pointer_cast<Text>(HeaderColumn->Controls()[0]);
	HeaderLabel->SetText(Header);
	return *this;
}

TableHeader& TableHeader::SetHeaderWidth(int Column, float Width)
{
	std::shared_ptr<Container> HeaderColumn = this->Column(Column);
	HeaderColumn->SetSize({ Width, GetTheme()->GetFont()->Size() });
	HeaderColumn->SetExpand(Expand::None);
	return *this;
}

//
// Table
//

Table::Table(Window* InWindow)
	: Container(InWindow)
{
	std::shared_ptr<BoxContainer> Outer = AddControl<VerticalContainer>();
	Outer
		->SetSpacing({ 0.0f, 0.0f })
		->SetExpand(Expand::Both);

	m_Header = Outer->AddControl<TableHeader>();
	m_Body = Outer->AddControl<ScrollableViewControl>();
	m_Body->SetExpand(Expand::Both);

	m_Row = m_Body->AddControl<HorizontalContainer>();
	m_Row
		->SetSpacing({ 0.0f, 0.0f })
		->SetExpand(Expand::Both);
}

Table& Table::SetColumns(int Columns)
{
	if (m_Header->Columns() == Columns)
	{
		return *this;
	}

	m_Header->SetColumns(Columns);

	// TODO: Look into only remove/adding necessary controls.
	m_Row->ClearControls();
	for (int I = 0; I < Columns; I++)
	{
		std::shared_ptr<Container> Column = m_Row->AddControl<Container>();
		Column
			->SetClip(true)
			.SetExpand(Expand::Both);
	}

	return *this;
}

int Table::Columns() const
{
	return m_Header->Columns();
}

std::shared_ptr<Container> Table::Column(int Index) const
{
	assert(Columns() > 0);
	assert(Index >= 0 && Index < Columns());

	std::shared_ptr<Container> Result = std::static_pointer_cast<Container>(m_Row->Controls()[Index]);
	return Result;
}

Table& Table::SetHeader(int Column, const char* Text)
{
	m_Header->SetHeader(Column, Text);
	return *this;
}

Table& Table::SetColumnWidth(int Index, float Width)
{
	std::shared_ptr<Container> Column = this->Column(Index);
	Column
		->SetExpand(Expand::Height)
		->SetSize({ Width, Column->GetSize().Y });

	m_Header->SetHeaderWidth(Index, Width);	

	return *this;
}

}
