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

void Load(OctaneGUI::Application& Application, const std::vector<std::string>& Items, OctaneGUI::ControlList& List)
{
	std::string Json = "{\"Type\": \"ListBox\", \"ID\": \"ListBox\", \"Expand\": \"Both\", \"Controls\": [";
	for (const std::string& Item : Items)
	{
		Json += "{\"Type\": \"TextSelectable\", \"Text\": {\"Text\": \"" + Item + "\"}}";
		if (Item != Items.back())
		{
			Json += ",";
		}
	}
	Json += "]}";
	Utility::Load(Application, Json.c_str(), List);
}

TEST_SUITE(ListBox,

TEST_CASE(Selection,
{
	OctaneGUI::ControlList List;
	Load(Application, {"One", "Two", "Three"}, List);

	const std::shared_ptr<OctaneGUI::ListBox> ListBox = List.To<OctaneGUI::ListBox>("ListBox");

	int Selected = -1;
	ListBox->SetOnSelect([&](int Index, std::weak_ptr<OctaneGUI::Control> Control) -> void
		{
			Selected = Index;
		});
	
	const OctaneGUI::Vector2 Position = ListBox->GetAbsolutePosition();
	const OctaneGUI::Vector2 Size = ListBox->GetSize();

	Utility::MouseClick(Application, Position + OctaneGUI::Vector2{5.0f, 5.0f});
	VERIFYF(Selected == 0, "Invalid selected index %d. Should be 0!", Selected);
	VERIFYF(Selected == ListBox->Index(), "Callback index %d doesn't match ListBox index %d!", Selected, ListBox->Index());

	Utility::MouseClick(Application, Size + OctaneGUI::Vector2{-5.0f, 5.0f});
	VERIFYF(Selected == 0, "Invalid selected index %d. Should be 0!", Selected);

	Utility::MouseClick(Application, ListBox->Item(1)->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 5.0f});
	VERIFYF(Selected == 1, "Invalid selected index %d. Should be 1!", Selected);

	Utility::MouseClick(Application, ListBox->Item(2)->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 5.0f});
	VERIFYF(Selected == 2, "Invalid selected index %d. Should be 2!", Selected);

	ListBox->Deselect();
	Utility::MouseClick(Application, ListBox->Item(2)->GetAbsolutePosition() + OctaneGUI::Vector2{5.0f, 50.0f});
	VERIFYF(Selected == 2 && ListBox->Index() == -1, "ListBox index should be deselected when it is %d!", ListBox->Index());

	return true;
})

TEST_CASE(HScrollSelection,
{
	OctaneGUI::ControlList List;
	Load(Application, {"One"}, List);

	const std::shared_ptr<OctaneGUI::ListBox> ListBox = List.To<OctaneGUI::ListBox>("ListBox");
	
	std::string LongString = "The quick brown fox jumped over the lazy dog.";
	for (int I = 0; I < 3; I++)
	{
		LongString += LongString;
	}
	ListBox->AddItem<OctaneGUI::TextSelectable>()->SetText(LongString.c_str());
	Application.Update();

	int Selected = -1;
	ListBox->SetOnSelect([&](int Index, std::weak_ptr<OctaneGUI::Control> Item) -> void
		{
			Selected = Index;
		});

	const OctaneGUI::Vector2 Overflow { ListBox->Scrollable()->Overflow() };
	ListBox->Scrollable()->SetOffset(Overflow);
	Application.Update();

	const OctaneGUI::Vector2 Position(ListBox->GetSize().X - 5.0f, 5.0f);
	Utility::MouseClick(Application, Position);

	VERIFYF(Selected == 0, "Selected index '%d' is not '0'.", Selected);

	const OctaneGUI::Vector2 Offset { ListBox->Scrollable()->Offset() };
	VERIFYF(std::ceil(Offset.X) == std::ceil(Overflow.X), "The scroll offset '%.2f' does not equal the overflow '%.2f'.", std::ceil(Offset.X), std::ceil(Overflow.X));

	return true;
})

TEST_CASE(VScrollSelection,
{
	OctaneGUI::ControlList List;
	Load(Application, {}, List);

	const std::shared_ptr<OctaneGUI::ListBox> ListBox = List.To<OctaneGUI::ListBox>("ListBox");
	for (int I = 0; I < 40; I++)
	{
		const std::string String = std::string("Item ") + std::to_string(I);
		ListBox->AddItem<OctaneGUI::TextSelectable>()->SetText(String.c_str());
	}
	Application.Update();

	int Selected = -1;
	ListBox->SetOnSelect([&](int Index, std::weak_ptr<OctaneGUI::Control> Item) -> void
		{
			Selected = Index;
		});
	
	const OctaneGUI::Vector2 Overflow { ListBox->Scrollable()->Overflow() };
	ListBox->Scrollable()->SetOffset(Overflow);
	Application.Update();

	const std::shared_ptr<OctaneGUI::Control>& Item = ListBox->Item(0);
	const float Height { Item->GetSize().Y };
	const int Expected { (int)(Overflow.Y / Height) };
	Utility::MouseClick(Application, { 5.0f, 5.0f });

	VERIFYF(Selected == Expected, "Selected index '%d' is not equal to expected index '%d'!", Selected, Expected);

	const OctaneGUI::Vector2 Offset { ListBox->Scrollable()->Offset() };
	VERIFYF(std::ceil(Offset.Y) == std::ceil(Overflow.Y), "The scroll offset '%.2f' does not meet the overflow '%.2f'!", std::ceil(Offset.Y), std::ceil(Overflow.Y));

	return true;
})

)

}
