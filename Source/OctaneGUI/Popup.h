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

#include "CallbackDefs.h"
#include "Mouse.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace OctaneGUI
{

class Container;
class Control;
class Paint;
struct Vector2;

class Popup
{
public:
	enum class State : uint8_t
	{
		None,
		Opening,
		Opened
	};

	Popup();
	~Popup();

	void Open(const std::shared_ptr<Container>& InContainer, bool Modal);
	void Close();
	void Update();
	bool IsModal() const;
	const std::shared_ptr<Container>& GetContainer() const;
	bool HasControl(const std::shared_ptr<Control>& Item) const;
	void OnMouseMove(const Vector2& Position);
	std::weak_ptr<Control> GetControl(const Vector2& Position) const;
	void OnPaint(Paint& Brush);
	void SetOnInvalidate(OnInvalidateSignature Fn);
	void SetOnClose(OnContainerSignature Fn);

private:
	std::shared_ptr<Container> m_Container;
	bool m_Modal;
	State m_State;
	OnInvalidateSignature m_OnInvalidate;
	OnContainerSignature m_OnClose;
};

}
