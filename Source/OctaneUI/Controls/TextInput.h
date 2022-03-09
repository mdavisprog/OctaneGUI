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
	class TextPosition
	{
	public:
		TextPosition();
		TextPosition(uint32_t Line, uint32_t Column, uint32_t Index);

		bool operator==(const TextPosition& Other) const;
		bool operator!=(const TextPosition& Other) const;
		bool operator<(const TextPosition& Other) const;

		void Invalidate();
		bool IsValid() const;
		bool IsValidIndex() const;

		void SetLine(uint32_t Line);
		uint32_t Line() const;

		void SetColumn(uint32_t Column);
		uint32_t Column() const;

		void SetIndex(uint32_t Index);
		uint32_t Index() const;
	
	private:
		uint32_t m_Line { (uint32_t)-1 };
		uint32_t m_Column { (uint32_t)-1 };
		uint32_t m_Index { (uint32_t)-1 };
	};

	TextInput(Window* InWindow);
	virtual ~TextInput();

	TextInput* SetText(const char* InText);
	const char* GetText() const;

	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnFocused() override;
	virtual void OnUnfocused() override;
	virtual void OnLoad(const Json& Root) override;
	virtual void OnKeyPressed(Keyboard::Key Key) override;
	virtual void OnMouseMove(const Vector2& Position) override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnText(uint32_t Code) override;

private:
	void Delete(int32_t Range);
	void MoveHome();
	void MoveEnd();
	void MovePosition(int32_t Line, int32_t Column, bool UseAnchor = false);
	Vector2 GetPositionLocation(const TextPosition& Position) const;
	TextPosition GetPosition(const Vector2& Position) const;
	bool IsShiftPressed() const;
	int32_t GetRangeOr(int32_t Value) const;
	uint32_t LineStartIndex(uint32_t Index) const;
	uint32_t LineEndIndex(uint32_t Index) const;
	uint32_t LineSize(uint32_t Index) const;

	std::shared_ptr<Text> m_Text { nullptr };
	TextPosition m_Position {};
	TextPosition m_Anchor {};
	bool m_Focused { false };
	bool m_Drag { false };
	bool m_Multiline { false };
};

}
