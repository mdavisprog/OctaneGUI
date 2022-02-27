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

#pragma once

#include "Container.h"

namespace OctaneUI
{

class ScrollBar;

class ScrollableContainer : public Container
{
	CLASS(ScrollableContainer)

public:
	ScrollableContainer(Window* InWindow);

	bool IsScrollBarVisible(const std::shared_ptr<Control>& Item) const;
	Vector2 ContentSize() const;

	virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

	virtual void Update() override;
	virtual void OnPaint(Paint& Brush) const override;

protected:
	virtual void PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const override;

private:
	Rect TranslatedBounds() const;
	Vector2 GetContentSize(const std::vector<std::shared_ptr<Control>>& Controls) const;

	std::shared_ptr<ScrollBar> m_HorizontalSB;
	std::shared_ptr<ScrollBar> m_VerticalSB;
	Vector2 m_ContentSize { Vector2() };
};

}
