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
#include "Controls/ControlList.h"
#include "Controls/MenuBar.h"
#include "Controls/MenuItem.h"
#include "Controls/VerticalContainer.h"
#include "Json.h"
#include "Paint.h"
#include "Window.h"

namespace OctaneUI
{

Window::Window(Application* InApplication)
	: m_Application(InApplication)
{
	m_Popup.SetOnInvalidate([=](Control* Focus, InvalidateType Type) -> void
	{
		m_Repaint = true;
	});

	m_Popup.SetOnClose([=](Container* Focus) -> void
	{
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

void Window::SetPopup(const std::shared_ptr<Container>& Popup, bool Modal)
{
	m_Popup.Open(Popup, Modal);
}

void Window::OnKeyPressed(Keyboard::Key Key)
{
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
		Hovered = m_Container->GetControl(Position);
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
	std::shared_ptr<Control> Focused = m_Focus.lock();
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

	if (New != Focused)
	{
		if (Focused)
		{
			Focused->OnUnfocused();
		}

		m_Focus = New;

		if (New)
		{
			New->OnFocused();
		}
	}

	m_Popup.Close();
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

void Window::CreateContainer()
{
	m_Container = std::make_shared<VerticalContainer>(this);
	m_Container
		->SetSpacing({0.0f, 0.0f})
		->SetExpand(Expand::Both)
		->SetOnInvalidate([=](Control* Focus, InvalidateType Type) -> void
		{
			m_Repaint = true;
		});

	m_MenuBar = std::make_shared<MenuBar>(this);
	m_Container->InsertControl(m_MenuBar);
	
	m_Body = m_Container->AddControl<Container>();
	m_Body->SetExpand(Expand::Both);

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

TextureCache& Window::TextureCache() const
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
	if (m_Container->ShouldUpdateLayout())
	{
		m_Container->Layout();
	}

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
	SetSize({Width, Height});

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

}
