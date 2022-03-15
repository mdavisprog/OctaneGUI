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
#include "Variant.h"

#include <cstring>

namespace OctaneUI
{

Variant::Variant()
{
}

Variant::Variant(bool Value)
	: m_Type(Type::Bool)
{
	m_Data.Bool = Value;
}

Variant::Variant(unsigned char Value)
	: m_Type(Type::Byte)
{
	m_Data.Byte = Value;
}

Variant::Variant(int Value)
	: m_Type(Type::Int)
{
	m_Data.Int = Value;
}

Variant::Variant(float Value)
	: m_Type(Type::Float)
{
	m_Data.Float = Value;
}

Variant::Variant(const char* Value)
	: m_Type(Type::String)
{
	Set(Value);
}

Variant::Variant(const std::string& Value)
	: m_Type(Type::String)
{
	Set(Value.c_str());
}

Variant::Variant(const Vector2& Value)
	: m_Type(Type::Vector)
{
	m_Data.Vector = Value;
}

Variant::Variant(const Color& Value)
	: m_Type(Type::Color)
{
	m_Data.Col = Value;
}

Variant::Variant(const Json& Value)
{
	Copy(Value);
}

Variant::Variant(const Variant& Other)
{
	Clear();
	Copy(Other);
}

Variant::Variant(Variant&& Other)
{
	Move(std::move(Other));
}

Variant::~Variant()
{
	Clear();
}

Variant& Variant::operator=(bool Value)
{
	Clear();
	m_Type = Type::Bool;
	m_Data.Bool = Value;
	return *this;
}

Variant& Variant::operator=(unsigned char Value)
{
	Clear();
	m_Type = Type::Byte;
	m_Data.Byte = Value;
	return *this;
}

Variant& Variant::operator=(int Value)
{
	Clear();
	m_Type = Type::Int;
	m_Data.Int = Value;
	return *this;
}

Variant& Variant::operator=(float Value)
{
	Clear();
	m_Type = Type::Float;
	m_Data.Float = Value;
	return *this;
}

Variant& Variant::operator=(const char* Value)
{
	Clear();
	m_Type = Type::String;
	Set(Value);
	return *this;
}

Variant& Variant::operator=(const std::string& Value)
{
	Clear();
	m_Type = Type::String;
	Set(Value.c_str());
	return *this;
}

Variant& Variant::operator=(const Vector2& Value)
{
	Clear();
	m_Type = Type::Vector;
	m_Data.Vector = Value;
	return *this;
}

Variant& Variant::operator=(const Color& Value)
{
	Clear();
	m_Type = Type::Color;
	m_Data.Col = Value;
	return *this;
}

Variant& Variant::operator=(const Json& Value)
{
	Copy(Value);
	return *this;
}

Variant& Variant::operator=(const Variant& Value)
{
	if (this != &Value)
	{
		Clear();
		Copy(Value);
	}
	return *this;
}

Variant& Variant::operator=(Variant&& Value)
{
	Move(std::move(Value));
	return *this;
}

bool Variant::Bool() const
{
	if (!IsBool())
	{
		return false;
	}

	return m_Data.Bool;
}

unsigned char Variant::Byte() const
{
	if (!IsByte())
	{
		return 0;
	}

	return m_Data.Byte;
}

int Variant::Int() const
{
	if (!IsInt())
	{
		return 0;
	}

	return m_Data.Int;
}

float Variant::Float() const
{
	if (!IsFloat())
	{
		return 0.0f;
	}

	return m_Data.Float;
}

const char* Variant::String() const
{
	if (!IsString())
	{
		return "";
	}

	return m_Data.String->c_str();
}

Vector2 Variant::Vector() const
{
	if (!IsVector())
	{
		return {};
	}

	return m_Data.Vector;
}

Color Variant::ToColor() const
{
	if (!IsColor())
	{
		return {};
	}

	return m_Data.Col;
}

bool Variant::IsNull() const
{
	return m_Type == Type::Null;
}

bool Variant::IsBool() const
{
	return m_Type == Type::Bool;
}

bool Variant::IsByte() const
{
	return m_Type == Type::Byte;
}

bool Variant::IsInt() const
{
	return m_Type == Type::Int;
}

bool Variant::IsFloat() const
{
	return m_Type == Type::Float;
}

bool Variant::IsString() const
{
	return m_Type == Type::String;
}

bool Variant::IsVector() const
{
	return m_Type == Type::Vector;
}

bool Variant::IsColor() const
{
	return m_Type == Type::Color;
}

Variant::Type Variant::GetType() const
{
	return m_Type;
}

void Variant::Set(const char* Value)
{
	if (m_Data.String == nullptr)
	{
		m_Data.String = new std::string();
	}

	*m_Data.String = Value;
}

void Variant::Copy(const Json& Value)
{
	Clear();

	switch (Value.GetType())
	{
	case Json::Type::Boolean: m_Type = Type::Bool; m_Data.Bool = Value.Boolean(); break;
	case Json::Type::Number: m_Type = Type::Float; m_Data.Float = Value.Number(); break;
	case Json::Type::String: m_Type = Type::String; Set(Value.String()); break; 
	default: break;
	}
}

void Variant::Copy(const Variant& Other)
{
	m_Type = Other.m_Type;
	switch (m_Type)
	{
	case Type::Bool: m_Data.Bool = Other.Bool(); break;
	case Type::Byte: m_Data.Byte = Other.Byte(); break;
	case Type::Int: m_Data.Int = Other.Int(); break;
	case Type::Float: m_Data.Float = Other.Float(); break;
	case Type::String: Set(Other.String()); break;
	case Type::Vector: m_Data.Vector = Other.Vector(); break;
	case Type::Color: m_Data.Col = Other.ToColor(); break;
	case Type::Null:
	default: break;
	}
}

void Variant::Move(Variant&& Other)
{
	if (this == &Other)
	{
		return;
	}

	Clear();

	m_Type = std::exchange(Other.m_Type, Type::Null);
	switch (m_Type)
	{
	case Type::Bool: m_Data.Bool = std::exchange(Other.m_Data.Bool, false); break;
	case Type::Byte: m_Data.Byte = std::exchange(Other.m_Data.Byte, 0); break;
	case Type::Int: m_Data.Int = std::exchange(Other.m_Data.Int, 0); break;
	case Type::Float: m_Data.Float = std::exchange(Other.m_Data.Float, 0.0f); break;
	case Type::String: m_Data.String = std::exchange(Other.m_Data.String, nullptr); break;
	case Type::Vector: m_Data.Vector = std::exchange(Other.m_Data.Vector, {}); break;
	case Type::Color: m_Data.Col = std::exchange(Other.m_Data.Col, {}); break;
	case Type::Null:
	default: break;
	}
}

void Variant::Clear()
{
	if (IsString())
	{
		if (m_Data.String != nullptr)
		{
			delete m_Data.String;
		}
	}

	m_Type = Type::Null;
	std::memset(&m_Data, 0, sizeof(m_Data));
}

}
