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

namespace OctaneGUI
{

class Text;

class TextSelectable : public Control
{
	CLASS(TextSelectable)

public:
	TextSelectable(Window* InWindow);
	virtual ~TextSelectable();

	TextSelectable& SetSelected(bool Selected);
	bool IsSelected() const;

	TextSelectable& SetText(const char* Contents);
	const char* GetText() const;

	TextSelectable& SetAlignment(HorizontalAlignment Align);
	HorizontalAlignment GetAlignment() const;

	TextSelectable& SetOnHovered(OnTextSelectableSignature Fn);
	TextSelectable& SetOnPressed(OnTextSelectableSignature Fn);

	virtual void OnPaint(Paint& Brush) const override;
	virtual void Update() override;
	virtual void OnLoad(const Json& Root) override;
	virtual void OnSave(Json& Root) const override;
	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnThemeLoaded() override;

protected:
	std::shared_ptr<Text> GetTextControl() const;

private:
	void UpdateSize();

	bool m_Hovered { false };
	bool m_Selected { false };
	std::shared_ptr<Text> m_Text { nullptr };
	OnTextSelectableSignature m_OnHovered { nullptr };
	OnTextSelectableSignature m_OnPressed { nullptr };
	HorizontalAlignment m_Align { HorizontalAlignment::Left };
};

}
