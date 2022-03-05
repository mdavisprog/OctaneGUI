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

class Text;

class TextInput : public Control
{
	CLASS(TextInput)

public:
	TextInput(Window* InWindow);
	virtual ~TextInput();

	TextInput* SetText(const char* InText);
	const char* GetText() const;

	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnFocused() override;
	virtual void OnUnfocused() override;
	virtual void OnKeyPressed(Keyboard::Key Key) override;
	virtual void OnMouseMove(const Vector2& Position) override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnText(uint32_t Code) override;

private:
	void Delete(int32_t Range);
	void MovePosition(int32_t Count, bool UseAnchor = false);
	Vector2 GetPositionLocation() const;
	uint32_t GetPosition(const Vector2& Position) const;
	bool IsShiftPressed() const;
	int32_t GetRangeOr(int32_t Value) const;

	std::shared_ptr<Text> m_Text { nullptr };
	uint32_t m_Position { 0 };
	uint32_t m_Anchor { (uint32_t)-1 };
	bool m_Focused { false };
	bool m_Drag { false };
};

}
