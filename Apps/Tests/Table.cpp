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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"
#include "Utility.h"

#include <cmath>

namespace Tests
{

void LoadTable(OctaneGUI::Application& Application, const char* JSON, OctaneGUI::ControlList& List)
{
    std::string Stream = R"({"Type": "Table", "ID": "Table", "Expand": "Both", )";
    Stream += JSON;
    Stream += R"(})";
    Utility::Load(Application, Stream.c_str(), List);
}

TEST_SUITE(Table,

TEST_CASE(Headers,
{
    OctaneGUI::ControlList List;
    LoadTable(Application, R"("Header": [{"Label": "One"}, {"Label": "Two"}])", List);

    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    return Table->Columns() == 2;
})

TEST_CASE(Rows,
{
    OctaneGUI::ControlList List;
    LoadTable(Application, R"("Header": [{"Label": "One"}, {"Label": "Two"}], "Rows": [{}, {}, {}])", List);

    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    return Table->Columns() == 2 && Table->Rows() == 3;
})

TEST_CASE(Resize,
{
    const char* Stream = 
R"("Header": [{"Label": "One"}, {"Label": "Two"}], "Rows": [
    {"Columns": [
        {"Controls": [{"Type": "Text", "ID": "Text", "Text": "Hello"}]}
    ]}
])";

    OctaneGUI::ControlList List;
    LoadTable(Application, Stream, List);

    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    const std::shared_ptr<OctaneGUI::Container> Cell = Table->Cell(0, 0);
    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Table.Text");

    const std::shared_ptr<OctaneGUI::Font> Font = Application.GetTheme()->GetFont();
    const OctaneGUI::Vector2 HeaderSize = Font->Measure(U"One");

    VERIFYF(Cell->GetSize().X == HeaderSize.X, "First cell width %.2f does not match header width %.2f!", Cell->GetSize().X, HeaderSize.X);

    OctaneGUI::Vector2 MousePos(HeaderSize.X + 2.0f, 2.0f);
    Utility::MousePress(Application, MousePos);

    MousePos.X += 5.0f;
    Utility::MouseMove(Application, MousePos);
    Utility::MouseRelease(Application, MousePos);
    Application.Update();

    return Cell->GetSize().X == std::floor(HeaderSize.X) + 5.0f;
})

TEST_CASE(RowSelectable,
{
    const char* Stream = 
R"("RowSelectable": true, "Header": [{"Label": "One"}, {"Label": "Two"}], "Rows": [
    {"Columns": [
        {"Controls": [{"Type": "Text","Text": "Hello"}]}
    ]},
    {"Columns": [
        {"Controls": [{"Type": "Text","Text": "World"}]}
    ]},
    {"Columns": [
        {"Controls": [{"Type": "Text","Text": "Foo"}]}
    ]}
])";

    OctaneGUI::ControlList List;
    LoadTable(Application, Stream, List);

    size_t Result = 0;
    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    Table->SetOnSelected([&](OctaneGUI::Table&, size_t Selected) -> void
        {
            Result = Selected;
        });
    const OctaneGUI::Vector2 Size = Table->Cell(0, 0)->GetSize();
    
    // Take into account height of header + Row 1 + Row 2.
    Utility::MousePress(Application, {2.0f, Size.Y * 3.0f});
    Application.Update();

    return Result == 1;
})

TEST_CASE(Scrolling,
{\
    const char* Stream = R"("RowSelectable": true, "Header": [{"Label": "One"}, {"Label": "The quick brown fox jumped over the lazy dog"}])";

    OctaneGUI::ControlList List;
    LoadTable(Application, Stream, List);

    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    const OctaneGUI::Vector2 WindowSize { Application.GetMainWindow()->GetSize() };

    // Add enough rows to extend the rows height to overflow the table's height and display the vertical scroll bar.
    for (size_t I = 0; I < 50; I++)
    {
        Table->AddRow();
        const std::shared_ptr<OctaneGUI::Container> Cell = Table->Cell(I, 0);
        Cell->AddControl<OctaneGUI::Text>()->SetText(U"Hello");
    }
    Application.Update();

    // Scroll the rows vertically.
    std::shared_ptr<OctaneGUI::Container> Cell = Table->Cell(0, 1);
    OctaneGUI::Vector2 MousePos(WindowSize.X - 2.0f, Cell->GetAbsolutePosition().Y);
    Utility::MousePress(Application, MousePos);
    MousePos.Y += Cell->GetAbsolutePosition().Y;
    Utility::MouseMove(Application, MousePos);
    Utility::MouseRelease(Application, MousePos);
    Application.Update();
    VERIFYF(Cell->GetAbsolutePosition().Y == 0.0f, "First cell Y position (%.2f) is not 0.0 after scrolling!", Cell->GetAbsolutePosition().Y);

    // Expand the middle column to the size of the window. This will push the last column to the overflow, which should
    // display the horizontal scroll bar.
    // The header control should not have scrolled from the above, so the separator should be clickable at the top of the control.
    MousePos = OctaneGUI::Vector2(Cell->GetAbsolutePosition().X - 2.0f, 2.0f);
    Utility::MousePress(Application, MousePos);
    MousePos.X = WindowSize.X - 10.0f;
    Utility::MouseMove(Application, MousePos);
    Utility::MouseRelease(Application, MousePos);
    Application.Update();

    Cell = Table->Cell(0, 0);
    VERIFYF(Cell->GetAbsolutePosition().X == 0.0f, "First cell X position (%.2f) is not at 0!", Cell->GetAbsolutePosition().X);
    // Attempt to click on and move the horizontal scroll bar.
    MousePos = OctaneGUI::Vector2(2.0f, WindowSize.Y - 2.0f);
    Utility::MousePress(Application, MousePos);
    MousePos.X += 5.0f;
    Utility::MouseMove(Application, MousePos);
    Utility::MouseRelease(Application, MousePos);
    Application.Update();
    VERIFYF(Cell->GetAbsolutePosition().X == -5.0f, "First cell X position (%.2f) is not -5!", Cell->GetAbsolutePosition().X);

    return true;
})

TEST_CASE(CellInput,
{
    const char* Stream = R"("Header": [{"Label": "One"}], "Rows": [
        {"Columns": [
            {"Controls": [{"Type": "TextButton", "ID": "Button", "Text": {"Text": "Button"}}]}
        ]}
    ])";

    OctaneGUI::ControlList List;
    LoadTable(Application, Stream, List);

    const std::shared_ptr<OctaneGUI::Table> Table = List.To<OctaneGUI::Table>("Table");
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Table.Button");

    bool Clicked = false;
    Button->SetOnClicked([&](OctaneGUI::Button&) -> void
        {
            Clicked = true;
        });
    
    Utility::MouseClick(Application, Button->GetAbsolutePosition());

    return Clicked;
})

)

}
