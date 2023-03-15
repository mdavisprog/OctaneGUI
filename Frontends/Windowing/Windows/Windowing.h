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

namespace OctaneGUI
{

struct Rect;

}

namespace Frontend
{
namespace Windowing
{
namespace Windows
{

typedef void (*OnWindowMoveSignature)(void*, const OctaneGUI::Vector2&);
typedef HitTestResult (*OnHitTestSignature)(void*, const OctaneGUI::Vector2&);

OctaneGUI::Rect GetWorkingArea(void* Handle);
OctaneGUI::Rect GetCaptionArea(void* Handle);
void FocusWindow(void* Handle);
void MinimizeWindow(void* Handle);
void MaximizeWindow(void* Handle);
void RestoreWindow(void* Handle);
void SetEnabled(void* Handle, bool Enabled);
void SetAlwaysOnTop(void* Handle);
void ShowMinimize(void* Handle, bool Show);
void SetOnHitTest(void* Handle, OnHitTestSignature&& Fn);
void SetOnWindowMove(void* Handle, OnWindowMoveSignature&& Fn);
std::u32string FileDialog(OctaneGUI::FileDialogType Type, const std::vector<OctaneGUI::FileDialogFilter>& Filters, void* Handle);

}
}
}
