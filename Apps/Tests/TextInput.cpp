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

TEST_SUITE(TextInput,

TEST_CASE(SingleLine_NoScrollBars,
{
	OctaneGUI::ControlList List;
	Utility::Load(
		Application,
		"{\"Type\": \"TextInput\", \"ID\": \"TextInput\", \"Text\": {\"Text\": \"Well Hello Friends! Welcome to the program!\"}}",
		List);
	
	const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

	return !TextInput->Scrollable()->HorizontalScrollBar()->ShouldPaint() && !TextInput->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(MultiLine_ScrollBars,
{
	OctaneGUI::ControlList List;
	Utility::Load(
		Application,
		"{\"Type\": \"TextInput\", \"ID\": \"TextInput\", \"Multiline\": true, \"Size\": [80, 30], \"Text\": {\"Text\": \"Well Hello Friends!\n Welcome to the program!\"}}",
		List);
	
	const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

	return TextInput->Scrollable()->HorizontalScrollBar()->ShouldPaint() && TextInput->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

)

}
