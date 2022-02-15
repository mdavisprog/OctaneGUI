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

#include "Control.h"

namespace OctaneUI
{

class ScrollBar : public Control
{
	CLASS(ScrollBar)

public:
	ScrollBar(Window* InWindow, Orientation InOrientation);

	ScrollBar* SetHandleSize(float HandleSize);
	ScrollBar* SetOnDrag(onScrollBarSignature Fn);
	float Offset() const;
	float OffsetPct() const;
	bool HasHandle() const;

	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnMouseMove(const Vector2& Position) override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseLeave() override;

private:
	Rect HandleBounds() const;

	Orientation m_Orientation { Orientation::Horizontal };
	float m_HandleSize { 0.0f };
	float m_Offset { 0.0f };
	bool m_Hovered { false };
	bool m_Drag { false };
	Vector2 m_DragAnchor { Vector2() };
	onScrollBarSignature m_OnDrag { nullptr };
};

}
