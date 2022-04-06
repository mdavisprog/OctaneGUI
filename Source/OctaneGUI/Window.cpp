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

#include "Window.h"
#include "Application.h"
#include "Controls/ControlList.h"
#include "Controls/MenuBar.h"
#include "Controls/MenuItem.h"
#include "Controls/VerticalContainer.h"
#include "Json.h"
#include "Paint.h"
#include "Timer.h"

#if TOOLS
	#include "Tools/CommandPalette.h"
#endif

#include <algorithm>

namespace OctaneGUI
{

Window::Window(Application* InApplication)
	: m_Application(InApplication)
{
	m_Popup.SetOnInvalidate([=](Control* Focus, InvalidateType Type) -> void
		{
			if ((Type == InvalidateType::Layout || Type == InvalidateType::Both))
			{
				RequestLayout(dynamic_cast<Container*>(Focus));
			}

			m_Repaint = true;
		});

	m_Popup.SetOnClose([=](const Container& Focus) -> void
		{
			if (m_OnPopupClose)
			{
				m_OnPopupClose(Focus);
				m_OnPopupClose = nullptr;
			}

			m_MenuBar->Close();
			m_Repaint = true;
		});
}

Window::~Window()
{
	m_Container = nullptr;
}

void Window::SetTitle(const char* Title)
{
	m_Title = Title;
}

const char* Window::GetTitle() const
{
	return m_Title.c_str();
}

void Window::SetSize(Vector2 Size)
{
	m_Bounds.Max = m_Bounds.Min + Size;
	m_Container->SetSize(m_Bounds.GetSize());
	m_Container->InvalidateLayout();
}

Vector2 Window::GetSize() const
{
	return m_Bounds.Max - m_Bounds.Min;
}

void Window::SetID(const char* ID)
{
	m_ID = ID;
}

const char* Window::ID() const
{
	return m_ID.c_str();
}

bool Window::HasID() const
{
	return !m_ID.empty();
}

void Window::SetVisible(bool Visible)
{
	m_Visible = Visible;
	m_Repaint = Visible;
}

bool Window::IsVisible() const
{
	return m_Visible;
}

void Window::RequestClose(bool Request)
{
	m_RequestClose = Request;
}

bool Window::ShouldClose() const
{
	return m_RequestClose;
}

void Window::SetPopup(const std::shared_ptr<Container>& Popup, OnContainerSignature Callback, bool Modal)
{
	m_Popup.Open(Popup, Modal);
	m_OnPopupClose = Callback;
	RequestLayout(Popup.get());
}

void Window::ClosePopup()
{
	m_Popup.Close();
}

const std::shared_ptr<Container>& Window::GetPopup() const
{
	return m_Popup.GetContainer();
}

void Window::OnKeyPressed(Keyboard::Key Key)
{
#if TOOLS
	if (Key == Keyboard::Key::P && (IsKeyPressed(Keyboard::Key::LeftControl) || IsKeyPressed(Keyboard::Key::RightControl)))
	{
		if (m_Popup.GetContainer() != m_CommandPalette)
		{
			m_CommandPalette->Show();
			SetPopup(m_CommandPalette);
			UpdateFocus(m_CommandPalette->Input());
			m_Repaint = true;
		}
		return;
	}
#endif

	if (m_Focus.expired() || Key == Keyboard::Key::None)
	{
		return;
	}

	std::shared_ptr<Control> Focused = m_Focus.lock();
	Focused->OnKeyPressed(Key);
}

void Window::OnKeyReleased(Keyboard::Key Key)
{
	if (m_Focus.expired() || Key == Keyboard::Key::None)
	{
		return;
	}

	std::shared_ptr<Control> Focused = m_Focus.lock();
	Focused->OnKeyReleased(Key);
}

void Window::OnMouseMove(const Vector2& Position)
{
	m_MousePosition = Position;

	std::weak_ptr<Control> Hovered = m_Popup.GetControl(Position);

	if (!m_Popup.IsModal() && Hovered.expired())
	{
		Hovered = m_MenuBar->GetControl(Position);
		Hovered = Hovered.expired() ? m_Body->GetControl(Position) : Hovered;
	}

	std::shared_ptr<Control> Current = Hovered.lock();
	std::shared_ptr<Control> Previous = m_Hovered.lock();
	if (Current != Previous)
	{
		if (Previous)
		{
			Previous->OnMouseLeave();
		}

		m_Hovered = Hovered;

		if (Current)
		{
			Current->OnMouseEnter();
		}
	}

	if (Current)
	{
		Current->OnMouseMove(Position);
	}

	if (!m_Focus.expired())
	{
		std::shared_ptr<Control> Focused = m_Focus.lock();
		Focused->OnMouseMove(Position);
	}
}

void Window::OnMousePressed(const Vector2& Position, Mouse::Button MouseButton)
{
	std::shared_ptr<Control> New = nullptr;
	if (!m_Hovered.expired())
	{
		std::shared_ptr<Control> Hovered = m_Hovered.lock();
		bool Pressed = Hovered->OnMousePressed(Position, MouseButton);

		if (Pressed)
		{
			New = Hovered;
		}
	}

	UpdateFocus(New);
}

void Window::OnMouseReleased(const Vector2& Position, Mouse::Button MouseButton)
{
	std::shared_ptr<Control> Hovered;
	std::shared_ptr<Control> Focused;

	if (!m_Hovered.expired())
	{
		Hovered = m_Hovered.lock();
	}

	if (!m_Focus.expired())
	{
		Focused = m_Focus.lock();
	}

	if (Hovered && Hovered != Focused)
	{
		Hovered->OnMouseReleased(Position, MouseButton);
	}

	if (Focused)
	{
		Focused->OnMouseReleased(Position, MouseButton);
	}
}

void Window::OnText(uint32_t Code)
{
	if (m_Focus.expired())
	{
		return;
	}

	std::shared_ptr<Control> Focused = m_Focus.lock();
	Focused->OnText(Code);
}

void Window::ThemeLoaded()
{
	m_Container->OnThemeLoaded();
}

void Window::CreateContainer()
{
	m_Container = std::make_shared<VerticalContainer>(this);
	m_Container
		->SetSpacing({ 0.0f, 0.0f })
		->SetExpand(Expand::Both)
		->SetOnInvalidate([=](Control* Focus, InvalidateType Type) -> void
			{
				if ((Type == InvalidateType::Layout || Type == InvalidateType::Both))
				{
					RequestLayout(dynamic_cast<Container*>(Focus));
				}

				m_Repaint = true;
			});

	m_MenuBar = std::make_shared<MenuBar>(this);
	m_Container->InsertControl(m_MenuBar);

	m_Body = m_Container->AddControl<Container>();
	m_Body->SetExpand(Expand::Both);

#if TOOLS
	if (m_Application->IsMainWindow(this))
	{
		m_CommandPalette = std::make_shared<Tools::CommandPalette>(this);
	}
#endif

	m_Repaint = true;
}

std::shared_ptr<Container> Window::GetContainer() const
{
	return m_Body;
}

std::shared_ptr<MenuBar> Window::GetMenuBar() const
{
	return m_MenuBar;
}

std::shared_ptr<Theme> Window::GetTheme() const
{
	return m_Application->GetTheme();
}

std::shared_ptr<Icons> Window::GetIcons() const
{
	return m_Application->GetIcons();
}

TextureCache& Window::GetTextureCache() const
{
	return m_Application->GetTextureCache();
}

Vector2 Window::GetMousePosition() const
{
	return m_MousePosition;
}

bool Window::IsKeyPressed(Keyboard::Key Key) const
{
	return m_Application->IsKeyPressed(Key);
}

void Window::Update()
{
	UpdateTimers();

	for (Container* Item : m_LayoutRequests)
	{
		Item->Layout();
	}
	m_LayoutRequests.clear();

	m_Popup.Update();
}

void Window::DoPaint(Paint& Brush)
{
	if (m_Repaint)
	{
		m_Container->OnPaint(Brush);
		m_Popup.OnPaint(Brush);
		m_Repaint = false;
		m_OnPaint(this, Brush.GetBuffer());
	}
}

void Window::Load(const char* JsonStream)
{
	Load(Json::Parse(JsonStream));
}

void Window::Load(const char* JsonStream, ControlList& List)
{
	Load(Json::Parse(JsonStream));
	Populate(List);
}

void Window::Load(const Json& Root)
{
	const std::string Title = Root["Title"].String();
	float Width = Root["Width"].Number(640.0f);
	float Height = Root["Height"].Number(480.0f);

	const Json& MB = Root["MenuBar"];
	const Json& Body = Root["Body"];

	SetTitle(Title.c_str());
	SetSize({ Width, Height });

	m_MenuBar->OnLoad(MB);
	m_Body->OnLoad(Body);

	// Need to update the expansion type after OnLoad functions as it will be reset there.
	m_Body->SetExpand(Expand::Both);
}

void Window::Load(const Json& Root, ControlList& List)
{
	Load(Root);
	Populate(List);
}

void Window::Clear()
{
	m_MenuBar->ClearControls();
	m_Body->ClearControls();
}

std::shared_ptr<Timer> Window::CreateTimer(int Interval, bool Repeat, OnEmptySignature&& Callback)
{
	std::shared_ptr Result = std::make_unique<Timer>(Interval, Repeat, this, std::move(Callback));
	return std::move(Result);
}

void Window::StartTimer(const std::shared_ptr<Timer>& Object)
{
	for (std::vector<TimerHandle>::iterator It = m_Timers.begin(); It != m_Timers.end();)
	{
		TimerHandle& Handle = *It;

		if (Handle.Object.expired())
		{
			It = m_Timers.erase(It);
		}
		else if (Handle.Object.lock() == Object)
		{
			Handle.Elapsed.Reset();
			return;
		}
		else
		{
			It++;
		}
	}

	m_Timers.emplace_back(Object);
}

bool Window::ClearTimer(const std::shared_ptr<Timer>& Object)
{
	for (std::vector<TimerHandle>::const_iterator It = m_Timers.begin(); It != m_Timers.end();)
	{
		const TimerHandle& Handle = *It;

		if (Handle.Object.expired() || Handle.Object.lock() == Object)
		{
			m_Timers.erase(It);
			return true;
		}
	}

	return false;
}

Window* Window::SetOnPaint(OnPaintSignature Fn)
{
	m_OnPaint = Fn;
	return this;
}

Window::Window()
{
}

void Window::Populate(ControlList& List) const
{
	std::vector<std::shared_ptr<MenuItem>> MenuItems;
	m_MenuBar->GetMenuItems(MenuItems);

	std::vector<std::shared_ptr<Control>> Controls;
	m_Body->GetControls(Controls);

	Controls.insert(Controls.end(), MenuItems.begin(), MenuItems.end());

	for (const std::shared_ptr<Control>& Item : Controls)
	{
		if (Item->HasID())
		{
			List.AddControl(Item);
		}
	}
}

void Window::RequestLayout(Container* Request)
{
	if (Request == nullptr)
	{
		return;
	}

	if (std::find(m_LayoutRequests.begin(), m_LayoutRequests.end(), Request) == m_LayoutRequests.end())
	{
		m_LayoutRequests.push_back(Request);
	}
}

void Window::UpdateTimers()
{
	for (std::vector<TimerHandle>::iterator It = m_Timers.begin(); It != m_Timers.end();)
	{
		TimerHandle& Handle = *It;

		if (Handle.Object.expired())
		{
			It = m_Timers.erase(It);
			continue;
		}

		std::shared_ptr<Timer> Object = Handle.Object.lock();
		if (Handle.Elapsed.MeasureMS() >= Object->Interval())
		{
			Object->Invoke();

			if (Object->Repeat())
			{
				Handle.Elapsed.Reset();
				It++;
			}
			else
			{
				It = m_Timers.erase(It);
			}
		}
		else
		{
			It++;
		}
	}
}

void Window::UpdateFocus(const std::shared_ptr<Control>& Focus)
{
	std::shared_ptr<Control> Focused = m_Focus.lock();

	bool ShouldClosePopup = false;
	if (Focus != Focused)
	{
		if (Focused)
		{
			Focused->OnUnfocused();
		}

		m_Focus = Focus;

		if (Focus)
		{
			ShouldClosePopup = !m_Popup.HasControl(Focus);
			Focus->OnFocused();
		}
		else
		{
			ShouldClosePopup = true;
		}
	}

	if (ShouldClosePopup)
	{
		m_Popup.Close();
	}
}

}
