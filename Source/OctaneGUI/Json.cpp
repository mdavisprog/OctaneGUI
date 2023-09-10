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

#include "Json.h"
#include "Assert.h"
#include "String.h"

#include <cctype>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <iterator>
#include <utility>

namespace OctaneGUI
{

// TODO: Maybe move this into its own file to be used by other systems.
class Lexer
{
public:
    Lexer(const char* Stream)
        : m_Stream(Stream)
    {
    }

    unsigned char Current() const
    {
        return *m_Stream;
    }

    unsigned char Next()
    {
        if (IsEnd())
        {
            return Current();
        }

        m_Stream++;
        m_Column++;
        if (Current() == '\n')
        {
            m_Line++;
            m_Column = 1;
        }
        return Current();
    }

    bool IsEnd() const
    {
        return Current() == '\0';
    }

    bool IsSpace() const
    {
        return std::isspace(Current());
    }

    void ConsumeSpaces()
    {
        while (!IsEnd() && IsSpace())
        {
            Next();
        }
    }

    int Line() const
    {
        return m_Line;
    }

    int Column() const
    {
        return m_Column;
    }

private:
    const char* m_Stream { nullptr };
    int m_Line { 1 };
    int m_Column { 1 };
};

const char* Json::ToString(Type InType)
{
    switch (InType)
    {
    case Type::Boolean: return "Boolean";
    case Type::Number: return "Number";
    case Type::String: return "String";
    case Type::Object: return "Object";
    case Type::Array: return "Array";
    case Type::Null:
    default: break;
    }
    return "Null";
}

Json Json::Parse(const char* Stream)
{
    bool IsError = false;
    return Parse(Stream, IsError);
}

Json Json::Parse(const char* Stream, bool& IsError)
{
    IsError = false;
    Lexer Lex { Stream };
    Json Result;
    ParseValue(Lex, Result, IsError);
    return Result;
}

const Json Json::Invalid;

Json::Json()
    : m_Type(Type::Null)
{
    Clear();
}

Json::Json(Type InType)
    : m_Type(InType)
{
    if (IsString())
    {
        m_Data.String = new std::string();
    }
    else if (IsArray())
    {
        m_Data.Array = new std::vector<Json>();
    }
    else if (IsObject())
    {
        m_Data.Object = new Map();
    }
}

Json::Json(bool Value)
    : m_Type(Type::Boolean)
{
    m_Data.Bool = Value;
}

Json::Json(float Value)
    : m_Type(Type::Number)
{
    m_Data.Number = Value;
}

Json::Json(const char* Value)
    : m_Type(Type::String)
{
    m_Data.String = new std::string();
    *m_Data.String = Value;
}

Json::Json(const std::string& Value)
    : Json(Value.c_str())
{
}

Json::Json(const Json& Other)
{
    Clear();
    Copy(Other);
}

Json::Json(Json&& Other)
{
    Move(std::move(Other));
}

Json::~Json()
{
    Clear();
}

Json::Type Json::GetType() const
{
    return m_Type;
}

bool Json::IsArray() const
{
    return m_Type == Type::Array;
}

bool Json::IsBoolean() const
{
    return m_Type == Type::Boolean;
}

bool Json::IsNull() const
{
    return m_Type == Type::Null;
}

bool Json::IsNumber() const
{
    return m_Type == Type::Number;
}

bool Json::IsObject() const
{
    return m_Type == Type::Object;
}

bool Json::IsString() const
{
    return m_Type == Type::String;
}

bool Json::Boolean(bool Default) const
{
    if (!IsBoolean())
    {
        return Default;
    }

    return m_Data.Bool;
}

float Json::Number(float Default) const
{
    if (!IsNumber())
    {
        return Default;
    }

    return m_Data.Number;
}

const char* Json::String(const char* Default) const
{
    if (!IsString())
    {
        return Default;
    }

    return m_Data.String->c_str();
}

unsigned int Json::Count() const
{
    if (IsObject())
    {
        return (unsigned int)m_Data.Object->size();
    }

    if (IsArray())
    {
        return (unsigned int)m_Data.Array->size();
    }

    return 0;
}

void Json::ForEach(std::function<void(const std::string&, const Json&)> Callback) const
{
    if (Callback == nullptr || !IsObject())
    {
        return;
    }

    for (const std::pair<std::string, Json> Item : *m_Data.Object)
    {
        Callback(Item.first, Item.second);
    }
}

void Json::ForEach(std::function<void(const Json&)> Callback) const
{
    if (Callback == nullptr || !IsArray())
    {
        return;
    }

    for (const Json& Item : *m_Data.Array)
    {
        Callback(Item);
    }
}

Json& Json::operator=(bool Value)
{
    Clear();
    m_Type = Type::Boolean;
    m_Data.Bool = Value;
    return *this;
}

Json& Json::operator=(float Value)
{
    Clear();
    m_Type = Type::Number;
    m_Data.Number = Value;
    return *this;
}

Json& Json::operator=(const char* Value)
{
    Clear();
    m_Type = Type::String;
    m_Data.String = new std::string();
    *m_Data.String = Value;
    return *this;
}

Json& Json::operator=(const std::string& Value)
{
    Clear();
    m_Type = Type::String;
    m_Data.String = new std::string();
    *m_Data.String = Value;
    return *this;
}

Json& Json::operator=(const Json& Other)
{
    if (this != &Other)
    {
        Clear();
        Copy(Other);
    }
    return *this;
}

Json& Json::operator=(Json&& Other)
{
    Move(std::move(Other));
    return *this;
}

bool Json::operator==(const Json& Other) const
{
    return Equals(Other);
}

bool Json::operator!=(const Json& Other) const
{
    return !Equals(Other);
}

Json& Json::operator[](const char* Key)
{
    return (*m_Data.Object)[Key];
}

Json& Json::operator[](const std::string& Key)
{
    return (*m_Data.Object)[Key];
}

Json& Json::operator[](unsigned int Index)
{
    return (*m_Data.Array)[Index];
}

Json& Json::Push(const Json& Value)
{
    Assert(IsArray(), "Trying to push value onto non-array Json type: %s", ToString(GetType()));
    m_Data.Array->push_back(Value);
    return *this;
}

Json& Json::Push(Json&& Value)
{
    Assert(IsArray(), "Trying to push value onto non-array Json type: %s", ToString(GetType()));
    m_Data.Array->push_back(std::move(Value));
    return *this;
}

const Json& Json::operator[](const char* Key) const
{
    if (!IsObject() || m_Data.Object->find(Key) == m_Data.Object->end())
    {
        return Invalid;
    }

    return m_Data.Object->at(Key);
}

const Json& Json::operator[](const std::string& Key) const
{
    if (!IsObject() || m_Data.Object->find(Key) == m_Data.Object->end())
    {
        return Invalid;
    }

    return m_Data.Object->at(Key);
}

const Json& Json::operator[](unsigned int Index) const
{
    if (!IsArray())
    {
        return Invalid;
    }

    return (*m_Data.Array)[Index];
}

bool Json::Contains(const std::string& Key) const
{
    if (!IsObject())
    {
        return false;
    }

    return m_Data.Object->find(Key) != m_Data.Object->end();
}

bool Json::Erase(const std::string& Key)
{
    if (!IsObject())
    {
        return false;
    }

    if (!Contains(Key))
    {
        return false;
    }

    m_Data.Object->erase(Key);

    return true;
}

std::string Json::ToString() const
{
    return ToString(false, 0);
}

std::string Json::ToStringPretty() const
{
    return ToString(true, 0);
}

void Json::ParseKey(Lexer& InLexer, std::string& Key, bool& IsError)
{
    Key = "";

    bool Parsing = false;
    while (!InLexer.IsEnd())
    {
        unsigned char Ch = InLexer.Current();

        if (Ch == '"')
        {
            if (!Parsing)
            {
                Parsing = true;
            }
            else
            {
                // Advance after the current '"' character.
                InLexer.Next();
                break;
            }
        }
        else if (Parsing)
        {
            Key += Ch;
        }
        else
        {
            Key = "Key does not start with '\"' character.";
            IsError = true;
            break;
        }

        InLexer.Next();
    }
}

void Json::ParseValue(Lexer& InLexer, Json& Value, bool& IsError)
{
    Value = Json();

    InLexer.ConsumeSpaces();

    std::string Token;
    bool ParseString = false;
    unsigned char LastCh = 0;
    while (!InLexer.IsEnd())
    {
        unsigned char Ch = InLexer.Current();

        if (Ch == '{')
        {
            InLexer.Next();
            ParseObject(InLexer, Value, IsError);
            break;
        }
        else if (Ch == '[')
        {
            InLexer.Next();
            ParseArray(InLexer, Value, IsError);
            break;
        }
        else if (Ch == '"')
        {
            Token += Ch;
            if (ParseString)
            {
                InLexer.Next();
                break;
            }
            else
            {
                ParseString = true;
            }
        }
        else if (!ParseString && (Ch == ',' || Ch == '}' || Ch == ']' || std::isspace(Ch)))
        {
            break;
        }
        else if (isalnum(Ch) || ParseString || Ch == '.' || Ch == '-')
        {
            // Check for valid escape sequences.
            if (LastCh == '\\')
            {
                switch (Ch)
                {
                case '"': Ch = '\"'; break;
                case '\\':
                case '/': Token += Ch; break;
                case 'b': Ch = '\b'; break;
                case 'f': Ch = '\f'; break;
                case 'n': Ch = '\n'; break;
                case 'r': Ch = '\r'; break;
                case 't': Ch = '\t'; break;
                case 'u': break; // TODO: Implement parsing 4-digit hex value.
                default: Token += LastCh;
                }
            }

            if (Ch != '\\')
            {
                Token += Ch;
            }
        }

        LastCh = Ch;
        InLexer.Next();
    }

    // This may come up if the stream just contains a number or boolean.
    if (!Token.empty())
    {
        Value = ParseToken(Token, IsError);

        if (IsError)
        {
            Value = Error(InLexer, "Invalid Json value '%s'.", Token.c_str());
        }
    }
}

void Json::ParseArray(Lexer& InLexer, Json& Root, bool& IsError)
{
    Root = Json(Type::Array);

    while (!InLexer.IsEnd())
    {
        if (InLexer.Current() == ']')
        {
            InLexer.Next();
            break;
        }

        Json Value;
        ParseValue(InLexer, Value, IsError);

        if (IsError)
        {
            Root = std::move(Value);
            break;
        }
        else if (!Value.IsNull())
        {
            Root.m_Data.Array->push_back(std::move(Value));
        }

        InLexer.ConsumeSpaces();

        if (InLexer.Current() == ']')
        {
            InLexer.Next();
            break;
        }
        else if (InLexer.Current() == ',')
        {
            InLexer.Next();
        }
        else
        {
            Root = Error(InLexer, "Invalid array separator '%c'. Expected ',' or '].", InLexer.Current());
            IsError = true;
            break;
        }
    }
}

void Json::ParseObject(Lexer& InLexer, Json& Root, bool& IsError)
{
    Root = Json(Type::Object);

    std::string Key;
    while (!InLexer.IsEnd())
    {
        InLexer.ConsumeSpaces();

        // Empty object.
        if (InLexer.Current() == '}')
        {
            InLexer.Next();
            break;
        }

        ParseKey(InLexer, Key, IsError);

        if (IsError)
        {
            Root = Error(InLexer, "%s", Key.c_str());
            break;
        }
        else
        {
            InLexer.ConsumeSpaces();
            if (InLexer.Current() == ':')
            {
                Json Value;
                InLexer.Next();
                ParseValue(InLexer, Value, IsError);
                if (IsError)
                {
                    Root = std::move(Value);
                }
                else
                {
                    Root[Key] = std::move(Value);
                }
            }
            else
            {
                Root = Error(InLexer, "Expected ':' character. Found '%c' instead.", InLexer.Current());
                IsError = true;
                break;
            }
        }

        InLexer.ConsumeSpaces();

        if (InLexer.Current() == '}')
        {
            // Object is complete.
            InLexer.Next();
            break;
        }
        else if (InLexer.Current() == ',')
        {
            InLexer.Next();
        }
        else
        {
            Root = Error(InLexer, "Invalid object separator '%c'. Expected ',' or '}'.", InLexer.Current());
            IsError = true;
            break;
        }
    }
}

Json Json::ParseToken(const std::string& Token, bool& IsError)
{
    Json Result;

    if (Token.empty())
    {
        return Result;
    }

    std::string Lower = String::ToLower(Token);
    if (Token.front() == '\"' && Token.back() == '\"')
    {
        Result = Token.substr(1, Token.length() - 2);
    }
    else if (Lower == "true")
    {
        Result = true;
    }
    else if (Lower == "false")
    {
        Result = false;
    }
    else if (Lower == "null")
    {
        // Do nothing.
    }
    else if (Token.find_first_not_of("-.0123456789") == std::string::npos)
    {
        Result = std::stof(Token);
    }
    else
    {
        IsError = true;
    }

    return Result;
}

Json Json::Error(const Lexer& InLexer, const char* Message, ...)
{
    va_list List;
    va_start(List, Message);

    std::string Buffer;
    Buffer.resize(SHRT_MAX);
    vsnprintf(Buffer.data(), Buffer.size(), Message, List);

    va_end(List);

    const std::string Position = std::to_string(InLexer.Line()) + ":" + std::to_string(InLexer.Column());
    Json Result { Type::Object };
    Result["Error"] = Position + " " + Buffer;
    Result["Line"] = (float)InLexer.Line();
    Result["Column"] = (float)InLexer.Column();

    // Should be move constructed.
    return Result;
}

void Json::Clear()
{
    if (IsString())
    {
        delete m_Data.String;
    }
    else if (IsArray())
    {
        delete m_Data.Array;
    }
    else if (IsObject())
    {
        delete m_Data.Object;
    }

    m_Type = Type::Null;
    m_Data.String = nullptr;
}

bool Json::Equals(const Json& Other) const
{
    if (IsNull() && Other.IsNull())
    {
        return true;
    }

    if (IsBoolean() && Other.IsBoolean() && Boolean() == Other.Boolean())
    {
        return true;
    }

    if (IsNumber() && Other.IsNumber() && Number() == Other.Number())
    {
        return true;
    }

    if (IsString() && Other.IsString() && std::string(String()) == Other.String())
    {
        return true;
    }

    if (IsObject() && Other.IsObject())
    {
        bool Result = true;
        ForEach([&](const std::string& Key, const Json& Value) -> void
            {
                Result &= Value.Equals(Other[Key]);
            });
        return Result;
    }

    if (IsArray() && Other.IsArray())
    {
        bool Result = true;
        for (unsigned int I = 0; I < Count(); I++)
        {
            Result &= (*this)[I].Equals(Other[I]);
        }
        return Result;
    }

    return false;
}

void Json::Copy(const Json& Other)
{
    m_Type = Other.m_Type;

    switch (m_Type)
    {
    case Type::Boolean: m_Data.Bool = Other.Boolean(); break;
    case Type::Number: m_Data.Number = Other.Number(); break;
    case Type::String:
    {
        m_Data.String = new std::string();
        *m_Data.String = Other.String();
    }
    break;
    case Type::Object:
    {
        m_Data.Object = new Map();
        *m_Data.Object = *Other.m_Data.Object;
    }
    break;
    case Type::Array:
    {
        m_Data.Array = new std::vector<Json>();
        *m_Data.Array = *Other.m_Data.Array;
    }
    break;
    case Type::Null:
    default: break;
    }
}

void Json::Move(Json&& Other)
{
    if (this == &Other)
    {
        return;
    }

    Clear();
    m_Type = std::exchange(Other.m_Type, Type::Null);

    switch (m_Type)
    {
    case Type::Boolean: m_Data.Bool = std::exchange(Other.m_Data.Bool, false); break;
    case Type::Number: m_Data.Number = std::exchange(Other.m_Data.Number, 0.0f); break;
    case Type::String: m_Data.String = std::exchange(Other.m_Data.String, nullptr); break;
    case Type::Object: m_Data.Object = std::exchange(Other.m_Data.Object, nullptr); break;
    case Type::Array: m_Data.Array = std::exchange(Other.m_Data.Array, nullptr); break;
    case Type::Null:
    default: break;
    }
}

std::string Json::ToString(bool Pretty, int Depth) const
{
    std::string Result;

    if (IsArray())
    {
        Result += "[";
        if (Pretty)
        {
            Result += "\n";
            Depth++;
        }
        for (unsigned int I = 0; I < Count(); I++)
        {
            const Json& Item = m_Data.Array->at((size_t)I);
            Result += std::string(Depth * 4, ' ') + Item.ToString(Pretty, Depth);
            if (I < Count() - 1)
            {
                Result += ",";
            }

            if (Pretty)
            {
                Result += "\n";
            }
            else
            {
                Result += " ";
            }
        }

        if (Pretty)
        {
            Depth--;
        }

        Result += std::string(Depth * 4, ' ') + "]";
    }
    else if (IsObject())
    {
        Result += "{";
        if (Pretty)
        {
            Result += "\n";
            Depth++;
        }
        for (std::map<std::string, Json>::const_iterator It = m_Data.Object->begin(); It != m_Data.Object->end(); ++It)
        {
            Result += std::string(Depth * 4, ' ') + "\"" + It->first + "\"";
            Result += ": ";
            Result += It->second.ToString(Pretty, Depth);

            if (std::next(It) != m_Data.Object->end())
            {
                Result += ",";
            }

            if (Pretty)
            {
                Result += "\n";
            }
            else
            {
                Result += " ";
            }
        }

        if (Pretty)
        {
            Depth--;
        }

        Result += std::string(Depth * 4, ' ') + "}";
    }
    else if (IsBoolean())
    {
        Result = Boolean() ? "true" : "false";
    }
    else if (IsNumber())
    {
        Result = std::to_string(Number());
    }
    else
    {
        Result += "\"";
        Result += String();
        Result += "\"";
    }

    return Result;
}

}
