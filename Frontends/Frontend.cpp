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

#include "Frontend.h"
#include "OctaneGUI/OctaneGUI.h"
#include "Rendering/Rendering.h"
#include "Windowing/Windowing.h"

namespace Frontend
{
void OnWindowAction(OctaneGUI::Window* Window, OctaneGUI::WindowAction Action)
{
    switch (Action)
    {
    case OctaneGUI::WindowAction::Create:
    {
        Windowing::NewWindow(Window);
        Rendering::CreateRenderer(Window);
    }
    break;

    case OctaneGUI::WindowAction::Destroy:
    {
        Rendering::DestroyRenderer(Window);
        Windowing::DestroyWindow(Window);
    }
    break;

    case OctaneGUI::WindowAction::Raise:
    {
        Windowing::RaiseWindow(Window);
    }
    break;

    case OctaneGUI::WindowAction::Enable:
    case OctaneGUI::WindowAction::Disable:
    {
        Windowing::ToggleWindow(Window, Action == OctaneGUI::WindowAction::Enable);
    }
    break;

    default: break;
    }
}

OctaneGUI::Event OnEvent(OctaneGUI::Window* Window)
{
    return Windowing::Event(Window);
}

void OnPaint(OctaneGUI::Window* Window, const OctaneGUI::VertexBuffer& Buffer)
{
    Rendering::Paint(Window, Buffer);
}

uint32_t OnLoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
    return Rendering::LoadTexture(Data, Width, Height);
}

void OnExit()
{
    Rendering::Exit();
    Windowing::Exit();
}

void OnSetClipboardContents(const std::u32string& Contents)
{
    Windowing::SetClipboardContents(Contents);
}

std::u32string OnGetClipboardContents()
{
    return Windowing::GetClipboardContents();
}

void OnSetWindowTitle(OctaneGUI::Window* Window, const char* Title)
{
    Windowing::SetWindowTitle(Window, Title);
}

void OnSetMouseCursor(OctaneGUI::Window* Window, OctaneGUI::Mouse::Cursor Cursor)
{
    Windowing::SetMouseCursor(Window, Cursor);
}

void Initialize(OctaneGUI::Application& Application)
{
    Windowing::Initialize();
    Rendering::Initialize();

    Application
        .SetOnWindowAction(OnWindowAction)
        .SetOnEvent(OnEvent)
        .SetOnPaint(OnPaint)
        .SetOnLoadTexture(OnLoadTexture)
        .SetOnExit(OnExit)
        .SetOnSetClipboardContents(OnSetClipboardContents)
        .SetOnGetClipboardContents(OnGetClipboardContents)
        .SetOnSetWindowTitle(OnSetWindowTitle)
        .SetOnSetMouseCursor(OnSetMouseCursor);
}

}
