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

#include "../Paint.h"
#include "ScrollableContainer.h"
#include "ScrollBar.h"

namespace OctaneUI
{

ScrollableContainer::ScrollableContainer(Window* InWindow)
	: Container(InWindow)
{
	SetExpand(Expand::Both);

	m_HorizontalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Horizontal);
	m_HorizontalSB->SetOnDrag([this](ScrollBar*) -> void {
		SetPosition({-m_HorizontalSB->Offset(), GetPosition().Y});
		InvalidateLayout();
	});
	InsertControl(m_HorizontalSB);

	m_VerticalSB = std::make_shared<ScrollBar>(InWindow, Orientation::Vertical);
	m_VerticalSB->SetOnDrag([this](ScrollBar*) -> void {
		SetPosition({GetPosition().X, -m_VerticalSB->Offset()});
		InvalidateLayout();
	});
	InsertControl(m_VerticalSB);
}

void ScrollableContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Container::PlaceControls(Controls);

	Vector2 MaxSize;
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		if (Item != m_HorizontalSB && Item != m_VerticalSB)
		{
			const Vector2 Size = Item->GetSize();
			MaxSize.X = std::max<float>(MaxSize.X, Size.X);
			MaxSize.Y = std::max<float>(MaxSize.Y, Size.Y);
		}
	}

	const Vector2 Size = GetSize();
	const Vector2 Offset = MaxSize - Size;
	m_HorizontalSB->SetPosition(m_HorizontalSB->Offset(), Size.Y - 20.0f);
	m_HorizontalSB->SetSize(Size.X, 20.0f);
	m_HorizontalSB->SetSpace(Offset.X);

	m_VerticalSB->SetPosition(Size.X - 20.0f, m_VerticalSB->Offset());
	m_VerticalSB->SetSize(20.0f, Size.Y);
	m_VerticalSB->SetSpace(Offset.Y);
}

}
