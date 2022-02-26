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

static void Load(OctaneUI::Application& Application, const char* JsonControls, OctaneUI::ControlList& List)
{
	static const char* Base = "{\"Width\": 1280, \"Height\": 720, \"Body\": {\"Controls\": [";
	std::string Json = Base;
	Json += JsonControls;
	Json += "]}";
	Application.GetMainWindow()->Load(Json.c_str(), List);
	Application.GetMainWindow()->Update();
}

// TODO: May need to update when theme spacing is implemented.

TEST_SUITE(Container,

TEST_CASE(ExpandWidth,
{
	OctaneUI::ControlList List;
	Load(Application, "{\"ID\": \"Container\", \"Type\": \"Container\", \"Expand\": \"Width\"}", List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	std::shared_ptr<OctaneUI::Container> Container = List.To<OctaneUI::Container>("Container");
	return Container->GetSize().X == WindowSize.X && Container->GetSize().Y != WindowSize.Y;
})

TEST_CASE(ExpandHeight,
{
	OctaneUI::ControlList List;
	Load(Application, "{\"ID\": \"Container\", \"Type\": \"Container\", \"Expand\": \"Height\"}", List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	std::shared_ptr<OctaneUI::Container> Container = List.To<OctaneUI::Container>("Container");
	return Container->GetSize().X != WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(ExpandBoth,
{
	OctaneUI::ControlList List;
	Load(Application, "{\"ID\": \"Container\", \"Type\": \"Container\", \"Expand\": \"Both\"}", List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	std::shared_ptr<OctaneUI::Container> Container = List.To<OctaneUI::Container>("Container");
	return Container->GetSize().X == WindowSize.X && Container->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthControl,
{
	OctaneUI::ControlList List;
	Load(Application, "{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Controls\": ["
		"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Expand\": \"Width\", \"Text\": {\"Text\": \"Button\"}}]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().X == 0.0f && Button->GetSize().X == WindowSize.X;
})

TEST_CASE(FillHeightControl,
{
	OctaneUI::ControlList List;
	Load(Application, "{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Controls\": [{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Expand\": \"Height\", \"Text\": {\"Text\": \"Button\"}}]}", List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().Y == 0.0f && Button->GetSize().Y == WindowSize.Y;
})

TEST_CASE(FillWidthMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application, 
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Expand\": \"Width\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Expand\": \"Width\", \"Text\": {\"Text\": \"Button\"}}"
		"]}", 
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	return
		Button1->GetAbsolutePosition().X == 0.0f && Button1->GetSize().X == std::floor(WindowSize.X * 0.5f) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f) && Button2->GetSize().X == std::floor(WindowSize.X * 0.5f);
})

TEST_CASE(FillWidthMultiMixControls,
{
	OctaneUI::ControlList List;
	Load(Application, 
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Expand\": \"Width\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Text\", \"Type\": \"Text\", \"Text\": \"Text\"},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Expand\": \"Width\", \"Text\": {\"Text\": \"Button\"}}"
		"]}", 
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const std::shared_ptr<OctaneUI::Text> Text = List.To<OctaneUI::Text>("Text");
	return
		Button1->GetAbsolutePosition().X == 0.0f && std::floor(Button1->GetSize().X) == std::floor((WindowSize.X - Text->GetSize().X) * 0.5f) &&
		std::floor(Button2->GetAbsolutePosition().X) == std::floor(Button1->GetSize().X + Text->GetSize().X) &&
		std::floor(Button2->GetSize().X) == std::floor((WindowSize.X - Text->GetSize().X) * 0.5f);
})

TEST_CASE(FillHeightMultiMixControls,
{
	OctaneUI::ControlList List;
	Load(Application, 
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Expand\": \"Height\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Text\", \"Type\": \"Text\", \"Text\": \"Text\"},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Expand\": \"Height\", \"Text\": {\"Text\": \"Button\"}}"
		"]}", 
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const std::shared_ptr<OctaneUI::Text> Text = List.To<OctaneUI::Text>("Text");
	return
		Button1->GetAbsolutePosition().Y == 0.0f && std::floor(Button1->GetSize().Y) == std::floor((WindowSize.Y - Text->GetSize().Y) * 0.5f) &&
		std::floor(Button2->GetAbsolutePosition().Y) == std::floor(Button1->GetSize().Y + Text->GetSize().Y) &&
		std::floor(Button2->GetSize().Y) == std::floor((WindowSize.Y - Text->GetSize().Y) * 0.5f);
})

TEST_CASE(FillHeighthMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application, 
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Expand\": \"Height\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Expand\": \"Height\", \"Text\": {\"Text\": \"Button\"}}"
		"]}", 
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	return
		Button1->GetAbsolutePosition().Y == 0.0f && Button1->GetSize().Y == std::floor(WindowSize.Y * 0.5f) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f) && Button2->GetSize().Y == std::floor(WindowSize.Y * 0.5f);
})

TEST_CASE(HGrowCenter,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Grow\": \"Center\", \"Controls\": ["
			"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - Button->GetSize().X * 0.5f);
})

TEST_CASE(HGrowEnd,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Grow\": \"End\", \"Controls\": ["
			"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().X == std::floor(WindowSize.X - Button->GetSize().X);
})

TEST_CASE(VGrowCenter,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Grow\": \"Center\", \"Controls\": ["
			"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - Button->GetSize().Y * 0.5f);
})

TEST_CASE(VGrowEnd,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Grow\": \"End\", \"Controls\": ["
			"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button = List.To<OctaneUI::Button>("Button");
	return Button->GetAbsolutePosition().Y == std::floor(WindowSize.Y - Button->GetSize().Y);
})

TEST_CASE(HGrowCenterMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Grow\": \"Center\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X, Button1->GetSize().Y);
	return 
		Button1->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - TotalSize.X * 0.5f) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X * 0.5f - TotalSize.X * 0.5f + Button1->GetSize().X);
})

TEST_CASE(HGrowEndMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Grow\": \"End\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X + Button2->GetSize().X, Button1->GetSize().Y);
	return 
		Button1->GetAbsolutePosition().X == std::floor(WindowSize.X - TotalSize.X) &&
		Button2->GetAbsolutePosition().X == std::floor(WindowSize.X - TotalSize.X + Button1->GetSize().X);
})

TEST_CASE(VGrowCenterMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Grow\": \"Center\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y);
	return 
		Button1->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - TotalSize.Y * 0.5f) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y * 0.5f - TotalSize.Y * 0.5f + Button1->GetSize().Y);
})

TEST_CASE(VGrowEndMultiControls,
{
	OctaneUI::ControlList List;
	Load(Application,
		"{\"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Grow\": \"End\", \"Controls\": ["
			"{\"ID\": \"Button1\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}},"
			"{\"ID\": \"Button2\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Button\"}}"
		"]}",
	List);
	const OctaneUI::Vector2 WindowSize = Application.GetMainWindow()->GetSize();
	const std::shared_ptr<OctaneUI::Button> Button1 = List.To<OctaneUI::Button>("Button1");
	const std::shared_ptr<OctaneUI::Button> Button2 = List.To<OctaneUI::Button>("Button2");
	const OctaneUI::Vector2 TotalSize(Button1->GetSize().X, Button1->GetSize().Y + Button2->GetSize().Y);
	return 
		Button1->GetAbsolutePosition().Y == std::floor(WindowSize.Y - TotalSize.Y) &&
		Button2->GetAbsolutePosition().Y == std::floor(WindowSize.Y - TotalSize.Y + Button1->GetSize().Y);
})

)

}
