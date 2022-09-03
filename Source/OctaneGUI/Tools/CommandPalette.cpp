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
#include "../Application.h"
#include "../Controls/MarginContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/Text.h"
#include "../Controls/TextInput.h"
#include "../Controls/VerticalContainer.h"
#include "../Dialogs/MessageBox.h"
#include "../Json.h"
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

    const std::shared_ptr<VerticalContainer> Items = Margins->AddControl<VerticalContainer>();
    Items
        ->SetSpacing({})
        ->SetExpand(Expand::Width);

    const std::shared_ptr<Text> Label = Items->AddControl<Text>();
    Label
        ->SetText(U"Command Palette")
        .SetFontSize(14);

    m_Input = Items->AddControl<TextInput>();
    m_Input->SetExpand(Expand::Width);
}

void CommandPalette::Show()
{
    const Vector2 ContentSize = m_Container->DesiredSize();
    const Vector2 WindowSize = GetWindow()->GetSize();
    const Vector2 Size = { WindowSize.X * 0.6f, ContentSize.Y };
    SetSize(Size);
    SetPosition({ WindowSize.X * 0.5f - Size.X * 0.5f, 30.0f });
    // The proper index will be set when the up key is pressed for the first time after a 'Show'.
    m_HistoryIndex = m_History.size();
}

std::shared_ptr<Control> CommandPalette::Input() const
{
    return m_Input->Interaction();
}

bool CommandPalette::OnKeyPressed(Keyboard::Key Key)
{
    if (Key == Keyboard::Key::Enter)
    {
        const std::u32string Buffer = m_Input->GetText();
        std::vector<std::u32string> Tokens = String::ParseArguments(Buffer);
        m_Input->SetText(U"");
        GetWindow()->ClosePopup();

        std::u32string Command;
        if (!Tokens.empty())
        {
            Command = Tokens[0];
            Tokens.erase(Tokens.begin());
        }

        if (Process(Command, Tokens))
        {
            if (m_History.empty() || m_History.back() != Buffer)
            {
                m_History.push_back(Buffer);
            }
            return true;
        }
    }
    else if (Key == Keyboard::Key::Escape)
    {
        GetWindow()->ClosePopup();
        return true;
    }
    else if (Key == Keyboard::Key::Up)
    {
        if (m_HistoryIndex > 0)
        {
            m_HistoryIndex--;
        }

        m_Input
            ->SetText(m_History[m_HistoryIndex].c_str())
            .SelectAll();
        return true;
    }
    else if (Key == Keyboard::Key::Down)
    {
        m_HistoryIndex = std::min<size_t>(m_HistoryIndex + 1, m_History.size());
        if (m_HistoryIndex < m_History.size())
        {
            m_Input
                ->SetText(m_History[m_HistoryIndex].c_str())
                .SelectAll();
        }
        else
        {
            m_Input->SetText(U"");
        }

        return true;
    }

    return false;
}

std::shared_ptr<CommandPalette> CommandPalette::s_Root { nullptr };

bool CommandPalette::Process(const std::u32string& Command, const std::vector<std::u32string>& Arguments)
{
    bool Result = true;

    std::u32string Lower = String::ToLower(Command);
    if (Lower == U"inspector")
    {
        Inspector::Get().Inspect(GetWindow());
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
    else if (Lower == U"dumpjson")
    {
        const std::shared_ptr<Container> Root = GetWindow()->GetRootContainer();

        Json Value { Json::Type::Object };
        Root->OnSave(Value);

        bool Pretty = false;
        for (const std::u32string& Arg : Arguments)
        {
            if (String::ToLower(Arg) == U"pretty")
            {
                Pretty = true;
            }
        }

        std::string JsonString;
        if (Pretty)
        {
            JsonString = Value.ToStringPretty();
        }
        else
        {
            JsonString = Value.ToString();
        }

        printf("%s\n", JsonString.c_str());

        GetWindow()->App().SetClipboardContents(String::ToUTF32(JsonString));
    }
    else if (Lower == U"messagebox" || Lower == U"mb")
    {
        // Need the title and message to display.
        if (Arguments.size() >= 2)
        {
            const std::u32string Title = Arguments[0];
            const std::u32string Message = Arguments[1];

            MessageBox::Buttons::Type Buttons = MessageBox::Buttons::Type::OKCancel;
            if (Arguments.size() >= 3)
            {
                const std::u32string ButtonTypes = String::ToLower(Arguments[2]);
                if (ButtonTypes == U"ok")
                {
                    Buttons = MessageBox::Buttons::Type::OK;
                }
                else if (ButtonTypes == U"yesno")
                {
                    Buttons = MessageBox::Buttons::Type::YesNo;
                }
                else if (ButtonTypes == U"yesnocancel")
                {
                    Buttons = MessageBox::Buttons::Type::YesNoCancel;
                }
            }

            MessageBox::Show(
                GetWindow()->App(), Title.c_str(), Message.c_str(), [this](MessageBox::Response Response) -> void
                {
                    printf("Response: %d\n", (int)Response);
                },
                Buttons);
        }
        else
        {
            printf("Not enough arguments given for displaying message box.");
        }
    }
    else
    {
        Result = false;
    }

    return Result;
}

}

}
