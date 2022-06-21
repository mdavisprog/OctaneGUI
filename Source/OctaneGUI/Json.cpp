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

#include "Json.h"

#include <cassert>
#include <cctype>
#include <codecvt>
#include <cstring>
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

std::string Json::ToLower(const std::string& Value)
{
	std::string Result;

	for (const char Ch : Value)
	{
		Result += std::tolower(Ch);
	}

	return Result;
}

std::u32string Json::ToLower(const std::u32string& Value)
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

std::string Json::ToMultiByte(const std::u32string& Value)
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

std::u32string Json::ToUTF32(const std::string& Value)
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

Json Json::Parse(const char* Stream)
{
	Json Result;
	ParseValue(Lexer(Stream), Result);
	return std::move(Result);
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
		return m_Data.Object->size();
	}

	if (IsArray())
	{
		return m_Data.Array->size();
	}

	return 0;
}

void Json::ForEach(std::function<void(const std::string&, const Json&)> Callback) const
{
	if (Callback == nullptr || !IsObject())
	{
		return;
	}

	for (const std::pair<std::string, Json>& Item : *m_Data.Object)
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
	assert(IsArray());
	m_Data.Array->push_back(Value);
	return *this;
}

Json& Json::Push(Json&& Value)
{
	assert(IsArray());
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

std::string Json::ToString() const
{
	std::string Result;

	if (IsArray())
	{
		Result += "[";
		for (const Json& Item : *m_Data.Array)
		{
			Result += Item.ToString();
			Result += ", ";
		}

		if (m_Data.Array->size() > 0)
		{
			Result.erase(Result.length() - 2);
		}

		Result += "]";
	}
	else if (IsObject())
	{
		Result += "{";
		for (const std::pair<std::string, Json>& Item : *m_Data.Object)
		{
			Result += Item.first;
			Result += ": ";
			Result += Item.second.ToString();
			Result += ", ";
		}

		if (m_Data.Object->size() > 0)
		{
			Result.erase(Result.length() - 2);
		}

		Result += "}";
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

void Json::ParseKey(Lexer& InLexer, std::string& Key)
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
				break;
			}
		}
		else if (Ch == '}')
		{
			break;
		}
		else if (Parsing)
		{
			Key += Ch;
		}

		InLexer.Next();
	}
}

void Json::ParseValue(Lexer& InLexer, Json& Value)
{
	Value = Json();

	std::string Token;
	bool ParseString = false;
	unsigned char LastCh = 0;
	while (!InLexer.IsEnd())
	{
		unsigned char Ch = InLexer.Current();

		if (Ch == '{')
		{
			InLexer.Next();
			ParseObject(InLexer, Value);
			break;
		}
		else if (Ch == '[')
		{
			InLexer.Next();
			ParseArray(InLexer, Value);
			break;
		}
		else if (Ch == '"')
		{
			ParseString = !ParseString;
			Token += Ch;
		}
		else if (!ParseString && (Ch == ',' || Ch == '}' || Ch == ']'))
		{
			Value = ParseToken(Token);
			Token = "";
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

	if (!Token.empty())
	{
		Value = ParseToken(Token);
	}
}

void Json::ParseArray(Lexer& InLexer, Json& Root)
{
	Root = Json(Type::Array);

	while (!InLexer.IsEnd())
	{
		Json Value;
		ParseValue(InLexer, Value);

		if (!Value.IsNull())
		{
			Root.m_Data.Array->push_back(std::move(Value));
		}

		InLexer.ConsumeSpaces();

		if (InLexer.Current() == ']')
		{
			InLexer.Next();
			break;
		}

		InLexer.Next();
	}
}

void Json::ParseObject(Lexer& InLexer, Json& Root)
{
	Root = Json(Type::Object);

	std::string Key;
	while (!InLexer.IsEnd())
	{
		ParseKey(InLexer, Key);
		if (!Key.empty())
		{
			Json Value;
			InLexer.Next();
			ParseValue(InLexer, Value);
			Root[Key] = std::move(Value);
		}

		InLexer.ConsumeSpaces();

		if (InLexer.Current() == '}')
		{
			InLexer.Next();
			break;
		}

		InLexer.Next();
	}
}

Json Json::ParseToken(const std::string& Token)
{
	Json Result;

	if (Token.empty())
	{
		return Result;
	}

	std::string Lower = ToLower(Token);
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
	else if (Token.find_first_not_of("-.0123456789") == std::string::npos)
	{
		Result = std::stof(Token);
	}

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
		for (int I = 0; I < Count(); I++)
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

}
