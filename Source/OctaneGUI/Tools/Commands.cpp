/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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

#include "Commands.h"

namespace OctaneGUI
{
namespace Tools
{

Commands::Commands()
{
}

Commands& Commands::Register(const char32_t* Name, OnCommandSignature&& Fn)
{
    m_Commands.push_back({ Name, std::move(Fn) });
    return *this;
}

bool Commands::Invoke(const char32_t* Name, const std::vector<std::u32string>& Args) const
{
    for (const Command& Cmd : m_Commands)
    {
        if (Cmd.Name == Name)
        {
            Cmd.Callback(Args);
            return true;
        }
    }

    return false;
}

}
}
