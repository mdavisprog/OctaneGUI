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

#include "OctaneUI/OctaneUI.h"
#include "TestSuite.h"

#include <cmath>

namespace Tests
{

// TODO: May need to update when theme spacing is implemented.

TEST_SUITE(Container,

TEST_CASE(ExpandWidth,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Container> Container = Application.GetMainWindow()->GetContainer()->AddContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	Application.GetMainWindow()->Update();
	return Container->GetSize().X == WindowSize.X && Container->GetSize().Y != WindowSize.Y;
})

TEST_CASE(ExpandHeight,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Container> Container = Application.GetMainWindow()->GetContainer()->AddContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	Application.GetMainWindow()->Update();
	return Container->GetSize().X != WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(ExpandBoth,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Container> Container = Application.GetMainWindow()->GetContainer()->AddContainer();
	Container->SetExpand(OctaneUI::Expand::Both);
	Application.GetMainWindow()->Update();
	return Container->GetSize().X == WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthControl,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	Button->SetExpand(OctaneUI::Expand::Width);
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().X == 0.0f && Button->GetSize().X == WindowSize.X;
})

TEST_CASE(FillHeightControl,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	Button->SetExpand(OctaneUI::Expand::Height);
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().Y == 0.0f && Button->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	Button1->SetExpand(OctaneUI::Expand::Width);
	Button2->SetExpand(OctaneUI::Expand::Width);
	Application.GetMainWindow()->Update();
	return
		Button1->GetAbsolutePosition().X == 0.0f && Button1->GetSize().X == std::floor(WindowSize.X * 0.5f) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f) && Button2->GetSize().X == std::floor(WindowSize.X * 0.5f);
})

TEST_CASE(FillWidthMultiMixControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Text> Text = Container->AddText("Text");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	Button1->SetExpand(OctaneUI::Expand::Width);
	Button2->SetExpand(OctaneUI::Expand::Width);
	Application.GetMainWindow()->Update();
	return
		Button1->GetAbsolutePosition().X == 0.0f && std::floor(Button1->GetSize().X) == std::floor((WindowSize.X - Text->GetSize().X) * 0.5f) &&
		std::floor(Button2->GetAbsolutePosition().X) == std::floor(Button1->GetSize().X + Text->GetSize().X) &&
		std::floor(Button2->GetSize().X) == std::floor((WindowSize.X - Text->GetSize().X) * 0.5f);
})

TEST_CASE(FillHeightMultiMixControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Text> Text = Container->AddText("Text");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	Button1->SetExpand(OctaneUI::Expand::Height);
	Button2->SetExpand(OctaneUI::Expand::Height);
	Application.GetMainWindow()->Update();
	return
		Button1->GetAbsolutePosition().Y == 0.0f && std::floor(Button1->GetSize().Y) == std::floor((WindowSize.Y - Text->GetSize().Y) * 0.5f) &&
		std::floor(Button2->GetAbsolutePosition().Y) == std::floor(Button1->GetSize().Y + Text->GetSize().Y) &&
		std::floor(Button2->GetSize().Y) == std::floor((WindowSize.Y - Text->GetSize().Y) * 0.5f);
})

TEST_CASE(FillHeighthMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	Button1->SetExpand(OctaneUI::Expand::Height);
	Button2->SetExpand(OctaneUI::Expand::Height);
	Application.GetMainWindow()->Update();
	return
		Button1->GetAbsolutePosition().Y == 0.0f && Button1->GetSize().Y == std::floor(WindowSize.Y * 0.5f) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f) && Button2->GetSize().Y == std::floor(WindowSize.Y * 0.5f);
})

TEST_CASE(HGrowCenter,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	Container->SetGrow(OctaneUI::Grow::Center);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	const OctaneUI::Vector2 ButtonSize = Button->GetSize();
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - ButtonSize.X * 0.5f);
})

TEST_CASE(HGrowEnd,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	Container->SetGrow(OctaneUI::Grow::End);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	const OctaneUI::Vector2 ButtonSize = Button->GetSize();
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().X == std::floor(WindowSize.X - ButtonSize.X);
})

TEST_CASE(VGrowCenter,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	Container->SetGrow(OctaneUI::Grow::Center);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	const OctaneUI::Vector2 ButtonSize = Button->GetSize();
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - ButtonSize.Y * 0.5f);
})

TEST_CASE(VGrowEnd,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	Container->SetGrow(OctaneUI::Grow::End);
	const std::shared_ptr<OctaneUI::Button> Button = Container->AddButton("Button");
	const OctaneUI::Vector2 ButtonSize = Button->GetSize();
	Application.GetMainWindow()->Update();
	return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y - ButtonSize.Y);
})

TEST_CASE(HGrowCenterMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	Container->SetGrow(OctaneUI::Grow::Center);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X, Button1->GetSize().Y);
	Application.GetMainWindow()->Update();
	return 
		Button1->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - TotalSize.X * 0.5f) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - TotalSize.X * 0.5f + Button1->GetSize().X);
})

TEST_CASE(HGrowEndMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::HorizontalContainer> Container = Application.GetMainWindow()->GetContainer()->AddHorizontalContainer();
	Container->SetExpand(OctaneUI::Expand::Width);
	Container->SetGrow(OctaneUI::Grow::End);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X, Button1->GetSize().Y);
	Application.GetMainWindow()->Update();
	return 
		Button1->GetAbsolutePosition().X == std::floor(WindowSize.X - TotalSize.X) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X - TotalSize.X + Button1->GetSize().X);
})

TEST_CASE(VGrowCenterMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	Container->SetGrow(OctaneUI::Grow::Center);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y);
	Application.GetMainWindow()->Update();
	return 
		Button1->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - TotalSize.Y * 0.5f) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - TotalSize.Y * 0.5f + Button1->GetSize().Y);
})

TEST_CASE(VGrowEndMultiControls,
{
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::VerticalContainer> Container = Application.GetMainWindow()->GetContainer()->AddVerticalContainer();
	Container->SetExpand(OctaneUI::Expand::Height);
	Container->SetGrow(OctaneUI::Grow::End);
	const std::shared_ptr<OctaneUI::Button> Button1 = Container->AddButton("Button");
	const std::shared_ptr<OctaneUI::Button> Button2 = Container->AddButton("Button");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y);
	Application.GetMainWindow()->Update();
	return 
		Button1->GetAbsolutePosition().Y == std::floor(WindowSize.Y - TotalSize.Y) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y - TotalSize.Y + Button1->GetSize().Y);
})

)

}
