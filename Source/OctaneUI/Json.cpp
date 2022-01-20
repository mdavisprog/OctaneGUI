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

#include <cctype>

namespace OctaneUI
{

std::string ToLower(const std::string& Value)
{
	std::string Result;

	for (const char Ch : Value)
	{
		Result += std::tolower(Ch);
	}

	return Result;
}

const Json Json::Invalid;

Json::Json()
	: m_Type(Type::Null)
	, m_Data()
{
}

Json::Json(Type InType)
	: m_Type(InType)
	, m_Data()
{
}

Json::Json(const Json& Other)
	: m_Type(Other.m_Type)
	, m_Data(Other.m_Data)
	, m_Map(Other.m_Map)
	, m_Array(Other.m_Array)
{
}

Json::~Json()
{
}

Json Json::Parse(const char* Stream)
{
	Json Result;
	ParseValue(Stream, Result);
	return Result;
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

bool Json::GetBoolean() const
{
	return m_Data.Bool();
}

bool Json::GetBooleanOr(bool Default) const
{
	if (!IsBoolean())
	{
		return Default;
	}

	return m_Data.Bool();
}

float Json::GetNumber() const
{
	return m_Data.Float();
}

float Json::GetNumberOr(float Default) const
{
	if (!IsNumber())
	{
		return Default;
	}

	return m_Data.Float();
}

const char* Json::GetString() const
{
	return m_Data.String();
}

const char* Json::GetStringOr(const char* Default) const
{
	if (!IsString())
	{
		return Default;
	}

	return m_Data.String();
}

unsigned int Json::GetCount() const
{
	return m_Array.size();
}

Json& Json::operator=(bool Value)
{
	m_Type = Type::Boolean;
	m_Data = Value;
	return *this;
}

Json& Json::operator=(float Value)
{
	m_Type = Type::Number;
	m_Data = Value;
	return *this;
}

Json& Json::operator=(const char* Value)
{
	m_Type = Type::String;
	m_Data = Value;
	return *this;
}

Json& Json::operator=(const std::string& Value)
{
	m_Type = Type::String;
	m_Data = Value;
	return *this;
}

Json& Json::operator=(const Json& Other)
{
	m_Type = Other.m_Type;
	m_Data = Other.m_Data;
	m_Array = Other.m_Array;
	m_Map = Other.m_Map;
	return *this;
}

Json& Json::operator[](const char* Key)
{
	return m_Map[Key];
}

Json& Json::operator[](const std::string& Key)
{
	return m_Map[Key];
}

Json& Json::operator[](unsigned int Index)
{
	return m_Array[Index];
}

const Json& Json::operator[](const char* Key) const
{
	if (m_Map.find(Key) == m_Map.end())
	{
		return Invalid;
	}

	return m_Map.at(Key);
}

const Json& Json::operator[](const std::string& Key) const
{
	if (m_Map.find(Key) == m_Map.end())
	{
		return Invalid;
	}

	return m_Map.at(Key);
}

const Json& Json::operator[](unsigned int Index) const
{
	return m_Array[Index];
}

std::string Json::ToString() const
{
	std::string Result;

	if (IsArray())
	{
		Result += "[";
		for (const Json& Item : m_Array)
		{
			Result += Item.ToString();
			Result += ", ";
		}

		if (m_Array.size() > 0)
		{
			Result.erase(Result.length() - 2);
		}

		Result += "]";
	}
	else if (IsObject())
	{
		Result += "{";
		for (const std::pair<std::string, Json>& Item : m_Map)
		{
			Result += Item.first;
			Result += ": ";
			Result += Item.second.ToString();
			Result += ", ";
		}

		if (m_Map.size() > 0)
		{
			Result.erase(Result.length() - 2);
		}

		Result += "}";
	}
	else if (IsBoolean())
	{
		Result = GetBoolean() ? "true" : "false";
	}
	else if (IsNumber())
	{
		Result = std::to_string(GetNumber());
	}
	else
	{
		Result += "\"";
		Result += GetString();
		Result += "\"";
	}

	return Result;
}

const char* Json::ParseKey(const char* Stream, std::string& Key)
{
	Key = "";

	if (Stream == nullptr)
	{
		return Stream;
	}

	const char* Ptr = Stream;
	while (*Ptr != '\0')
	{
		char Ch = *Ptr;

		if (std::isalnum(Ch))
		{
			Key += Ch;
		}
		else
		{
			break;
		}

		Ptr++;
	}

	return Ptr;
}

const char* Json::ParseValue(const char* Stream, Json& Value)
{
	Value = Json();

	if (Stream == nullptr)
	{
		return Stream;
	}

	const char* Ptr = Stream;
	std::string Token;
	bool ParseString = false;
	while (*Ptr != '\0')
	{
		char Ch = *Ptr;

		if (Ch == '{')
		{
			Ptr = ParseObject(++Ptr, Value);
			break;
		}
		else if (Ch == '[')
		{
			Ptr = ParseArray(++Ptr, Value);
			break;
		}
		else if (Ch == '"')
		{
			ParseString = !ParseString;
			Token += Ch;
		}
		else if (Ch == ',' || Ch == '}' || Ch == ']')
		{
			if (!Token.empty())
			{
				std::string Lower = ToLower(Token);
				if (Token.front() == '\"' && Token.back() == '\"')
				{
					Value = Token.substr(1, Token.length() - 2);
				}
				else if (Lower == "true")
				{
					Value = true;
				}
				else if (Lower == "false")
				{
					Value = false;
				}
				else if (Token.find_first_not_of("-.0123456789") == std::string::npos)
				{
					Value = std::stof(Token);
				}
			}

			break;
		}
		else if (isalnum(Ch) || ParseString)
		{
			Token += Ch;
		}

		Ptr++;
	}

	return Ptr;
}

const char* Json::ParseArray(const char* Stream, Json& Root)
{
	Root = Json(Type::Array);

	if (Stream == nullptr)
	{
		return Stream;
	}

	const char* Ptr = Stream;
	while (*Ptr != '\0')
	{
		Json Value;
		Ptr = ParseValue(Ptr, Value);

		if (!Value.IsNull())
		{
			Root.m_Array.push_back(Value);
		}

		if (*Ptr == ']')
		{
			Ptr++;
			break;
		}

		Ptr++;
	}

	return Ptr;
}

const char* Json::ParseObject(const char* Stream, Json& Root)
{
	Root = Json(Type::Object);

	if (Stream == nullptr)
	{
		return Stream;
	}

	const char* Ptr = Stream;
	std::string Key;
	while (*Ptr != '\0')
	{
		Ptr = ParseKey(Ptr, Key);
		if (!Key.empty())
		{
			Json Value;
			Ptr = ParseValue(++Ptr, Value);
			Root[Key] = Value;
		}

		if (*Ptr == '}')
		{
			Ptr++;
			break;
		}

		Ptr++;
	}

	return Ptr;
}

}
