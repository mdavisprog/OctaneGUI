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

#include "Popup.h"
#include "Controls/Container.h"
#include "Paint.h"

namespace OctaneGUI
{

Popup::Popup()
	: m_Container(nullptr)
	, m_Modal(false)
	, m_State(State::None)
	, m_OnInvalidate(nullptr)
	, m_OnClose(nullptr)
{
}

Popup::~Popup()
{
}

void Popup::Open(const std::shared_ptr<Container>& InContainer, bool Modal)
{
	if (m_Container != InContainer)
	{
		Close();

		m_Container = InContainer;
		m_Modal = Modal;

		if (m_Container)
		{
			m_Container->SetOnInvalidate([this](Control* Focus, InvalidateType Type) -> void
				{
					if (m_OnInvalidate)
					{
						m_OnInvalidate(Focus, Type);
					}
				});

			m_State = State::Opening;
		}
	}
}

void Popup::Close()
{
	if (m_State == State::Opening || !m_Container)
	{
		return;
	}

	if (m_Container)
	{
		m_Container->SetOnInvalidate(nullptr);

		if (m_OnClose)
		{
			m_OnClose(*m_Container.get());
		}
	}

	m_Container = nullptr;
	m_Modal = false;
	m_State = State::None;
}

void Popup::Update()
{
	if (m_Container)
	{
		if (m_State == State::Opening)
		{
			m_State = State::Opened;
		}
	}
}

bool Popup::IsModal() const
{
	return m_Modal;
}

const std::shared_ptr<Container>& Popup::GetContainer() const
{
	return m_Container;
}

bool Popup::HasControl(const std::shared_ptr<Control>& Item) const
{
	if (!m_Container)
	{
		return false;
	}

	return m_Container->HasControlRecurse(Item);
}

void Popup::OnMouseMove(const Vector2& Position)
{
	if (m_Container)
	{
		m_Container->OnMouseMove(Position);
	}
}

std::weak_ptr<Control> Popup::GetControl(const Vector2& Position) const
{
	std::weak_ptr<Control> Result;

	if (m_Container)
	{
		Result = m_Container->GetControl(Position);
	}

	return Result;
}

void Popup::OnPaint(Paint& Brush)
{
	if (m_Container)
	{
		m_Container->OnPaint(Brush);
	}
}

void Popup::SetOnInvalidate(OnInvalidateSignature Fn)
{
	m_OnInvalidate = Fn;
}

void Popup::SetOnClose(OnContainerSignature Fn)
{
	m_OnClose = Fn;
}

}
