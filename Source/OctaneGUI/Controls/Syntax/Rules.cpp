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

#include "Rules.h"

namespace OctaneGUI
{
namespace Syntax
{

Rules& Rules::Get(const std::u32string& Extension)
{
    if (Extension == U".cpp")
    {
        return s_Cpp;
    }
    else if (Extension == U".json")
    {
        return s_Json;
    }

    return s_Empty;
}

bool Rules::IsEmpty() const
{
    return Keywords.empty() && Ranges.empty();
}

bool Rules::IsValidIdentifier(char32_t Char) const
{
    return std::isalnum(Char) || Char == U'_';
}

Rules Rules::s_Empty {};
Rules Rules::s_Cpp {
    // Keywords
    { U"asm", U"auto", U"bool", U"break", U"case", U"catch", U"char", U"class", U"const", U"const_cast",
        U"continue", U"default", U"delete", U"do", U"double", U"dynamic_cast", U"else", U"enum", U"extern",
        U"false", U"float", U"for", U"friend", U"goto", U"if", U"inline", U"int", U"long", U"mutable",
        U"namespace", U"new", U"nullptr", U"operator", U"private", U"protected", U"public", U"reinterpret_cast",
        U"return", U"short", U"signed", U"sizeof", U"static", U"static_assert", U"static_cast", U"struct",
        U"switch", U"template", U"this", U"throw", U"true", U"try", U"typedef", U"typeid", U"typename", U"union",
        U"unsigned", U"using", U"virtual", U"void", U"volatile", U"wchar_t", U"while" },

    // Ranges
    { { U"/*", U"*/", { 106, 153, 85, 255 } },
        { U"\"", U"\"", { 206, 145, 120, 255 } },
        { U"//", U"\n", { 106, 153, 85, 255 }, false },
        { U"#", U" ", { 189, 99, 197, 255 }, false },
        { U"<", U">", { 206, 145, 120, 255 }, false } }
};

Rules Rules::s_Json {
    // Keywords
    {},

    // Ranges
    { { U"\"", U"\"", { 206, 145, 120, 255 } } }
};

}
}
