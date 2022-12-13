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

void Load(OctaneGUI::Application& Application, const char* Json, OctaneGUI::ControlList& ControlList)
{
    std::string Stream = R"({"Type": "VerticalContainer", "Controls": [)";
    Stream += Json;
    Stream += "]}";
    Utility::Load(Application, Stream.c_str(), ControlList);
}

TEST_SUITE(RadioButton,

TEST_CASE(OneGroup,
{
    const std::string Stream =
        R"({"Type": "RadioButton", "ID": "One", "Text": {"Text": "One"}},
        {"Type": "RadioButton", "ID": "Two", "Text": {"Text": "Two"}},
        {"Type": "RadioButton", "ID": "Three", "Text": {"Text": "Three"}})";

    OctaneGUI::ControlList List;
    Load(Application, Stream.c_str(), List);

    std::shared_ptr<OctaneGUI::RadioButton> One = List.To<OctaneGUI::RadioButton>("One");
    std::shared_ptr<OctaneGUI::RadioButton> Two = List.To<OctaneGUI::RadioButton>("Two");
    std::shared_ptr<OctaneGUI::RadioButton> Three = List.To<OctaneGUI::RadioButton>("Three");

    One->SetSelected(true);

    VERIFYF(One->IsSelected(), "'One' is not selected when it should be!");
    VERIFYF(!Two->IsSelected(), "'Two' is selected when it should not be!");
    VERIFYF(!Three->IsSelected(), "'Three' is selected when it should not be!");

    Two->SetSelected(true);

    VERIFYF(!One->IsSelected(), "'One' is selected when it should not be!");
    VERIFYF(Two->IsSelected(), "'Two' is not selected when it should be!");
    VERIFYF(!Three->IsSelected(), "'Three' is selected when it should not be!");

    Three->SetSelected(true);

    VERIFYF(!One->IsSelected(), "'One' is selected when it should not be!");
    VERIFYF(!Two->IsSelected(), "'Two' is selected when it should not be!");
    VERIFYF(Three->IsSelected(), "'Three' is not selected when it should be!");

    return true;
})

TEST_CASE(TwoGroups,
{
    const std::string Stream =
        R"({"Type": "HorizontalContainer", "Controls": [
        {"Type": "RadioButton", "ID": "One", "Text": {"Text": "One"}},
        {"Type": "RadioButton", "ID": "Two", "Text": {"Text": "Two"}}]},
        {"Type": "HorizontalContainer", "Controls": [
        {"Type": "RadioButton", "ID": "Three", "Text": {"Text": "Three"}},
        {"Type": "RadioButton", "ID": "Four", "Text": {"Text": "Four"}}]})";
    
    OctaneGUI::ControlList List;
    Load(Application, Stream.c_str(), List);

    std::shared_ptr<OctaneGUI::RadioButton> One = List.To<OctaneGUI::RadioButton>("One");
    std::shared_ptr<OctaneGUI::RadioButton> Two = List.To<OctaneGUI::RadioButton>("Two");
    std::shared_ptr<OctaneGUI::RadioButton> Three = List.To<OctaneGUI::RadioButton>("Three");
    std::shared_ptr<OctaneGUI::RadioButton> Four = List.To<OctaneGUI::RadioButton>("Four");

    One->SetSelected(true);
    Three->SetSelected(true);

    VERIFYF(One->IsSelected(), "One is not selected when it should be!");
    VERIFYF(!Two->IsSelected(), "Two is selected when it should not be!");
    VERIFYF(Three->IsSelected(), "Three is not selected when it should be!");
    VERIFYF(!Four->IsSelected(), "Four is selected when it should not be!");

    Two->SetSelected(true);
    Four->SetSelected(true);

    VERIFYF(!One->IsSelected(), "One is selected when it should not be!");
    VERIFYF(Two->IsSelected(), "Two is not selected when it should be!");
    VERIFYF(!Three->IsSelected(), "Three is selected when it should not be!");
    VERIFYF(Four->IsSelected(), "Four is not selected when it should be!");

    return true;
})

TEST_CASE(MouseClick,
{
    const std::string Stream =
        R"({"Type": "RadioButton", "ID": "One", "Text": {"Text": "One"}},
        {"Type": "RadioButton", "ID": "Two", "Text": {"Text": "Two"}})";
    
    OctaneGUI::ControlList List;
    Load(Application, Stream.c_str(), List);
    
    std::shared_ptr<OctaneGUI::RadioButton> One = List.To<OctaneGUI::RadioButton>("One");
    std::shared_ptr<OctaneGUI::RadioButton> Two = List.To<OctaneGUI::RadioButton>("Two");

    Utility::MouseClick(Application, One->GetAbsolutePosition());

    VERIFYF(One->IsSelected(), "One is not selected when it should be!");
    VERIFYF(!Two->IsSelected(), "Two is selected when it should not be!");

    Utility::MouseClick(Application, Two->GetAbsolutePosition());

    VERIFYF(!One->IsSelected(), "One is selected when it should not be!");
    VERIFYF(Two->IsSelected(), "Two is not selected when it should be!");

    return true;
})

)

}
