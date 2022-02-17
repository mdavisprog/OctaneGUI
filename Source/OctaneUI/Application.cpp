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

#include "Application.h"
#include "Controls/Container.h"
#include "Controls/ControlList.h"
#include "Event.h"
#include "Font.h"
#include "Icons.h"
#include "Json.h"
#include "Paint.h"
#include "Texture.h"
#include "Theme.h"
#include "Window.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <thread>

namespace OctaneUI
{

Application::Application()
{
	Texture::SetOnLoad([this](const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height) -> uint32_t
	{
		if (m_OnLoadTexture)
		{
			return m_OnLoadTexture(Data, Width, Height);
		}

		return 0;
	});
}

Application::~Application()
{
}

bool Application::Initialize(const char* Title)
{
	if (!Initialize())
	{
		Shutdown();
		return false;
	}

	NewWindow(Title, 1280.0f, 720.0f);

	return true;
}

bool Application::Initialize(const char* JsonStream, std::unordered_map<std::string, ControlList>& WindowControls)
{
	if (!Initialize())
	{
		Shutdown();
		return false;
	}

	const Json Root = Json::Parse(JsonStream);

	if (Root.IsNull())
	{
		Shutdown();
		return false;
	}

	const Json& Windows = Root["Windows"];
	if (!Windows.IsArray())
	{
		Shutdown();
		return false;
	}

	const char* FontPath = Root["Font"].String();
	float FontSize = Root["FontSize"].Number(16.0f);
	LoadFont(FontPath, FontSize);

	for (int I = 0; I < Windows.Count(); I++)
	{
		ControlList List;
		std::shared_ptr<Window> Result = CreateWindow(Windows[I], List);
		assert(Result->HasID());

		WindowControls[Result->ID()] = List;
	}

	assert(WindowControls.find("Main") != WindowControls.end());

	return true;
}

void Application::Shutdown()
{
	if (m_OnDestroyWindow)
	{
		for (const std::shared_ptr<Window>& Item : m_Windows)
		{
			m_OnDestroyWindow(Item.get());
		}
	}

	m_Windows.clear();
	m_Theme = nullptr;
	m_Icons = nullptr;
	m_IsRunning = false;

	if (m_OnExit)
	{
		m_OnExit();
	}
}

void Application::Update()
{
	for (const std::shared_ptr<Window>& Item : m_Windows)
	{
		Item->Update();
	}

	for (const std::shared_ptr<Window>& Item : m_Windows)
	{
		Paint Brush(m_Theme);
		Item->DoPaint(Brush);
	}
}

int Application::Run()
{
	if (m_OnEvent)
	{
		while (m_IsRunning)
		{
			Update();

			for (const std::shared_ptr<Window>& Item : m_Windows)
			{
				ProcessEvent(Item);

				if (!m_IsRunning)
				{
					break;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	Shutdown();
	return 0;
}

void Application::Quit()
{
	m_IsRunning = false;
}

std::shared_ptr<Font> Application::LoadFont(const char* Path, float Size)
{
	std::shared_ptr<Font> Result = std::make_shared<Font>();

	bool Success = Result->Load(Path, Size);
	if (Success)
	{
		if (m_Theme)
		{
			m_Theme->SetFont(Result);
		}
	}
	else
	{
		Result = nullptr;
	}

	return Result;
}

std::shared_ptr<Window> Application::GetMainWindow() const
{
	if (m_Windows.size() == 0)
	{
		return nullptr;
	}

	return m_Windows[0];
}

std::shared_ptr<Window> Application::NewWindow(const char* Title, float Width, float Height)
{
	Json Root;
	Root["Title"] = Title;
	Root["Width"] = Width;
	Root["Height"] = Height;

	ControlList List;
	return CreateWindow(Root, List);
}

std::shared_ptr<Window> Application::NewWindow(const char* JsonStream)
{
	ControlList List;
	return CreateWindow(Json::Parse(JsonStream), List);
}

std::shared_ptr<Window> Application::NewWindow(const char* JsonStream, ControlList& List)
{
	return CreateWindow(Json::Parse(JsonStream), List);
}

std::shared_ptr<Theme> Application::GetTheme() const
{
	return m_Theme;
}

std::shared_ptr<Icons> Application::GetIcons() const
{
	return m_Icons;
}

bool Application::IsKeyPressed(Keyboard::Key Key) const
{
	return std::find(m_PressedKeys.begin(), m_PressedKeys.end(), Key) != m_PressedKeys.end();
}

Application& Application::SetOnCreateWindow(OnWindowSignature Fn)
{
	m_OnCreateWindow = Fn;
	return *this;
}

Application& Application::SetOnDestroyWindow(OnWindowSignature Fn)
{
	m_OnDestroyWindow = Fn;
	return *this;
}

Application& Application::SetOnPaint(OnWindowPaintSignature Fn)
{
	m_OnPaint = Fn;
	return *this;
}

Application& Application::SetOnEvent(OnWindowEventSignature Fn)
{
	m_OnEvent = Fn;
	return *this;
}

Application& Application::SetOnLoadTexture(OnLoadTextureSignature Fn)
{
	m_OnLoadTexture = Fn;
	return *this;
}

Application& Application::SetOnExit(OnEmptySignature Fn)
{
	m_OnExit = Fn;
	return *this;
}

void Application::OnPaint(Window* InWindow, const VertexBuffer& Buffers)
{
	if (m_OnPaint)
	{
		m_OnPaint(InWindow, Buffers);
	}
}

std::shared_ptr<Window> Application::CreateWindow(const Json& Root, ControlList& List)
{
	std::shared_ptr<Window> Result = std::make_shared<Window>(this);
	Result->CreateContainer();
	Result->Load(Root, List);
	Result->SetOnPaint(std::bind(&Application::OnPaint, this, std::placeholders::_1, std::placeholders::_2));
	m_Windows.push_back(Result);

	if (m_OnCreateWindow)
	{
		m_OnCreateWindow(Result.get());
	}

	return Result;
}

void Application::DestroyWindow(const std::shared_ptr<Window>& Item)
{
	if (!Item)
	{
		return;
	}

	auto Iter = std::find(m_Windows.begin(), m_Windows.end(), Item);
	if (Iter == m_Windows.end())
	{
		return;
	}

	if (m_OnDestroyWindow)
	{
		m_OnDestroyWindow(Item.get());
	}

	m_Windows.erase(Iter);
}

void Application::ProcessEvent(const std::shared_ptr<Window>& Item)
{
	if (!Item)
	{
		return;
	}

	Event E = m_OnEvent(Item.get());

	switch (E.GetType())
	{
	case Event::Type::WindowClosed:
		if (Item == m_Windows[0])
		{
			m_IsRunning = false;
		}
		else
		{
			DestroyWindow(Item);
		}
		break;
	
	case Event::Type::KeyPressed:
		{
			Keyboard::Key Code = E.GetData().m_Key.m_Code;
			Item->OnKeyPressed(Code);
			if (std::find(m_PressedKeys.begin(), m_PressedKeys.end(), Code) == m_PressedKeys.end())
			{
				m_PressedKeys.push_back(Code);
			}
		} break;
	
	case Event::Type::KeyReleased:
		{
			Keyboard::Key Code = E.GetData().m_Key.m_Code;
			Item->OnKeyReleased(Code);
			std::vector<Keyboard::Key>::iterator Iter = std::remove(m_PressedKeys.begin(), m_PressedKeys.end(), Code);
			if (Iter != m_PressedKeys.end())
			{
				m_PressedKeys.erase(Iter);
			}
		} break;

	case Event::Type::MouseMoved:
		Item->OnMouseMove(E.GetData().m_MouseMove.m_Position);
		break;
	
	case Event::Type::MousePressed:
		Item->OnMousePressed(E.GetData().m_MouseButton.m_Position, E.GetData().m_MouseButton.m_Button);
		break;
	
	case Event::Type::MouseReleased:
		Item->OnMouseReleased(E.GetData().m_MouseButton.m_Position, E.GetData().m_MouseButton.m_Button);
		break;
	
	case Event::Type::WindowResized:
		Item->SetSize(E.GetData().m_Resized.m_Size);
		break;
	
	case Event::Type::Text:
		Item->OnText(E.GetData().m_Text.Code);
		break;
	
	case Event::Type::None:
	default: break;
	}
}

bool Application::Initialize()
{
	if (m_IsRunning)
	{
		return true;
	}

	m_Theme = std::make_shared<Theme>();

	m_Icons = std::make_shared<Icons>();
	m_Icons->Initialize();

	m_IsRunning = true;

	return true;
}

}
