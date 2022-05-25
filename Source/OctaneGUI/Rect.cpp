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

#include "Rect.h"
#include "Json.h"

#include <cmath>

namespace OctaneGUI
{

Rect Rect::FromJson(const Json& Root, const Rect& Default)
{
	if (!Root.IsArray())
	{
		return Default;
	}

	return {
		Root.Count() > 0 ? Root[0u].Number() : Default.Min.X,
		Root.Count() > 1 ? Root[1u].Number() : Default.Min.Y,
		Root.Count() > 2 ? Root[2u].Number() : Default.Max.X,
		Root.Count() > 3 ? Root[3u].Number() : Default.Max.Y
	};
}

Json Rect::ToJson(const Rect& Value)
{
	Json Result(Json::Type::Array);

	Result.Push(Value.Min.X);
	Result.Push(Value.Min.Y);
	Result.Push(Value.Max.X);
	Result.Push(Value.Max.Y);

	return std::move(Result);
}

Rect::Rect()
	: Min()
	, Max()
{
}

Rect::Rect(float MinX, float MinY, float MaxX, float MaxY)
	: Min(MinX, MinY)
	, Max(MaxX, MaxY)
{
}

Rect::Rect(const Vector2& InMin, const Vector2& InMax)
	: Min(InMin)
	, Max(InMax)
{
}

float Rect::Width() const
{
	return Max.X - Min.X;
}

float Rect::Height() const
{
	return Max.Y - Min.Y;
}

Vector2 Rect::GetSize() const
{
	return Max - Min;
}

Vector2 Rect::GetCenter() const
{
	return Min + GetSize() * 0.5f;
}

bool Rect::IsZero() const
{
	return Width() == 0.0f && Height() == 0.0f;
}

bool Rect::Contains(float X, float Y) const
{
	return Contains(Vector2(X, Y));
}

bool Rect::Contains(const Vector2& Point) const
{
	return Min.X <= Point.X && Point.X <= Max.X && Min.Y <= Point.Y && Point.Y <= Max.Y;
}

Rect& Rect::Move(float X, float Y)
{
	return Move(Vector2(X, Y));
}

Rect& Rect::Move(const Vector2& Delta)
{
	Min += Delta;
	Max += Delta;
	return *this;
}

Rect& Rect::SetPosition(float X, float Y)
{
	return SetPosition(Vector2(X, Y));
}

Rect& Rect::SetPosition(const Vector2& Position)
{
	const Vector2 Size = GetSize();
	Min = Position.Floor();
	Max = Min + Size;
	return *this;
}

Rect& Rect::SetSize(const Vector2& Size)
{
	Max = (Min + Size).Floor();
	return *this;
}

Rect& Rect::Expand(float Width, float Height)
{
	return Expand(Vector2(Width, Height));
}

Rect& Rect::Expand(const Vector2& Size)
{
	const Vector2 Half = Size * 0.5f;
	Min -= Half;
	Max += Half;
	return *this;
}

Rect& Rect::Shrink(float Width, float Height)
{
	return Shrink(Vector2(Width, Height));
}

Rect& Rect::Shrink(const Vector2& Size)
{
	const Vector2 Half = Size * 0.5f;
	Min += Half;
	Max -= Half;
	return *this;
}

bool Rect::Intersects(const Rect& Other) const
{
	if (Min.X >= Other.Max.X || Other.Min.X >= Max.X)
	{
		return false;
	}

	if (Max.Y <= Other.Min.Y || Other.Max.Y <= Min.Y)
	{
		return false;
	}

	return true;
}

bool Rect::Encompasses(const Rect& Other) const
{
	return Contains(Other.Min) && Contains(Other.Max);
}

}
