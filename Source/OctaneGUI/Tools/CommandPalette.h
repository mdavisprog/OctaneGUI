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

#include "../Controls/Container.h"

namespace OctaneGUI
{

class TextInput;
class VerticalContainer;

namespace Tools
{

class CommandPalette : public Container
{
	CLASS(CommandPalette)

public:
	CommandPalette(Window* InWindow);

	void Show();

	std::shared_ptr<Control> Input() const;

	virtual bool OnKeyPressed(Keyboard::Key Key) override;

private:
	// TODO: Maybe create an args parser class that can be used in multiple places.
	static std::vector<std::string> Tokenize(const std::string& Value);

	bool Process(const std::string& Command, const std::vector<std::string>& Arguments);

	std::shared_ptr<VerticalContainer> m_Container { nullptr };
	std::shared_ptr<TextInput> m_Input { nullptr };
};

}

}
