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

#include <cmath>

namespace Tests
{

void LoadSplitter(OctaneGUI::Application& Application, const char* Containers, OctaneGUI::ControlList& List, bool Fit)
{
    std::string JSON = R"({"Type": "Splitter", "ID": "Splitter", "Expand": "Both")";
    JSON += std::string(R"(, "Fit": )") + (Fit ? "true" : "false");
    JSON += R"(, "Containers": [)";
    JSON += Containers;
    JSON += "]}";
    Utility::Load(Application, JSON.c_str(), List);
}

TEST_SUITE(Splitter,

TEST_CASE(TwoContainers,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, false);
    return List.To<OctaneGUI::Splitter>("Splitter")->Count() == 2;
})

TEST_CASE(FourContainers,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {}, {}, {})", List, false);
    return List.To<OctaneGUI::Splitter>("Splitter")->Count() == 4;
})

TEST_CASE(SplitterHorizontalSizeFit,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, true);

    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    const std::shared_ptr<OctaneGUI::Container> Top = Splitter->GetSplit(0);
    const OctaneGUI::Vector2 SplitterSize = Splitter->SplitterSize();
    const float CenterY = (WindowSize.Y * 0.5f) - (SplitterSize.Y * 0.5f);

    return std::floor(Top->GetSize().Y) == CenterY;
})

TEST_CASE(ChangeSplitterVerticalFit,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, true);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    Splitter->SetOrientation(OctaneGUI::Orientation::Vertical);
    Application.Update();

    const std::shared_ptr<OctaneGUI::Container> Left = Splitter->GetSplit(0);
    const std::shared_ptr<OctaneGUI::Container> Right = Splitter->GetSplit(1);
    const OctaneGUI::Vector2 SplitterSize { Splitter->SplitterSize() };

    return Left->GetSize().X == Right->GetSize().X && Left->GetSize().X == Splitter->GetSize().X * 0.5f - SplitterSize.X * 0.5f;
})

TEST_CASE(SplitterHorizontalMoveFit,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, true);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    const OctaneGUI::Vector2 Size { Splitter->GetSize() };
    const OctaneGUI::Vector2 SplitterSize  { Splitter->SplitterSize() };
    const std::shared_ptr<OctaneGUI::Container> Top = Splitter->GetSplit(0);
    const std::shared_ptr<OctaneGUI::Container> Bottom = Splitter->GetSplit(1);

    VERIFYF(Top->GetSize().Y == Size.Y * 0.5f - SplitterSize.Y * 0.5f, "Top container size is not split in half!");
    VERIFYF(Bottom->GetSize().Y == Size.Y * 0.5f - SplitterSize.Y * 0.5f, "Bottom container size is not split in half!");

    OctaneGUI::Vector2 Mouse(5.0f, Top->GetSize().Y + 1.0f);
    Utility::MousePress(Application, Mouse);
    Application.Update();

    Mouse.Y -= 5.0f;
    Utility::MouseMove(Application, Mouse);
    Application.Update();

    Utility::MouseRelease(Application, Mouse);
    Application.Update();

    return Top->GetSize().Y == (Size.Y * 0.5f) - (SplitterSize.Y * 0.5f) - 5.0f &&
        Bottom->GetSize().Y == (Size.Y * 0.5f) - (SplitterSize.Y * 0.5f) + 5.0f;
})

TEST_CASE(SplitterVerticalMoveFit,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, true);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    Splitter->SetOrientation(OctaneGUI::Orientation::Vertical);
    Application.Update();

    const OctaneGUI::Vector2 Size { Splitter->GetSize() };
    const OctaneGUI::Vector2 SplitterSize  { Splitter->SplitterSize() };
    const std::shared_ptr<OctaneGUI::Container> Left = Splitter->GetSplit(0);
    const std::shared_ptr<OctaneGUI::Container> Right = Splitter->GetSplit(1);

    VERIFYF(Left->GetSize().X == Size.X * 0.5f - SplitterSize.X * 0.5f, "Left container size is not split in half!");
    VERIFYF(Right->GetSize().X == Size.X * 0.5f - SplitterSize.X * 0.5f, "Right container size is not split in half!");

    OctaneGUI::Vector2 Mouse(Left->GetSize().X + 1.0f, 2.0f);
    Utility::MousePress(Application, Mouse);
    Application.Update();

    Mouse.X += 5.0f;
    Utility::MouseMove(Application, Mouse);
    Application.Update();

    Utility::MouseRelease(Application, Mouse);
    Application.Update();

    return Left->GetSize().X == (Size.X * 0.5f) - (SplitterSize.X * 0.5f) + 5.0f &&
        Right->GetSize().X == (Size.X * 0.5f) - (SplitterSize.X * 0.5f) - 5.0f;
})

TEST_CASE(SplitterHoveredControl,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({"Controls": [{"Type": "TextButton", "ID": "Button", "Text": {"Text": "Hello"}}]}, {})", List, false);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Splitter.Button");
    const std::shared_ptr<OctaneGUI::Container> Top = Splitter->GetSplit(0);

    Utility::MouseClick(Application, {5.0f, Top->GetSize().Y + 1.0f});
    Application.Update();

    VERIFYF(!Application.GetMainWindow()->Focus().expired(), "No control is focused!");

    bool Clicked = false;
    Button->SetOnClicked([&](OctaneGUI::Button&) -> void
        {
            Clicked = true;
        });

    Utility::MouseClick(Application, Button->GetAbsolutePosition());

    return Clicked;
})

TEST_CASE(SplitterSetSize,
{
    OctaneGUI::ControlList List;
    LoadSplitter(Application, R"({}, {})", List, false);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = List.To<OctaneGUI::Splitter>("Splitter");
    Splitter->SetOrientation(OctaneGUI::Orientation::Vertical);
    Application.Update();
    const std::shared_ptr<OctaneGUI::Container> Left = Splitter->GetSplit(0);

    VERIFYF(Left->GetSize().X == 0.0f, "Left container's size should be 0 but it is %.2f", Left->GetSize().X);

    Splitter->SetSplitterSize(0, 100.0f);
    Application.Update();

    return Left->GetSize().X == 100.0f;
})

TEST_CASE(InitialSplitterSize,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", List);

    const std::shared_ptr<OctaneGUI::Splitter> Splitter = Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::Splitter>();
    Splitter
        ->AddContainers(2)
        .SetSplitterSize(0, 100.0f)
        .SetExpand(OctaneGUI::Expand::Both);
    Application.Update();

    VERIFYF(Splitter->GetSize().X == 1280.0f && Splitter->GetSize().Y == 720.0f, 
        "Splitter size is not expanded! Size is %.2f %.2f.", Splitter->GetSize().X, Splitter->GetSize().Y);
    
    const std::shared_ptr<OctaneGUI::Container> Top = Splitter->GetSplit(0);
    return Top->GetSize().Y == 100.0f;
})

)

}
