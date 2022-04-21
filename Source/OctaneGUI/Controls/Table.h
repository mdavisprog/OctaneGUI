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

class BoxContainer;
class Text;

class Table : public Container
{
	CLASS(Table)

public:
	Table(Window* InWindow);

	Table& SetColumns(int Columns);
	int Columns() const;
	const std::shared_ptr<Container>& Column(int Index) const;

	Table& SetHeader(int Index, const char* Header);

	virtual Vector2 DesiredSize() const override;

private:
	class TableColumn : public Container
	{
		CLASS(TableColumn)

	public:
		TableColumn(Window* InWindow);

		TableColumn& SetHeader(const char* Header);

		const std::shared_ptr<Container>& Body() const;

		virtual Vector2 DesiredSize() const override;
	
	private:
		std::shared_ptr<Container> m_Outer { nullptr };
		std::shared_ptr<Text> m_Header { nullptr };
		std::shared_ptr<Container> m_Body { nullptr };
	};

	std::shared_ptr<BoxContainer> m_Row { nullptr };
};

}
