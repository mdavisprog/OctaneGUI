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

void Load(OctaneGUI::Application& Application, const char* Json, OctaneGUI::ControlList& List, int Width = 1280, int Height = 720)
{
	std::string Stream = "{\"ID\": \"View\", \"Type\": \"ScrollableViewControl\", \"Expand\": \"Both\", \"Controls\": [";
	Stream += Json;
	Stream += "]}";
	Utility::Load(Application, Stream.c_str(), List, Width, Height);
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
	Load(Application, "{\"Type\": \"Container\", \"Size\": [220, 40]}", List, 200, 200);
	const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
	return View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && !View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(VScrollBar,
{
	OctaneGUI::ControlList List;
	Load(Application, "{\"Type\": \"Container\", \"Size\": [40, 240]}", List, 200, 200);
	const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
	return !View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(HAndVScrollBar,
{
	OctaneGUI::ControlList List;
	Load(Application, "{\"Type\": \"Container\", \"Size\": [240, 240]}", List, 200, 200);
	const std::shared_ptr<OctaneGUI::ScrollableViewControl> View = List.To<OctaneGUI::ScrollableViewControl>("View");
	return View->Scrollable()->HorizontalScrollBar()->ShouldPaint() && View->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

)

}
