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

    VERIFYF(Cell->GetSize().X == HeaderSize.X, "First cell width %.2f does not match header width %.2f!\n", Cell->GetSize().X, HeaderSize.X);

    OctaneGUI::Vector2 MousePos(HeaderSize.X + 2.0f, 2.0f);
    Utility::MousePress(Application, MousePos);

    MousePos.X += 5.0f;
    Utility::MouseMove(Application, MousePos);
    Utility::MouseRelease(Application, MousePos);
    Application.Update();

    return Cell->GetSize().X == std::floor(HeaderSize.X) + 5.0f;
})

)

}