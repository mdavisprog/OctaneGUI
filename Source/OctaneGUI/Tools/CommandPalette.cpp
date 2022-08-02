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
#include "../String.h"
#include "../Window.h"
#include "Inspector.h"
#include "ProfileViewer.h"
#include "Profiler.h"
#include "TextureViewer.h"

namespace OctaneGUI
{

namespace Tools
{

std::shared_ptr<CommandPalette> CommandPalette::Get(Window* InWindow)
{
    if (!s_Root)
    {
        s_Root = std::make_shared<CommandPalette>(InWindow);
    }
    else
    {
        s_Root->SetWindow(InWindow);
    }

    return s_Root;
}

CommandPalette::CommandPalette(Window* InWindow)
    : Container(InWindow)
{
    std::shared_ptr<Panel> Background = AddControl<Panel>();
    Background->SetExpand(Expand::Both);

    m_Container = AddControl<VerticalContainer>();
    m_Container->SetExpand(Expand::Both);

    std::shared_ptr<MarginContainer> Margins = m_Container->AddControl<MarginContainer>();
    Margins
        ->SetMargins({ 4.0f, 4.0f, 4.0f, 4.0f })
        .SetExpand(Expand::Both);

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
        std::vector<std::u32string> Tokens = Tokenize(m_Input->GetText());
        m_Input->SetText(U"");
        GetWindow()->ClosePopup();

        std::u32string Command;
        if (!Tokens.empty())
        {
            Command = Tokens[0];
            Tokens.erase(Tokens.begin());
        }
        return Process(Command, Tokens);
    }
    else if (Key == Keyboard::Key::Escape)
    {
        GetWindow()->ClosePopup();
        return true;
    }

    return false;
}

std::shared_ptr<CommandPalette> CommandPalette::s_Root { nullptr };

std::vector<std::u32string> CommandPalette::Tokenize(const std::u32string& Value)
{
    std::vector<std::u32string> Result;

    if (Value.empty())
    {
        return std::move(Result);
    }

    size_t Start = 0;
    size_t Pos = Value.find(' ');
    while (Pos != std::string::npos)
    {
        Result.push_back(std::move(Value.substr(Start, Pos - Start)));
        Start = Pos + 1;
        Pos = Value.find(' ', Start);
    }

    Result.push_back(std::move(Value.substr(Start, Value.size() - Start)));

    return std::move(Result);
}

bool CommandPalette::Process(const std::u32string& Command, const std::vector<std::u32string>& Arguments)
{
    std::u32string Lower = String::ToLower(Command);
    if (Lower == U"inspector")
    {
        Inspector::Get().Inspect(GetWindow());
        return true;
    }
    else if (Lower == U"profile")
    {
        if (!Arguments.empty())
        {
            Lower = String::ToLower(Arguments.front());

            if (Lower == U"enable" || Lower == U"e")
            {
                Profiler::Get().Enable();
            }
            else if (Lower == U"disable" || Lower == U"d")
            {
                if (Profiler::Get().IsEnabled())
                {
                    Profiler::Get().Disable();
                    ProfileViewer::Get().View(GetWindow());
                }
            }
            else
            {
                printf("No proifle command given. Must be 'enable' or 'disable'.\n");
            }
        }
    }
    else if (Lower == U"textureviewer" || Lower == U"tv")
    {
        TextureViewer::Get().Show(GetWindow()->App());
    }

    return false;
}

}

}
