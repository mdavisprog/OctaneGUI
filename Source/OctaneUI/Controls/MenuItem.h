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

#include "TextSelectable.h"

namespace OctaneUI
{

class Menu;

class MenuItem : public TextSelectable
{
public:
	MenuItem(Window* InWindow);
	virtual ~MenuItem();

	std::shared_ptr<Menu> CreateMenu();
	std::shared_ptr<Menu> GetMenu() const;
	MenuItem* DestroyMenu();
	MenuItem* SetIsMenuBar(bool IsMenuBar);

	MenuItem* SetChecked(bool Checked);
	bool IsChecked() const;

	MenuItem* SetOnHover(OnMenuItemSignature Fn);
	MenuItem* SetOnSelected(OnMenuItemSignature Fn);

	virtual const char* GetType() const override;
	virtual void OnPaint(Paint& Brush) const override;
	virtual void Update() override;
	virtual void OnMouseEnter() override;

private:
	void OnPressed(TextSelectable* Item);

	std::shared_ptr<Menu> m_Menu;
	bool m_IsMenuBar;
	bool m_IsChecked;
	OnMenuItemSignature m_OnHover;
	OnMenuItemSignature m_OnSelected;
};

}
