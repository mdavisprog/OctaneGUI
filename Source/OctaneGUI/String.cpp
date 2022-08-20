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

#include "String.h"

#include <codecvt>

namespace OctaneGUI
{

size_t String::FindFirstOfReverse(const std::u32string& Ref, const std::u32string& Search, size_t Pos)
{
    if (Pos == 0)
    {
        return std::string::npos;
    }

    size_t Start = Pos - 1;
    size_t Result = Ref.find_first_of(Search, Start);
    while (Result != 0 && Result > Start)
    {
        Start = Start - 1;
        Result = Ref.find_first_of(Search, Start);
    }

    if (Result == 0)
    {
        Result = std::string::npos;
    }
    else
    {
        Result++;
    }

    return Result;
}

size_t String::FirdFirstNotOfReverse(const std::u32string& Ref, const std::u32string& Search, size_t Pos)
{
    if (Pos == 0)
    {
        return std::string::npos;
    }

    size_t Start = Pos - 1;
    size_t Result = Ref.find_first_not_of(Search, Start);
    while (Result != 0 && Result > Start)
    {
        Start = Start - 1;
        Result = Ref.find_first_not_of(Search, Start);
    }

    if (Result == 0)
    {
        Result = std::string::npos;
    }
    else
    {
        Result++;
    }

    return Result;
}

size_t String::Count(const std::u32string_view& Ref, char32_t Character)
{
    size_t Result = 0;

    size_t Start = 0;
    size_t Pos = Ref.find(Character, Start);
    while (Pos != std::string::npos)
    {
        Result++;
        Start = Pos + 1;
        Pos = Ref.find(Character, Start);
    }

    return Result;
}

std::string String::ToLower(const std::string& Value)
{
    std::string Result;

    for (const char Ch : Value)
    {
        Result += std::tolower(Ch);
    }

    return Result;
}

std::u32string String::ToLower(const std::u32string& Value)
{
    std::u32string Result;

    for (const char32_t Ch : Value)
    {
        Result += std::tolower(Ch);
    }

    return Result;
}

class Converter : public std::codecvt<char32_t, char, std::mbstate_t>
{
public:
    Converter()
        : std::codecvt<char32_t, char, std::mbstate_t>()
    {
    }

    ~Converter()
    {
    }
};

class WConverter : public std::codecvt<wchar_t, char, std::mbstate_t>
{
public:
    WConverter()
        : std::codecvt<wchar_t, char, std::mbstate_t>()
    {
    }

    ~WConverter()
    {
    }
};

std::string String::ToMultiByte(const std::u32string& Value)
{
    std::string Result;

    std::mbstate_t State {};
    Converter Convert;

    Result.resize(Value.length() * sizeof(char32_t));

    const char32_t* From = nullptr;
    char* To = nullptr;
    Convert.out(State, Value.data(), &Value[Value.size()], From, Result.data(), &Result[Result.size()], To);

    // TODO: Should probably do some error checking here.
    Result.resize(To - Result.data());

    return Result;
}

std::string String::ToMultiByte(const char32_t* Value)
{
    return ToMultiByte(std::u32string { Value });
}

std::string String::ToMultiByte(const std::u32string_view& Value)
{
    return ToMultiByte(std::u32string { Value });
}

std::string String::ToMultiByte(const std::wstring& Value)
{
    std::string Result;

    std::mbstate_t State {};
    WConverter Convert;

    Result.resize(Value.length() * sizeof(wchar_t));

    const wchar_t* From = nullptr;
    char* To = nullptr;
    Convert.out(State, Value.data(), &Value[Value.size()], From, Result.data(), &Result[Result.size()], To);

    // TODO: Should probably do some error checking here.
    Result.resize(To - Result.data());

    return Result;
}

std::u32string String::ToUTF32(const std::string& Value)
{
    std::u32string Result;

    std::mbstate_t State {};
    Converter Convert;

    Result.resize(Value.length());

    const char* From = nullptr;
    char32_t* To = nullptr;
    Convert.in(State, Value.data(), &Value[Value.size()], From, Result.data(), &Result[Result.size()], To);

    return Result;
}

std::u32string String::ToUTF32(const char* Value)
{
    return ToUTF32(std::string { Value });
}

std::u32string String::ToUTF32(const std::string_view& Value)
{
    return ToUTF32(std::string { Value });
}

std::u32string String::ToUTF32(const std::wstring& Value)
{
    // A codecvt<wchar_t, char32_t> does not exist on all platforms.
    // This means two conversions are necessary unfortunately.
    const std::string MBValue = ToMultiByte(Value);
    return ToUTF32(MBValue);
}

float String::ToFloat(const std::string& Value)
{
    return std::stof(Value);
}

float String::ToFloat(const std::u32string& Value)
{
    return ToFloat(ToMultiByte(Value));
}

std::string String::ToMultiByte(float Value)
{
    return std::to_string(Value);
}

std::u32string String::ToUTF32(float Value)
{
    return ToUTF32(ToMultiByte(Value));
}

}
