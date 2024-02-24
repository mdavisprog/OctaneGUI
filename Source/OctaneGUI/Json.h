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

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Lexer;

class Json
{
public:
    enum class Type : unsigned char
    {
        Null,
        Boolean,
        Number,
        String,
        Object,
        Array
    };

    static const char* ToString(Type InType);
    static Json Parse(const char* Stream);
    static Json Parse(const char* Stream, bool& IsError);

    // TODO: Maybe not allow declaring a Null type due to no memory allocation
    // 		 for retrieving values from array/string/object? Or memory allocation could
    // 		 be handled better?
    Json();
    Json(Type InType);
    Json(bool Value);
    Json(float Value);
    Json(const char* Value);
    Json(const std::string& Value);
    Json(const Json& Other);
    Json(Json&& Other);
    ~Json();

    Type GetType() const;
    bool IsArray() const;
    bool IsBoolean() const;
    bool IsNull() const;
    bool IsNumber() const;
    bool IsObject() const;
    bool IsString() const;

    bool Boolean(bool Default = false) const;
    float Number(float Default = 0.0f) const;
    const char* String(const char* Default = "") const;
    unsigned int Count() const;

    void ForEach(std::function<void(const std::string&, const Json&)> Callback) const;
    void ForEach(std::function<void(const Json&)> Callback) const;

    Json& operator=(bool Value);
    Json& operator=(float Value);
    Json& operator=(const char* Value);
    Json& operator=(const std::string& Value);
    Json& operator=(const Json& Other);
    Json& operator=(Json&& Other);

    bool operator==(const Json& Other) const;
    bool operator!=(const Json& Other) const;

    Json& operator[](const char* Key);
    Json& operator[](const std::string& Key);
    Json& operator[](unsigned int Index);

    Json& Push(const Json& Value);
    Json& Push(Json&& Value);

    const Json& operator[](const char* Key) const;
    const Json& operator[](const std::string& Key) const;
    const Json& operator[](unsigned int Index) const;

    bool Contains(const std::string& Key) const;
    bool Erase(const std::string& Key);

    std::string ToString() const;
    std::string ToStringPretty() const;

private:
    typedef std::map<std::string, Json> Map;

    union Data
    {
        bool Bool;
        float Number;
        std::string* String;
        std::vector<Json>* Array;
        Map* Object;
    };

    static void ParseKey(Lexer& InLexer, std::string& Key, bool& IsError);
    static void ParseValue(Lexer& InLexer, Json& Value, bool& IsError);
    static void ParseArray(Lexer& InLexer, Json& Root, bool& IsError);
    static void ParseObject(Lexer& InLexer, Json& Root, bool& IsError);
    static Json ParseToken(const std::string& Token, bool& IsError);
    static Json Error(const Lexer& InLexer, const char* Message, ...);

    static const Json Invalid;

    void Clear();

    bool Equals(const Json& Other) const;
    void Copy(const Json& Other);
    void Move(Json&& Other);
    std::string ToString(bool Pretty, int Depth) const;

    Type m_Type { Type::Null };
    Data m_Data {};
};

}
