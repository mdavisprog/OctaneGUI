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

#include "../Color.h"
#include "Control.h"

namespace OctaneGUI
{

class Button : public Control
{
	CLASS(Button)

public:
	Button(Window* InWindow);

	Button& SetOnPressed(OnButtonSignature&& Fn);
	Button& SetOnClicked(OnButtonSignature&& Fn);
	Button& SetOnReleased(OnButtonSignature&& Fn);

	Button& SetDisabled(bool Disabled);
	bool IsDisabled() const;
	bool IsHovered() const;
	bool IsPressed() const;

	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnLoad(const Json& Root) override;
	virtual void OnSave(Json& Root) const override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;

protected:
	virtual void OnPressed();
	virtual void OnReleased();
	virtual void OnClicked();

private:
	enum class State : uint8_t
	{
		None,
		Hovered,
		Pressed
	};

	static const char* ToString(State Type);

	State m_State { State::None };
	OnButtonSignature m_OnPressed { nullptr };
	OnButtonSignature m_OnClicked { nullptr };
	OnButtonSignature m_OnReleased { nullptr };
	bool m_Disabled { false };
};

}
