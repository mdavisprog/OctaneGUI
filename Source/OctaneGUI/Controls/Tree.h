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

class HorizontalContainer;
class ImageButton;
class Text;
class VerticalContainer;

class Tree : public Container
{
	CLASS(Tree)

public:
	Tree(Window* InWindow);

	std::shared_ptr<Tree> AddChild(const char* Label);

	Tree& SetLabel(const char* Label);
	const char* Label() const;

	virtual Vector2 DesiredSize() const override;

	virtual void OnLoad(const Json& Root) override;
	virtual void OnThemeLoaded() override;

private:
	void EnableToggle(bool Enable);
	void SetExpand(bool Expand);

	std::shared_ptr<HorizontalContainer> m_Row { nullptr };
	std::shared_ptr<ImageButton> m_Toggle { nullptr };
	std::shared_ptr<Text> m_Text { nullptr };
	std::shared_ptr<VerticalContainer> m_List { nullptr };

	bool m_Expand { false };
};

}
