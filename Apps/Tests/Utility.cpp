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

namespace Tests
{
namespace Utility
{

void Load(OctaneGUI::Application& Application, const char* Json, OctaneGUI::ControlList& List)
{
	static const char* Base = "{\"Width\": 1280, \"Height\": 720, \"Body\": {\"Controls\": [";
	std::string Stream = Base;
	Stream += Json;
	Stream += "]}}";
	Application.GetMainWindow()->Load(Stream.c_str(), List);
	Application.GetMainWindow()->Update();
}

void MouseMove(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position)
{
	Application.GetMainWindow()->OnMouseMove(Position);
}

void MouseClick(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button)
{
	MouseMove(Application, Position);
	Application.GetMainWindow()->OnMousePressed(Position, Button);
	Application.GetMainWindow()->OnMouseReleased(Position, Button);
}

}
}
