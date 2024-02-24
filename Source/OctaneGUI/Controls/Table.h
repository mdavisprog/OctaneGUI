/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

class ScrollableViewControl;
class Splitter;
class TableRows;
class VerticalContainer;

class Table : public Container
{
    CLASS(Table)

public:
    typedef std::function<void(Table&, size_t)> OnSelectedSignature;

    Table(Window* InWindow);

    Table& AddColumn(const char32_t* Label);
    Table& FitColumn(size_t Column);
    Table& SetColumnSize(size_t Column, float Size);
    size_t Columns() const;

    Table& AddRow();
    Table& ClearRows();
    size_t Rows() const;

    Table& SetRowSelectable(bool Value);
    bool RowSelectable() const;

    std::shared_ptr<Container> Cell(size_t Row, size_t Column) const;

    Table& SetOnSelected(OnSelectedSignature&& Fn);

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;
    virtual Vector2 DesiredSize() const override;

    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnMouseMove(const Vector2& Position) override;
    virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count) override;
    virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override;
    virtual void OnMouseLeave() override;

private:
    void SyncSize();
    void SyncSize(size_t Row);
    void OnPaintSelection(Paint& Brush, size_t Index) const;
    void SetHovered(int32_t Value);

    bool m_RowSelectable { false };
    int32_t m_Hovered { -1 };
    int32_t m_Selected { -1 };
    std::shared_ptr<VerticalContainer> m_Contents { nullptr };
    std::shared_ptr<Splitter> m_Header { nullptr };
    std::shared_ptr<TableRows> m_Rows { nullptr };
    std::shared_ptr<Control> m_Interaction { nullptr };
    OnSelectedSignature m_OnSelected { nullptr };
};

}
