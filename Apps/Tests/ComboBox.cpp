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

static const char* Json = "{\"Width\": 1280, \"Height\": 720, \"Body\": {\"Controls\": [{\"Type\": \"ComboBox\", \"ID\": \"ComboBox\"}]}}";

TEST_SUITE(ComboBox,

TEST_CASE(Select,
{
	OctaneGUI::ControlList List;
	Application.GetMainWindow()->Load(Json, List);
	Application.GetMainWindow()->Update();

	VERIFY(List.Contains("ComboBox"))
	std::shared_ptr<OctaneGUI::ComboBox> ComboBox = List.To<OctaneGUI::ComboBox>("ComboBox");

	std::u32string Selected;
	ComboBox->SetOnSelected([&Selected](const std::u32string& Item) -> void
		{
			Selected = Item;
		});

	OctaneGUI::Json Items(OctaneGUI::Json::Type::Array);
	Items.Push("Red");
	Items.Push("Green");
	Items.Push("Blue");

	OctaneGUI::Json Root(OctaneGUI::Json::Type::Object);
	Root["Items"] = std::move(Items);
	ComboBox->OnLoad(Root);
	Application.GetMainWindow()->Update();

	OctaneGUI::Vector2 Position = ComboBox->GetAbsolutePosition() + OctaneGUI::Vector2(2.0f, 2.0f);
	Utility::MouseClick(Application, Position);
	VERIFY(!ComboBox->IsOpen())

	Position = ComboBox->GetAbsoluteBounds().Max - OctaneGUI::Vector2(2.0f, 2.0f);
	Utility::MouseClick(Application, Position);
	Application.GetMainWindow()->Update();
	VERIFY(ComboBox->IsOpen())

	Position = ComboBox->GetAbsolutePosition() + OctaneGUI::Vector2(2.0f, ComboBox->GetSize().Y + 15.0f);
	Utility::MouseClick(Application, Position);
	Application.GetMainWindow()->Update();

	VERIFYF(Selected == U"Red", "Selected item does not match the expected result: %s != Red\n", OctaneGUI::String::ToMultiByte(Selected).c_str());
	VERIFY(!ComboBox->IsOpen());

	Selected = U"";
	Position = ComboBox->GetAbsoluteBounds().Max - OctaneGUI::Vector2(2.0f, 2.0f);
	Utility::MouseClick(Application, Position);
	Application.GetMainWindow()->Update();
	VERIFY(ComboBox->IsOpen())

	const float Height = Application.GetTheme()->GetFont()->Size();
	Position = ComboBox->GetAbsoluteBounds().Max + OctaneGUI::Vector2(-2.0f, ComboBox->GetSize().Y + Height - 2.0f);
	Utility::MouseClick(Application, Position);
	Application.GetMainWindow()->Update();

	VERIFYF(Selected == U"Green", "Selected item does not match the expected result: %s != Green\n", OctaneGUI::String::ToMultiByte(Selected).c_str());
	VERIFY(!ComboBox->IsOpen());

	return true;
})

)

}
