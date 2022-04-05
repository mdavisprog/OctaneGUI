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

#include "MarginContainer.h"
#include "../Json.h"

namespace OctaneUI
{

MarginContainer::MarginContainer(Window* InWindow)
	: Container(InWindow)
{
	SetExpand(Expand::Both);
}

MarginContainer& MarginContainer::SetMargins(const Rect& Margins)
{
	m_Margins = Margins;
	return *this;
}

Vector2 MarginContainer::DesiredSize() const
{
	Vector2 Result;

	for (const std::shared_ptr<Control>& Item : Controls())
	{
		Vector2 Size = Item->GetSize();
		const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			Size = ItemContainer->DesiredSize();
		}

		Result.X = std::max<float>(Result.X, Size.X);
		Result.Y = std::max<float>(Result.Y, Size.Y);
	}

	return Result + m_Margins.Min + m_Margins.Max;
}

void MarginContainer::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);

	SetExpand(Expand::Both);

	m_Margins.Min.X = Root["Left"].Number();
	m_Margins.Min.Y = Root["Top"].Number();
	m_Margins.Max.X = Root["Right"].Number();
	m_Margins.Max.Y = Root["Bottom"].Number();
}

void MarginContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Vector2 Size = Item->GetSize();

		const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			Size = ItemContainer->DesiredSize();
		}

		switch (Item->GetExpand())
		{
		case Expand::Both: Size = GetSize() - m_Margins.Min - m_Margins.Max; break;
		case Expand::Width: Size.X = GetSize().X - m_Margins.Min.X - m_Margins.Max.X; break;
		case Expand::Height: Size.Y = GetSize().Y - m_Margins.Min.Y - m_Margins.Max.Y; break;
		case Expand::None:
		default: break;
		}

		Item->SetSize(Size);
		Item->SetPosition(m_Margins.Min);
	}
}

}
