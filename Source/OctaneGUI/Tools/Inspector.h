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

#include <memory>
#include <vector>

namespace OctaneGUI
{

class Container;
class Control;
class ScrollableViewControl;
class Splitter;
class Tree;
struct Vector2;
class Window;

namespace Tools
{

class InspectorProxy;
class Properties;

class Inspector
{
public:
    Inspector();

    void Inspect(Window* Target);

private:
    void Populate();
    void ParseProperty(Control const* Target);
    void Close();

    void SetEnabled(const std::shared_ptr<InspectorProxy>& Proxy, bool Enabled, const Vector2& Size);
    void ExpandTree(const std::shared_ptr<Tree>& Root, std::vector<Control*>& Stack);
    void OnSelected(const std::weak_ptr<Control>& Selected);

    void OnToggled(Tree& Ref);
    void OnSelected(Tree& Ref);
    void OnHovered(Tree& Ref);

    // TODO: Find a way to use a weak_ptr instead.
    Window* m_Target { nullptr };
    std::weak_ptr<ScrollableViewControl> m_TreeView {};
    std::weak_ptr<Tree> m_Tree {};
    std::weak_ptr<Properties> m_Properties {};
    std::weak_ptr<Window> m_Window {};

    std::shared_ptr<InspectorProxy> m_MenuBarProxy { nullptr };
    std::shared_ptr<InspectorProxy> m_BodyProxy { nullptr };
};

}
}
