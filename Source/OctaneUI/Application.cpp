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
#include "Icons.h"
#include "Json.h"
#include "Paint.h"
#include "Texture.h"
#include "Theme.h"
#include "Window.h"

#include <algorithm>
#undef NDEBUG
#include <cassert>
#include <chrono>
#include <sstream>
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

	m_Theme->Load(Root["Theme"]);

	const Json& Windows = Root["Windows"];
	if (!Windows.IsObject())
	{
		Shutdown();
		return false;
	}

	Windows.ForEach([&](const std::string& Key, const Json& Value) -> void
	{
		ControlList List;
		CreateWindow(Key.c_str(), Value, List);
		WindowControls[Key] = List;
	});

	assert(m_Windows.find("Main") != m_Windows.end());
	DisplayWindow("Main");

	return true;
}

void Application::Shutdown()
{
	if (m_OnDestroyWindow)
	{
		for (auto& Item : m_Windows)
		{
			m_OnDestroyWindow(Item.second.get());
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
	for (auto& Item : m_Windows)
	{
		if (Item.second->IsVisible())
		{
			Item.second->Update();
		}
	}

	for (auto& Item : m_Windows)
	{
		if (Item.second->IsVisible())
		{
			Paint Brush(m_Theme);
			Item.second->DoPaint(Brush);
		}
	}

	for (auto& Item : m_Windows)
	{
		if (Item.second->ShouldClose())
		{
			DestroyWindow(Item.second);
		}
	}
}

int Application::Run()
{
	if (m_OnEvent)
	{
		while (m_IsRunning)
		{
			Update();

			for (auto& Item : m_Windows)
			{
				if (Item.second->IsVisible())
				{
					ProcessEvent(Item.second);

					if (!m_IsRunning)
					{
						break;
					}
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

std::shared_ptr<Window> Application::GetMainWindow() const
{
	if (m_Windows.size() == 0)
	{
		return nullptr;
	}

	return m_Windows.at("Main");
}

std::shared_ptr<Window> Application::NewWindow(const char* ID, const char* JsonStream)
{
	ControlList List;
	return CreateWindow(ID, Json::Parse(JsonStream), List);
}

std::shared_ptr<Window> Application::NewWindow(const char* ID, const char* JsonStream, ControlList& List)
{
	return CreateWindow(ID, Json::Parse(JsonStream), List);
}

bool Application::DisplayWindow(const char* ID) const
{
	auto It = m_Windows.find(ID);
	if (It == m_Windows.end())
	{
		return false;
	}

	if (It->second->IsVisible())
	{
		return true;
	}

	if (m_OnCreateWindow != nullptr)
	{
		m_OnCreateWindow(It->second.get());
	}

	It->second->SetVisible(true);

	return true;
}

std::shared_ptr<Theme> Application::GetTheme() const
{
	return m_Theme;
}

std::shared_ptr<Icons> Application::GetIcons() const
{
	return m_Icons;
}

TextureCache& Application::GetTextureCache()
{
	return m_TextureCache;
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

std::shared_ptr<Window> Application::CreateWindow(const char* ID, const Json& Root, ControlList& List)
{
	std::shared_ptr<Window> Result = std::make_shared<Window>(this);
	Result->CreateContainer();
	Result->Load(Root, List);
	Result->SetOnPaint(std::bind(&Application::OnPaint, this, std::placeholders::_1, std::placeholders::_2));
	Result->SetID(ID);
	m_Windows[ID] = Result;
	return Result;
}

void Application::DestroyWindow(const std::shared_ptr<Window>& Item)
{
	if (!Item || !Item->IsVisible())
	{
		return;
	}

	if (m_OnDestroyWindow)
	{
		m_OnDestroyWindow(Item.get());
	}

	Item->SetVisible(false);
	Item->RequestClose(false);

	if (std::string(Item->ID()) == "Main")
	{
		m_IsRunning = false;
	}
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
		DestroyWindow(Item);
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
	m_Theme->SetOnThemeLoaded([this]() -> void
	{
		for (const std::pair<std::string, std::shared_ptr<Window>>& Item : m_Windows)
		{
			Item.second->ThemeLoaded();
			if (Item.second->IsVisible())
			{
				Item.second->GetContainer()->Invalidate();
			}
		}
	});

	m_Icons = std::make_shared<Icons>();
	m_Icons->Initialize();

	m_IsRunning = true;

	return true;
}

}
