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

namespace OctaneGUI
{

class ScrollBar;
class Timer;

class ScrollableContainer : public Container
{
	CLASS(ScrollableContainer)

public:
	ScrollableContainer(Window* InWindow);

	bool IsInScrollBar(const Vector2& Point) const;
	bool IsScrollBarVisible(const std::shared_ptr<Control>& Item) const;
	Vector2 ContentSize() const;

	ScrollableContainer& SetHorizontalSBEnabled(bool Enabled);
	ScrollableContainer& SetVerticalSBEnabled(bool Enabled);
	ScrollableContainer& SetOffset(const Vector2& Offset);
	ScrollableContainer& AddOffset(const Vector2& Delta);

	Vector2 GetScrollableSize() const;

	virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

	virtual void Update() override;
	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnLoad(const Json& Root) override;
	virtual void OnMouseMove(const Vector2& Position) override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnThemeLoaded() override;

private:
	Rect TranslatedBounds() const;
	Vector2 GetContentSize(const std::vector<std::shared_ptr<Control>>& Controls) const;
	Vector2 GetOverflow() const;
	void SetOffset(const Vector2& Offset, bool UpdateSBHandles);
	void UpdateScrollBars();

	std::shared_ptr<ScrollBar> m_HorizontalSB { nullptr };
	std::shared_ptr<ScrollBar> m_VerticalSB { nullptr };
	Vector2 m_ContentSize { Vector2() };

	Vector2 m_ScrollOffset { Vector2() };
	std::shared_ptr<Timer> m_ScrollTimer { nullptr };
	bool m_InitialScroll { true };
};

}
