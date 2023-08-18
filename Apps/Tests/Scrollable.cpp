/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

void Load(OctaneGUI::Application& Application, const char* Json, OctaneGUI::ControlList& List, int Width = 1280, int Height = 720)
{
    std::string Stream = R"({"ID": "View", "Type": "ScrollableViewControl", "Expand": "Both", "Controls": [)";
    Stream += Json;
    Stream += "]}";
    Utility::Load(Application, "", Stream.c_str(), List, Width, Height);
}

TEST_SUITE(Scrollable,

TEST_CASE(NoScrollBars,
{
    OctaneGUI::ControlList List;
    Load(Application, "{}", List);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    return !View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && !View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(HScrollBar,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "Size": [220, 40]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    return View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && !View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(VScrollBar,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "Size": [40, 240]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    return !View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(HAndVScrollBar,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "Size": [240, 240]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    return View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(HScrollBarVisibility,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "ID": "Container", "Size": [40, 200]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("View.Container");
    VERIFYF(!View->Scrollable()->HorizontalScrollBar()->ShouldPaint(), "Horizontal scroll bar is painted when it should not be!");
    Container->SetSize({ 240.0f, 200.0f });
    Application.Update();
    VERIFYF(View->Scrollable()->HorizontalScrollBar()->ShouldPaint(), "Horizontal scroll bar is not painted when it should be.");
    Container->SetSize({ 40.0f, 200.0f });
    Application.Update();
    return !View->Scrollable()->HorizontalScrollBar()->ShouldPaint();
})

TEST_CASE(VScrollBarVisibility,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "ID": "Container", "Size": [200, 40]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("View.Container");
    VERIFYF(!View->Scrollable()->VerticalScrollBar()->ShouldPaint(), "Vertical scroll bar is painted when it should not be!");
    Container->SetSize({ 200.0f, 240.0f });
    Application.Update();
    VERIFYF(View->Scrollable()->VerticalScrollBar()->ShouldPaint(), "Vertical scroll bar is not painted when it should be.");
    Container->SetSize({ 200.0f, 40.0f });
    Application.Update();
    return !View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(HScrollOffsetUpdate,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "ID": "Container", "Size": [240, 200]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("View.Container");
    VERIFYF(View->Scrollable()->HorizontalScrollBar()->ShouldPaint(), "Horizontal scroll bar is not painted when it should be.");
    VERIFYF(View->Scrollable()->Overflow().X == View->Scrollable()->VerticalScrollBar()->GetSize().X + 40.0f,
        "Horizontal overflow is %.2f when it should be %.2f.",
        View->Scrollable()->Overflow().X, View->Scrollable()->VerticalScrollBar()->GetSize().X + 40.0f);
    View->Scrollable()->SetOffset({ View->Scrollable()->Overflow().X, 0.0f });
    Application.Update();
    VERIFYF(View->Scrollable()->Offset().X == View->Scrollable()->Overflow().X,
        "Scrollable horizontal offset %.2f does not match overflow %.2f",
        View->Scrollable()->Offset().X, View->Scrollable()->Overflow().X);
    Container->SetSize({ 220.0f, 200.0f });
    Application.Update();
    VERIFYF(View->Scrollable()->Overflow().X == View->Scrollable()->VerticalScrollBar()->GetSize().X + 20.0f,
        "Second Horizontal overflow is %.2f when it should be %.2f",
        View->Scrollable()->Offset().X, View->Scrollable()->VerticalScrollBar()->GetSize().X + 20.0f);
    VERIFYF(View->Scrollable()->Offset().X == View->Scrollable()->Overflow().X,
        "Horizontal offset is %.2f when it should be %.2f",
        View->Scrollable()->Offset().X, View->Scrollable()->Overflow().X);
    return true;
})

TEST_CASE(VScrollOffsetUpdate,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "ID": "Container", "Size": [200, 240]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("View.Container");
    VERIFYF(View->Scrollable()->VerticalScrollBar()->ShouldPaint(), "Vertical scroll bar is not painted when it should be.");
    VERIFYF(View->Scrollable()->Overflow().Y == View->Scrollable()->HorizontalScrollBar()->GetSize().Y + 40.0f,
        "Vertical overflow is %.2f when it should be %.2f.",
        View->Scrollable()->Overflow().Y, View->Scrollable()->VerticalScrollBar()->GetSize().Y + 40.0f);
    View->Scrollable()->SetOffset({ 0.0f, View->Scrollable()->Overflow().Y });
    Application.Update();
    VERIFYF(View->Scrollable()->Offset().Y == View->Scrollable()->Overflow().Y,
        "Scrollable vertical offset %.2f does not match overflow %.2f",
        View->Scrollable()->Offset().Y, View->Scrollable()->Overflow().Y);
    Container->SetSize({ 200.0f, 220.0f });
    Application.Update();
    VERIFYF(View->Scrollable()->Overflow().Y == View->Scrollable()->HorizontalScrollBar()->GetSize().Y + 20.0f,
        "Vertical overflow is %.2f when it should be %.2f",
        View->Scrollable()->Offset().Y, View->Scrollable()->HorizontalScrollBar()->GetSize().Y + 20.0f);
    VERIFYF(View->Scrollable()->Offset().Y == View->Scrollable()->Overflow().Y,
        "Vertical offset is %.2f when it should be %.2f",
        View->Scrollable()->Offset().Y, View->Scrollable()->Overflow().Y);
    return true;
})

TEST_CASE(ScrollBarPositions,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "Container", "Size": [300, 300]})", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    const float SBSize = View->Scrollable()->ScrollBarPropertySize();

    const std::shared_ptr<OctaneGUI::ScrollBar>& HScrollBar = View->Scrollable()->HorizontalScrollBar();
    VERIFYF(HScrollBar->GetPosition().Y == View->GetSize().Y - SBSize, "HScrollBar is not positioned at %.2f. Positioned at %.2f.",
        View->GetSize().Y - SBSize, HScrollBar->GetPosition().Y);

    const std::shared_ptr<OctaneGUI::ScrollBar>& VScrollBar = View->Scrollable()->VerticalScrollBar();
    VERIFYF(VScrollBar->GetPosition().X == View->GetSize().X - SBSize, "VScrollBar is not positioned at %.2f. Positioned at %.2f",
        View->GetSize().X - SBSize, VScrollBar->GetPosition().X);

    return true;
})

TEST_CASE(ContextMenu,
{
    OctaneGUI::ControlList List;
    Load(Application, "", List, 200, 200);
    const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
    return Utility::ContextMenu(Application, View);
})

)

}
