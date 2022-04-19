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

#include "ScrollableViewControl.h"
#include "ScrollableContainer.h"

namespace OctaneGUI
{

ScrollableViewControl::ScrollableViewControl(Window* InWindow)
	: Container(InWindow)
{
	m_Scrollable = AddControl<ScrollableContainer>();
}

const std::shared_ptr<ScrollableContainer>& ScrollableViewControl::Scrollable() const
{
	return m_Scrollable;
}

std::weak_ptr<Control> ScrollableViewControl::GetControl(const Vector2& Point) const
{
	if (m_Scrollable->IsInScrollBar(Point))
	{
		return m_Scrollable->GetControl(Point);
	}

	return Container::GetControl(Point);
}

}
