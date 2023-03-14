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

#pragma once

#include <string>

namespace OctaneGUI
{
class Application;
class ControlList;
}

namespace Tests
{
namespace Utility
{

void Load(OctaneGUI::Application& Application, const char* MenuBarJson, const char* BodyJson, OctaneGUI::ControlList& List, int WindowWidth, int WindowHeight);
void Load(OctaneGUI::Application& Application, const char* BodyJson, OctaneGUI::ControlList& List);
void Load(OctaneGUI::Application& Application, const char* MenuBarJson, const char* BodyJson, OctaneGUI::ControlList& List);
void KeyEvent(OctaneGUI::Application& Application, OctaneGUI::Keyboard::Key Key);
void MouseMove(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position);
void MousePress(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button = OctaneGUI::Mouse::Button::Left);
void MouseRelease(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button = OctaneGUI::Mouse::Button::Left);
void MouseClick(OctaneGUI::Application& Application, const OctaneGUI::Vector2& Position, OctaneGUI::Mouse::Button Button = OctaneGUI::Mouse::Button::Left);
void TextEvent(OctaneGUI::Application& Application, const std::u32string& Text);

}
}
