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

class Separator;
class SplitterInteraction;

class Splitter : public Container
{
	friend SplitterInteraction;

	CLASS(Splitter)

public:
	Splitter(Window* InWindow);

	Splitter& SetOrientation(Orientation InOrientation);
	Orientation GetOrientation() const;

	std::shared_ptr<Container> First() const;
	std::shared_ptr<Container> Second() const;

	virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;

	virtual void Update() override;
	virtual void OnLoad(const Json& Root) override;

private:
	void UpdateLayout();
	void SetSplitterPosition(float Position);

	std::shared_ptr<Separator> m_Separator { nullptr };
	std::shared_ptr<Container> m_First { nullptr };
	std::shared_ptr<Container> m_Second { nullptr };
	std::shared_ptr<SplitterInteraction> m_Interaction { nullptr };
	bool m_UpdateLayout { false };
};

}
