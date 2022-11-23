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

namespace OctaneGUI
{

class Container;
class Table;
class Text;
class Tree;
class Window;

namespace Tools
{

class Profiler;
class Timeline;

class ProfileViewer
{
public:
    enum class ViewMode
    {
        Elapsed,
        Count
    };

    ProfileViewer();

    void View(Window* InWindow);

private:
    void SetViewMode(ViewMode Mode);
    void Populate();
    void SetFrame(size_t Index);
    void UpdateFrameInfo();

    size_t m_FrameIndex { 0 };

    std::weak_ptr<Window> m_Window {};
    std::weak_ptr<Container> m_Root {};
    std::weak_ptr<Timeline> m_Timeline {};
    std::weak_ptr<Table> m_Table {};
    std::weak_ptr<Tree> m_Tree {};
    std::weak_ptr<Container> m_FrameTimes {};
    std::weak_ptr<Container> m_InclusiveEventCount {};
    std::weak_ptr<Container> m_ExclusiveEventCount {};
    std::weak_ptr<Text> m_HoveredFrame {};
};

}
}
