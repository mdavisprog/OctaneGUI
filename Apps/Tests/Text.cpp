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

namespace Tests
{

TEST_SUITE(Text,

TEST_CASE(SingleLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "Text", "ID": "Text", "Text": "Well Hello Friends!"})", List);

    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Text");
    const std::shared_ptr<OctaneGUI::Font> Font = Application.GetTheme()->GetFont();

    return Text->GetSize().Y == Font->Size();
})

TEST_CASE(MultiLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "Text", "ID": "Text", "Text": "Well\n Hello\n Friends!"})", List);

    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Text");
    const std::shared_ptr<OctaneGUI::Font> Font = Application.GetTheme()->GetFont();

    return Text->GetSize().Y == Font->Size() * 3.0f;
})

TEST_CASE(Wrap,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "Container", "Size": [80, 200], "Controls": [{"Type": "Text", "ID": "Text", "Wrap": true, "Text": "Well Hello Friends"}]})", List);

    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Text");
    const std::shared_ptr<OctaneGUI::Font> Font = Application.GetTheme()->GetFont();

    return Text->GetSize().Y > Font->Size();
})

)

}
