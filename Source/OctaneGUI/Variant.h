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

#include "Color.h"
#include "Vector2.h"

#include <string>

namespace OctaneGUI
{

class Json;

class Variant
{
public:
    enum class Type : unsigned char
    {
        Null,
        Bool,
        Byte,
        Int,
        Float,
        String,
        Vector,
        Color
    };

    Variant();
    Variant(bool);
    Variant(unsigned char);
    Variant(int);
    Variant(float);
    Variant(const char*);
    Variant(const std::string&);
    Variant(const Vector2&);
    Variant(const Color&);
    Variant(const Json&);
    Variant(const Variant&);
    Variant(Variant&&);
    ~Variant();

    Variant& operator=(bool Value);
    Variant& operator=(unsigned char Value);
    Variant& operator=(int Value);
    Variant& operator=(float Value);
    Variant& operator=(const char* Value);
    Variant& operator=(const std::string& Value);
    Variant& operator=(const Vector2& Value);
    Variant& operator=(const Color& Value);
    Variant& operator=(const Json& Value);
    Variant& operator=(const Variant& Value);
    Variant& operator=(Variant&& Value);

    bool Bool(bool Or = false) const;
    unsigned char Byte(unsigned char Or = 0) const;
    int Int(int Or = 0) const;
    float Float(float Or = 0.0f) const;
    const char* String(const char* Or = "") const;
    Vector2 Vector(const Vector2& Or = {}) const;
    Color ToColor(const Color& Or = {}) const;

    bool IsNull() const;
    bool IsBool() const;
    bool IsByte() const;
    bool IsInt() const;
    bool IsFloat() const;
    bool IsString() const;
    bool IsVector() const;
    bool IsColor() const;

    Type GetType() const;
    std::string ToString() const;

private:
    union Data
    {
        bool Bool;
        unsigned char Byte;
        int Int;
        float Float;
        std::string* String;
        Vector2 Vector;
        Color Col;
    };

    void Copy(const Json&);
    void Copy(const Variant&);
    void Move(Variant&&);
    void Clear();

    Type m_Type { Type::Null };
    Data m_Data {};
};

}
