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

class Splitter;
class TableRows;
class VerticalContainer;

class Table : public Container
{
    CLASS(Table)

public:
    Table(Window* InWindow);

    Table& AddColumn(const char32_t* Label);
    size_t Columns() const;

    Table& AddRow();
    size_t Rows() const;

    std::shared_ptr<Container> Cell(size_t Row, size_t Column) const;

    virtual Vector2 DesiredSize() const override;

    virtual void OnLoad(const Json& Root) override;

private:
    void SyncSize();

    std::shared_ptr<VerticalContainer> m_Contents { nullptr };
    std::shared_ptr<Splitter> m_Header { nullptr };
    std::shared_ptr<TableRows> m_Rows { nullptr };
};

}
