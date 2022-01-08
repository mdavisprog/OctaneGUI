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

#include "../Theme.h"
#include "VerticalContainer.h"

namespace OctaneUI
{

VerticalContainer::VerticalContainer(Window* InWindow)
	: Container(InWindow)
{
}

VerticalContainer::~VerticalContainer()
{
}

const char* VerticalContainer::GetType() const
{
	return "VerticalContainer";
}

Vector2 VerticalContainer::CalculateSize(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Vector2 Result;

	for (const std::shared_ptr<Control>& Item : Controls)
	{
		const Vector2 Size = Item->GetSize();
		Result.X = std::max<float>(Result.X, Size.X);
		Result.Y += Size.Y;
	}

	ExpandSize(Result);

	return Result;
}

void VerticalContainer::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	int ExpandedControls = 0;
	Vector2 Size = GetPotentialSize(ExpandedControls);
	Size.X = GetSize().X;
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Vector2 ItemSize = Item->GetSize();
		Expand Direction = Item->GetExpand();

		switch (Direction)
		{
		case Expand::Both: ItemSize.X = Size.X; ItemSize.Y = Size.Y / (float)ExpandedControls; break;
		case Expand::Width: ItemSize.X = Size.X; break;
		case Expand::Height: ItemSize.Y = Size.Y / (float)ExpandedControls; break;
		case Expand::None:
		default: break;
		}

		Item->SetSize(ItemSize);
	}

	Vector2 Position;
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Item->SetPosition(Position);
		Item->Update();
		Position.Y += Item->GetSize().Y;
	}
}

}
