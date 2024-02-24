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

namespace OctaneGUI
{

class Json;

struct Vector2
{
public:
    static Vector2 Zero;
    static Json ToJson(const Vector2& Value);
    static Vector2 FromJson(const Json& Value, const Vector2& Default = {});

    float X { 0.0f };
    float Y { 0.0f };

    Vector2();
    Vector2(float InX, float InY);

    Vector2 operator-() const;
    Vector2 operator+(const Vector2& Other) const;
    Vector2 operator-(const Vector2& Other) const;
    Vector2 operator*(const Vector2& Other) const;
    Vector2 operator/(const Vector2& Other) const;

    Vector2 operator+(float Other) const;
    Vector2 operator-(float Other) const;
    Vector2 operator*(float Other) const;
    Vector2 operator/(float Other) const;

    Vector2& operator+=(const Vector2& Other);
    Vector2& operator-=(const Vector2& Other);
    Vector2& operator*=(const Vector2& Other);
    Vector2& operator/=(const Vector2& Other);

    Vector2& operator+=(float Other);
    Vector2& operator-=(float Other);
    Vector2& operator*=(float Other);
    Vector2& operator/=(float Other);

    bool operator==(const Vector2& Other) const;
    bool operator!=(const Vector2& Other) const;

    float Length() const;
    float LengthSq() const;
    Vector2 Invert() const;
    Vector2 Unit() const;
    Vector2 Floor() const;
    bool IsZero() const;
};

}
