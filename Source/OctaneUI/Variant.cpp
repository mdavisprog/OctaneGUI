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

#include "Variant.h"

#include <cstring>

namespace OctaneUI
{

Variant::Variant()
	: m_Type(Type::Null)
	, m_Data()
{
}

Variant::~Variant()
{
}

Variant& Variant::operator=(bool Value)
{
	m_Type = Type::Bool;
	m_Data.Bool = Value;
	return *this;
}

Variant& Variant::operator=(int Value)
{
	m_Type = Type::Int;
	m_Data.Int = Value;
	return *this;
}

Variant& Variant::operator=(float Value)
{
	m_Type = Type::Float;
	m_Data.Float = Value;
	return *this;
}

Variant& Variant::operator=(const char* Value)
{
	m_Type = Type::String;
	m_String = Value;
	return *this;
}

Variant& Variant::operator=(const std::string& Value)
{
	m_Type = Type::String;
	m_String = Value;
	return *this;
}

bool Variant::Bool() const
{
	return m_Data.Bool;
}

int Variant::Int() const
{
	return m_Data.Int;
}

float Variant::Float() const
{
	return m_Data.Float;
}

const char* Variant::String() const
{
	return m_String.c_str();
}

bool Variant::IsNull() const
{
	return m_Type == Type::Null;
}

bool Variant::IsBool() const
{
	return m_Type == Type::Bool;
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

Variant::Type Variant::GetType() const
{
	return m_Type;
}

}
