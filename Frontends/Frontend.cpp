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

void OnCreateWindow(OctaneGUI::Window* Window)
{
	Windowing::CreateWindow(Window);
}

void OnDestroyWindow(OctaneGUI::Window* Window)
{
	Windowing::DestroyWindow(Window);
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

std::u32string OnGetClipboardContents()
{
	return Windowing::GetClipboardContents();
}

void OnSetWindowTitle(OctaneGUI::Window* Window, const char* Title)
{
	Windowing::SetWindowTitle(Window, Title);
}

void Initialize(OctaneGUI::Application& Application)
{
	Application
		.SetOnCreateWindow(OnCreateWindow)
		.SetOnDestroyWindow(OnDestroyWindow)
		.SetOnEvent(OnEvent)
		.SetOnPaint(OnPaint)
		.SetOnLoadTexture(OnLoadTexture)
		.SetOnExit(OnExit)
		.SetOnGetClipboardContents(OnGetClipboardContents)
		.SetOnSetWindowTitle(OnSetWindowTitle);
}

}
