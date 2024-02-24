/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

#include "Tools.h"
#include "../Window.h"
#include "CommandPalette.h"
#include "Inspector.h"
#include "Mouse.h"
#include "ProfileViewer.h"

namespace OctaneGUI
{
namespace Tools
{

Interface::Interface()
{
    m_Inspector = std::make_shared<Inspector>();
    m_ProfileViewer = std::make_shared<ProfileViewer>();
    m_Mouse = std::make_shared<Tools::Mouse>();
}

Interface& Interface::ShowCommandPalette(Window* Target)
{
    if (!m_CommandPalette)
    {
        m_CommandPalette = std::make_shared<CommandPalette>(Target);
        m_CommandPalette->SetOnCommand([this](const char32_t* Name, const std::vector<std::u32string>& Args) -> void
            {
                m_Commands.Invoke(Name, Args);
            });
    }

    if (m_CommandPalette != Target->GetPopup())
    {
        m_CommandPalette->SetWindow(Target);
        m_CommandPalette->Show();
    }

    return *this;
}

Interface& Interface::ShowInspector(Window* Target)
{
    m_Inspector->Inspect(Target);
    return *this;
}

Interface& Interface::ShowProfileViewer(Window* Target)
{
    m_ProfileViewer->View(Target);
    return *this;
}

Interface& Interface::RegisterCommand(const char32_t* Name, Commands::OnCommandSignature&& Fn)
{
    m_Commands.Register(Name, std::move(Fn));
    return *this;
}

const std::shared_ptr<Tools::Mouse>& Interface::Mouse() const
{
    return m_Mouse;
}

}
}
