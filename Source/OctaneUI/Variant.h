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

#pragma once

#include <string>

namespace OctaneUI
{

class Variant
{
public:
	enum class Type : unsigned char
	{
		Null,
		Bool,
		Int,
		Float,
		String
	};

	Variant();
	Variant(bool);
	Variant(int);
	Variant(float);
	Variant(const char*);
	Variant(const std::string&);
	Variant(const Variant&);
	~Variant();

	Variant& operator=(bool Value);
	Variant& operator=(int Value);
	Variant& operator=(float Value);
	Variant& operator=(const char* Value);
	Variant& operator=(const std::string& Value);
	Variant& operator=(const Variant& Value);

	bool Bool() const;
	int Int() const;
	float Float() const;
	const char* String() const;

	bool IsNull() const;
	bool IsBool() const;
	bool IsInt() const;
	bool IsFloat() const;
	bool IsString() const;

	Type GetType() const;

private:
	union Data
	{
		bool Bool;
		int Int;
		float Float;
	};

	void Copy(const Variant&);
	void Clear();

	Type m_Type { Type::Null };
	Data m_Data {};
	std::string m_String;
};

}
