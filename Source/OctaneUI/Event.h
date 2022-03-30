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

#include "Keyboard.h"
#include "Mouse.h"
#include "Vector2.h"

#include <cstdint>

namespace OctaneUI
{

class Event
{
public:
	enum class Type : uint32_t
	{
		None,
		KeyPressed,
		KeyReleased,
		MouseMoved,
		MousePressed,
		MouseReleased,
		Text,
		WindowClosed,
		WindowResized,
	};

	struct Key
	{
	public:
		Key(Keyboard::Key InCode)
			: m_Code(InCode)
		{
		}

		Keyboard::Key m_Code;
	};

	struct MouseMove
	{
	public:
		MouseMove()
			: m_Position()
		{
		}

		MouseMove(float X, float Y)
			: m_Position(X, Y)
		{
		}

		Vector2 m_Position;
	};

	struct MouseButton
	{
	public:
		MouseButton()
			: m_Button(Mouse::Button::Left)
			, m_Position()
		{
		}

		MouseButton(Mouse::Button InButton, float X, float Y)
			: m_Button(InButton)
			, m_Position(X, Y)
		{
		}

		Mouse::Button m_Button;
		Vector2 m_Position;
	};

	struct Text
	{
	public:
		Text(uint32_t InCode)
			: Code(InCode)
		{
		}

		uint32_t Code;
	};

	struct WindowResized
	{
	public:
		WindowResized(float Width, float Height)
			: m_Size(Width, Height)
		{
		}

		Vector2 m_Size;
	};

	union Data
	{
		Data();

		Key m_Key;
		MouseMove m_MouseMove;
		MouseButton m_MouseButton;
		Text m_Text;
		WindowResized m_Resized;
	};

	Event(Type InType);
	Event(Type InType, const Key& InKey);
	Event(const MouseMove& InMouseMove);
	Event(Type InType, const MouseButton& InMouseButton);
	Event(const WindowResized& InResized);
	Event(const Text& InText);
	~Event();

	Type GetType() const;
	const Data& GetData() const;

private:
	Event();

	Type m_Type;
	Data m_Data;
};

}
