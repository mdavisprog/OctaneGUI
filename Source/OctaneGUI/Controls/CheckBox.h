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

#include "Button.h"

namespace OctaneGUI
{

class Text;

class CheckBox : public Button
{
	CLASS(CheckBox)

public:
	enum class State : uint8_t
	{
		None,
		Intermediate,
		Checked
	};

	CheckBox(Window* InWindow);
	virtual ~CheckBox();

	CheckBox& SetText(const char* InText);
	const char32_t* GetText() const;

	CheckBox& SetState(State InState);
	State GetState() const;

	CheckBox& SetTriState(bool IsTriState);
	bool IsTriState() const;

	virtual void OnPaint(Paint& Brush) const override;
	virtual void Update() override;
	virtual void OnLoad(const Json& Root) override;
	virtual void OnSave(Json& Root) const override;
	virtual void OnThemeLoaded() override;

protected:
	virtual void OnClicked() override;

private:
	Vector2 BoxSize() const;
	void UpdateSize();

	std::shared_ptr<Text> m_Text { nullptr };
	State m_State { State::None };
	bool m_TriState { false };
};

}
