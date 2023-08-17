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

#include "CommandLine.h"

#include <exception>

namespace OctaneGUI
{

CommandLine::CommandLine()
{
}

CommandLine& CommandLine::Set(int Argc, char** Argv)
{
    m_Arguments.clear();

    for (int I = 0; I < Argc; I++)
    {
        m_Arguments.push_back(Argv[I]);
    }

    return *this;
}

std::string CommandLine::Get(const char* Argument) const
{
    bool Next { false };
    for (const std::string& Arg : m_Arguments)
    {
        if (Next)
        {
            return Arg;
        }

        if (Arg == Argument)
        {
            Next = true;
        }
    }

    return "";
}

int CommandLine::GetInt(const char* Argument, int Default) const
{
    const std::string Result { Get(Argument) };

    if (Result.empty())
    {
        return Default;
    }

    try
    {
        return std::stoi(Result.c_str());
    }
    catch (const std::exception&)
    {
    }

    return Default;
}

}
