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

static void Load(OctaneGUI::Application& Application, const char* JsonControls, OctaneGUI::ControlList& List)
{
    Utility::Load(Application, JsonControls, List);
}

TEST_SUITE(Container,

TEST_CASE(ExpandWidth,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"ID": "Container", "Type": "Container", "Expand": "Width"})", List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("Container");
    return Container->GetSize().X == WindowSize.X && Container->GetSize().Y != WindowSize.Y;
})

TEST_CASE(ExpandHeight,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"ID": "Container", "Type": "Container", "Expand": "Height"})", List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("Container");
    return Container->GetSize().X != WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(ExpandBoth,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"ID": "Container", "Type": "Container", "Expand": "Both"})", List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("Container");
    return Container->GetSize().X == WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthControl,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "HorizontalContainer", "Expand": "Width", "Controls": [
        {"ID": "Button", "Type": "TextButton", "Expand": "Width", "Text": {"Text": "Button"}}]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().X == 0.0f && Button->GetSize().X == WindowSize.X;
})

TEST_CASE(FillHeightControl,
{
    OctaneGUI::ControlList List;
    Load(Application, R"({"Type": "VerticalContainer", "Expand": "Height", "Controls": [{"ID": "Button", "Type": "TextButton", "Expand": "Height", "Text": {"Text": "Button"}}]})", List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().Y == 0.0f && Button->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application, 
        R"({"ID": "Container", "Type": "HorizontalContainer", "Expand": "Width", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Expand": "Width", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Expand": "Width", "Text": {"Text": "Button"}}
        ]})", 
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const float HalfWindowWidth = WindowSize.X * 0.5f;
    const float HalfSpacingWidth = Container->Spacing().X * 0.5f;
    return
        Button1->GetAbsolutePosition().X == 0.0f && Button1->GetSize().X == std::floor(HalfWindowWidth - HalfSpacingWidth) &&
        Button2->GetAbsolutePosition().X == std::floor(HalfWindowWidth + HalfSpacingWidth) && Button2->GetSize().X == std::floor(HalfWindowWidth - HalfSpacingWidth);
})

TEST_CASE(FillWidthMultiMixControls,
{
    OctaneGUI::ControlList List;
    Load(Application, 
        R"({"ID": "Container", "Type": "HorizontalContainer", "Expand": "Width", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Expand": "Width", "Text": {"Text": "Button"}},
            {"ID": "Text", "Type": "Text", "Text": "Text"},
            {"ID": "Button2", "Type": "TextButton", "Expand": "Width", "Text": {"Text": "Button"}}
        ]})", 
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Container.Text");
    const float HalfWindowWidth = (WindowSize.X - Text->GetSize().X) * 0.5f;
    const float HalfSpacingWidth = Container->Spacing().X * (float)(Container->Controls().size() - 1) * 0.5f;
    return
        Button1->GetAbsolutePosition().X == 0.0f && std::floor(Button1->GetSize().X) == std::floor(HalfWindowWidth - HalfSpacingWidth) &&
        std::floor(Button2->GetAbsolutePosition().X) == std::floor(Button1->GetSize().X + Text->GetSize().X + HalfSpacingWidth * 2.0f) &&
        std::floor(Button2->GetSize().X) == std::floor(HalfWindowWidth - HalfSpacingWidth);
})

TEST_CASE(FillHeightMultiMixControls,
{
    OctaneGUI::ControlList List;
    Load(Application, 
        R"({"ID": "Container", "Type": "VerticalContainer", "Expand": "Height", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Expand": "Height", "Text": {"Text": "Button"}},
            {"ID": "Text", "Type": "Text", "Text": "Text"},
            {"ID": "Button2", "Type": "TextButton", "Expand": "Height", "Text": {"Text": "Button"}}
        ]})", 
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const std::shared_ptr<OctaneGUI::Text> Text = List.To<OctaneGUI::Text>("Container.Text");
    const float HalfWindowHeight = (WindowSize.Y - Text->GetSize().Y) * 0.5f;
    const float HalfSpacingHeight = Container->Spacing().Y * (float)(Container->Controls().size() - 1) * 0.5f;
    return
        Button1->GetAbsolutePosition().Y == 0.0f && std::floor(Button1->GetSize().Y) == std::floor(HalfWindowHeight - HalfSpacingHeight) &&
        std::floor(Button2->GetAbsolutePosition().Y) == std::floor(Button1->GetSize().Y + Text->GetSize().Y + HalfSpacingHeight * 2.0f) &&
        std::floor(Button2->GetSize().Y) == std::floor(HalfWindowHeight - HalfSpacingHeight);
})

TEST_CASE(FillHeighthMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application, 
        R"({"ID": "Container", "Type": "VerticalContainer", "Expand": "Height", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Expand": "Height", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Expand": "Height", "Text": {"Text": "Button"}}
        ]})", 
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const float HalfWindowHeight = WindowSize.Y * 0.5f;
    const float HalfSpacingHeight = Container->Spacing().Y * 0.5f;
    return
        Button1->GetAbsolutePosition().Y == 0.0f && Button1->GetSize().Y == std::floor(HalfWindowHeight - HalfSpacingHeight) &&
        Button2->GetAbsolutePosition().Y == std::floor(HalfWindowHeight + HalfSpacingHeight) && Button2->GetSize().Y == std::floor(HalfWindowHeight - HalfSpacingHeight);
})

TEST_CASE(HGrowCenter,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"Type": "HorizontalContainer", "Expand": "Width", "Grow": "Center", "Controls": [
            {"ID": "Button", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - Button->GetSize().X * 0.5f);
})

TEST_CASE(HGrowEnd,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"Type": "HorizontalContainer", "Expand": "Width", "Grow": "End", "Controls": [
            {"ID": "Button", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().X == std::floor(WindowSize.X - Button->GetSize().X);
})

TEST_CASE(VGrowCenter,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"Type": "VerticalContainer", "Expand": "Height", "Grow": "Center", "Controls": [
            {"ID": "Button", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - Button->GetSize().Y * 0.5f);
})

TEST_CASE(VGrowEnd,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"Type": "VerticalContainer", "Expand": "Height", "Grow": "End", "Controls": [
            {"ID": "Button", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Button");
    return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y - Button->GetSize().Y);
})

TEST_CASE(HGrowCenterMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "HorizontalContainer", "Expand": "Width", "Grow": "Center", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const OctaneGUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X + Container->Spacing().X, Button1->GetSize().Y);
    const float HalfWindowWidth = WindowSize.X * 0.5f;
    const float HalfTotalWidth = TotalSize.X * 0.5f;
    const float HalfSpacingWidth = Container->Spacing().X * 0.5f;
    return 
        Button1->GetAbsolutePosition().X == std::floor(HalfWindowWidth - HalfTotalWidth) &&
        Button2->GetAbsolutePosition().X == std::floor(HalfWindowWidth + HalfSpacingWidth);
})

TEST_CASE(HGrowEndMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "HorizontalContainer", "Expand": "Width", "Grow": "End", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const OctaneGUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X + Container->Spacing().X, Button1->GetSize().Y);
    const float HalfTotalWidth = TotalSize.X * 0.5f;
    const float HalfSpacingWidth = Container->Spacing().X * 0.5f;
    return 
        Button1->GetAbsolutePosition().X == std::floor(WindowSize.X - TotalSize.X) &&
        Button2->GetAbsolutePosition().X == std::floor(WindowSize.X - HalfTotalWidth + HalfSpacingWidth);
})

TEST_CASE(VGrowCenterMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "VerticalContainer", "Expand": "Height", "Grow": "Center", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const OctaneGUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y + Container->Spacing().Y);
    const float HalfWindowHeight = WindowSize.Y * 0.5f;
    const float HalfTotalHeight = TotalSize.Y * 0.5f;
    const float HalfSpacingHeight = Container->Spacing().Y * 0.5f;
    return 
        Button1->GetAbsolutePosition().Y == std::floor(HalfWindowHeight - HalfTotalHeight) &&
        Button2->GetAbsolutePosition().Y == std::floor(HalfWindowHeight + HalfSpacingHeight);
})

TEST_CASE(VGrowEndMultiControls,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "VerticalContainer", "Expand": "Height", "Grow": "End", "Controls": [
            {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button"}},
            {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button"}}
        ]})",
    List);
    const OctaneGUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
    const std::shared_ptr<OctaneGUI::BoxContainer> Container = List.To<OctaneGUI::BoxContainer>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    const OctaneGUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y + Container->Spacing().Y);
    const float HalfTotalHeight = TotalSize.Y * 0.5f;
    const float HalfSpacingHeight = Container->Spacing().Y * 0.5f;
    return 
        Button1->GetAbsolutePosition().Y == std::floor(WindowSize.Y - TotalSize.Y) &&
        Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y - HalfTotalHeight + HalfSpacingHeight);
})

TEST_CASE(MarginContainer,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Margins", "Type": "MarginContainer", "Margins": [4, 4, 4, 4], "Controls": [
        {"ID": "Button", "Type": "TextButton", "Expand": "Both", "Text": {"Text": "Button"}}]})",
        List);
    const std::shared_ptr<OctaneGUI::Container> Margins = List.To<OctaneGUI::Container>("Margins");
    const std::shared_ptr<OctaneGUI::Button> Button = List.To<OctaneGUI::Button>("Margins.Button");
    const OctaneGUI::Vector2 Position = Button->GetPosition();
    const OctaneGUI::Vector2 Size = Button->GetSize();
    return Position.X == 4.0f && Position.Y == 4.0f && Size.X == Margins->GetSize().X - 8.0f && Size.Y == Margins->GetSize().Y - 8.0f;
})

TEST_CASE(HorizontalSpacing,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "HorizontalContainer", "Spacing": [4.0, 4.0], "Controls": [
        {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button 1"}},
        {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button 2"}}]})",
        List);
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    // FIXME: Need to floor here since position is floored in the library. Should switch all cases of floor
    // to roundf.
    return Button2->GetPosition().X == std::floor(Button1->GetSize().X) + 4.0f &&
        Button2->GetPosition().X + std::roundf(Button2->GetSize().X) == std::roundf(Container->GetSize().X);
})

TEST_CASE(VerticalSpacing,
{
    OctaneGUI::ControlList List;
    Load(Application,
        R"({"ID": "Container", "Type": "VerticalContainer", "Spacing": [4.0, 4.0], "Controls": [
        {"ID": "Button1", "Type": "TextButton", "Text": {"Text": "Button 1"}},
        {"ID": "Button2", "Type": "TextButton", "Text": {"Text": "Button 2"}}]})",
        List);
    const std::shared_ptr<OctaneGUI::Container> Container = List.To<OctaneGUI::Container>("Container");
    const std::shared_ptr<OctaneGUI::Button> Button1 = List.To<OctaneGUI::Button>("Container.Button1");
    const std::shared_ptr<OctaneGUI::Button> Button2 = List.To<OctaneGUI::Button>("Container.Button2");
    return Button2->GetPosition().Y == Button1->GetSize().Y + 4.0f &&
        Button2->GetPosition().Y + Button2->GetSize().Y == Container->GetSize().Y;
})

)

}
