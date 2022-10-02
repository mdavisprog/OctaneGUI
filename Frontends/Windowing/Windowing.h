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

#pragma once

#include "OctaneGUI/Dialogs/FileDialogType.h"
#include "OctaneGUI/Mouse.h"

#include <string>
#include <vector>

namespace OctaneGUI
{

class Event;
struct FileDialogFilter;
struct Vector2;
class Window;

}

namespace Frontend
{
namespace Windowing
{

enum class HitTestResult
{
    Normal,
    Draggable,
    TopLeft,
    Top,
    TopRight,
    Right,
    BottomRight,
    Bottom,
    BottomLeft,
    Left,
};

typedef HitTestResult (*OnHitTestSignature)(void*, const OctaneGUI::Vector2&);

bool Initialize();
void NewWindow(OctaneGUI::Window* Window);
void DestroyWindow(OctaneGUI::Window* Window);
void RaiseWindow(OctaneGUI::Window* Window);
void MinimizeWindow(OctaneGUI::Window* Window);
void MaximizeWindow(OctaneGUI::Window* Window);
void ToggleWindow(OctaneGUI::Window* Window, bool Enable);
void NewFrame();
OctaneGUI::Event Event(OctaneGUI::Window* Window);
void Exit();
void SetClipboardContents(const std::u32string& Contents);
std::u32string GetClipboardContents();
void SetWindowTitle(OctaneGUI::Window* Window, const char* Title);
void SetWindowPosition(OctaneGUI::Window* Window);
void SetMouseCursor(OctaneGUI::Window* Window, OctaneGUI::Mouse::Cursor Cursor);

void SetAlwaysOnTop(void* Handle);
void ShowMinimize(void* Handle, bool Show);
void Minimize(void* Handle);
void SetOnHitTest(void* Handle, OnHitTestSignature&& Fn);
void Focus(void* Handle);
std::u32string FileDialog(OctaneGUI::FileDialogType Type, const std::vector<OctaneGUI::FileDialogFilter>& Filters, void* Handle);

}
}
