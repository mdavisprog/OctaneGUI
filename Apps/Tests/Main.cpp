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

uint32_t TextureID = 0;

void OnCreateWindow(OctaneGUI::Window* Window)
{
}

void OnDestroyWindow(OctaneGUI::Window* Window)
{
}

OctaneGUI::Event OnEvent(OctaneGUI::Window* Window)
{
	return OctaneGUI::Event(OctaneGUI::Event::Type::WindowClosed);
}

void OnPaint(OctaneGUI::Window* Window, const OctaneGUI::VertexBuffer& Buffers)
{
}

uint32_t OnLoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
	return ++TextureID;
}

void OnExit()
{
}

int main(int argc, char** argv)
{
	OctaneGUI::Application Application;
	Application
		.SetOnCreateWindow(OnCreateWindow)
		.SetOnDestroyWindow(OnDestroyWindow)
		.SetOnEvent(OnEvent)
		.SetOnPaint(OnPaint)
		.SetOnLoadTexture(OnLoadTexture)
		.SetOnExit(OnExit);
	
	const char* Json = 
	"{"
		"\"Theme\": {\"FontPath\": \"Roboto-Regular.ttf\", \"FontSize\": 18},"
		"\"Windows\": {\"Main\": {\"Title\": \"Unit Tests\", \"Width\": 1280, \"Height\": 720}}"
	"}";
	
	std::unordered_map<std::string, OctaneGUI::ControlList> WindowControls;
	if (!Application.Initialize(Json, WindowControls))
	{
		printf("Failed to initialize application.\n");
		return -1;
	}

	Tests::TestSuite::Run(Application, argc, argv);

	// This should return immediately with stubbed functions.
	return Application.Run();
}
