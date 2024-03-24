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

#pragma once

#include <string>
#include <vector>
#include <cctype>

namespace OctaneGUI
{

// TODO: Look into making this the default string container throughout the library.
class String
{
public:
    static size_t FindFirstOfReverse(const std::u32string& Ref, const std::u32string& Search, size_t Pos);
    static size_t FirdFirstNotOfReverse(const std::u32string& Ref, const std::u32string& Search, size_t Pos);
    static size_t Count(const std::u32string_view& Ref, char32_t Character);

    static std::string ToLower(const std::string& Value);
    static std::u32string ToLower(const std::u32string& Value);

    static std::string ToMultiByte(const std::u32string& Value);
    static std::string ToMultiByte(const char32_t* Value);
    static std::string ToMultiByte(const std::u32string_view& Value);
    static std::string ToMultiByte(const std::wstring& Value);
    static std::wstring ToWide(const std::string& Value);
    static std::wstring ToWide(const std::u32string& Value);
    static std::u32string ToUTF32(const std::string& Value);
    static std::u32string ToUTF32(const char* Value);
    static std::u32string ToUTF32(const std::string_view& Value);
    static std::u32string ToUTF32(const std::wstring& Value);

    static float ToFloat(const std::string& Value);
    static float ToFloat(const std::u32string& Value);

    static std::string ToMultiByte(float Value);
    static std::u32string ToUTF32(float Value);

    static std::vector<std::u32string> Tokenize(const std::u32string& Stream, const std::u32string& Delim);
    static std::vector<std::u32string> ParseArguments(const std::u32string& Stream);

    static std::u32string Replace(const std::u32string& Contents, const std::u32string& What, const std::u32string& With);
};

}
