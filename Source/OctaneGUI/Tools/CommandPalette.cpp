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

#include "CommandPalette.h"
#include "../Controls/MarginContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/TextInput.h"
#include "../Controls/VerticalContainer.h"
#include "../Json.h"
#include "../Window.h"
#include "Inspector.h"

namespace OctaneGUI
{

namespace Tools
{

CommandPalette::CommandPalette(Window* InWindow)
	: Container(InWindow)
{
	std::shared_ptr<Panel> Background = AddControl<Panel>();
	Background->SetExpand(Expand::Both);

	m_Container = AddControl<VerticalContainer>();
	m_Container
		->SetSpacing({ 0.0f, 0.0f })
		->SetExpand(Expand::Both);

	std::shared_ptr<MarginContainer> Margins = m_Container->AddControl<MarginContainer>();
	Margins->SetMargins({ 4.0f, 4.0f, 4.0f, 4.0f });

	m_Input = Margins->AddControl<TextInput>();
	m_Input->SetExpand(Expand::Width);
}

void CommandPalette::Show()
{
	const Vector2 ContentSize = m_Container->DesiredSize();
	const Vector2 WindowSize = GetWindow()->GetSize();
	const Vector2 Size = { WindowSize.X * 0.6f, ContentSize.Y };
	SetSize(Size);
	SetPosition({ WindowSize.X * 0.5f - Size.X * 0.5f, 30.0f });
}

std::shared_ptr<Control> CommandPalette::Input() const
{
	return m_Input->Interaction();
}

bool CommandPalette::OnKeyPressed(Keyboard::Key Key)
{
	if (Key == Keyboard::Key::Enter)
	{
		std::string Command = m_Input->GetText();
		m_Input->SetText("");
		GetWindow()->ClosePopup();
		return Process(Command);
	}
	else if (Key == Keyboard::Key::Escape)
	{
		GetWindow()->ClosePopup();
		return true;
	}

	return false;
}

bool CommandPalette::Process(const std::string& Command)
{
	const std::string Lower = Json::ToLower(Command);
	if (Lower == "inspector")
	{
		Inspector::Get().Inspect(GetWindow()->GetRootContainer());
		return true;
	}

	return false;
}

}

}
