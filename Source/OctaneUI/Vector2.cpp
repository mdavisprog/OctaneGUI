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

#include "Vector2.h"

#include <cmath>

namespace OctaneUI
{

Vector2 Vector2::Zero;

Vector2::Vector2()
	: X(0.0f)
	, Y(0.0f)
{
}

Vector2::Vector2(float InX, float InY)
	: X(InX)
	, Y(InY)
{
}

Vector2 Vector2::operator+(const Vector2& Other) const
{
	return Vector2(X + Other.X, Y + Other.Y);
}

Vector2 Vector2::operator-(const Vector2& Other) const
{
	return Vector2(X - Other.X, Y - Other.Y);
}

Vector2 Vector2::operator*(const Vector2& Other) const
{
	return Vector2(X * Other.X, Y * Other.Y);
}

Vector2 Vector2::operator/(const Vector2& Other) const
{
	return Vector2(X / Other.X, Y / Other.Y);
}

Vector2 Vector2::operator*(float Other) const
{
	return Vector2(X * Other, Y * Other);
}

Vector2& Vector2::operator+=(const Vector2& Other)
{
	X += Other.X;
	Y += Other.Y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& Other)
{
	X -= Other.X;
	Y -= Other.Y;
	return *this;
}

Vector2& Vector2::operator*=(const Vector2& Other)
{
	X *= Other.X;
	Y *= Other.Y;
	return *this;
}

Vector2& Vector2::operator/=(const Vector2& Other)
{
	X /= Other.X;
	Y /= Other.Y;
	return *this;
}

float Vector2::Length() const
{
	return std::sqrt(X * X + Y * Y);
}

float Vector2::LengthSq() const
{
	return X * X + Y * Y;
}

Vector2 Vector2::Invert() const
{
	return Vector2(1.0f / X, 1.0f / Y);
}

Vector2 Vector2::Unit() const
{
	float Magnitude = Length();
	if (Magnitude == 0.0f)
	{
		return Vector2();
	}

	return Vector2(X / Magnitude, Y / Magnitude);
}

Vector2 Vector2::Floor() const
{
	return Vector2(std::floor(X), std::floor(Y));
}

bool Vector2::IsZero() const
{
	return X == 0.0f && Y == 0.0f;
}

}
