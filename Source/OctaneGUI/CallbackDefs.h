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

#include <functional>
#include <memory>

namespace OctaneGUI
{

class Button;
class Container;
class Control;
class MenuItem;
class ScrollBar;
class TextSelectable;
class Tree;

enum class InvalidateType : unsigned char
{
    Layout,
    Paint,
    Both
};

typedef std::function<void()> OnEmptySignature;
typedef std::function<void(const Container&)> OnContainerSignature;
typedef std::function<void(const Control&)> OnControlSignature;
typedef std::function<void(std::shared_ptr<Control>, InvalidateType)> OnInvalidateSignature;
typedef std::function<void(const ScrollBar&)> OnScrollBarSignature;
typedef std::function<void(TextSelectable&)> OnTextSelectableSignature;
typedef std::function<void(Tree&)> OnTreeSignature;

}
