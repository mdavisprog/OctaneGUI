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

#include "Vector2.h"

namespace OctaneGUI
{

struct Rect
{
public:
    static Rect FromJson(const Json& Root, const Rect& Default = {});
    static Json ToJson(const Rect& Value);

    Vector2 Min, Max;

    Rect();
    Rect(float MinX, float MinY, float MaxX, float MaxY);
    Rect(const Vector2& InMin, const Vector2& InMax);

    bool operator==(const Rect& Other) const;

    float Width() const;
    float Height() const;
    Vector2 GetSize() const;
    Vector2 GetCenter() const;

    bool IsZero() const;

    bool Contains(float X, float Y) const;
    bool Contains(const Vector2& Point) const;

    Rect& Move(float X, float Y);
    Rect& Move(const Vector2& Delta);

    Rect& SetPosition(float X, float Y);
    Rect& SetPosition(const Vector2& Position);

    Rect& SetSize(const Vector2& Size);

    Rect& Expand(float Width, float Height);
    Rect& Expand(const Vector2& Size);

    Rect& Shrink(float Width, float Height);
    Rect& Shrink(const Vector2& Size);

    bool Intersects(const Rect& Other) const;
    bool Encompasses(const Rect& Other) const;

    Rect Intersection(const Rect& Other) const;
};

}
