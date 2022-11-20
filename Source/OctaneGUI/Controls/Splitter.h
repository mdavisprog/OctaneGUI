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

#include "Container.h"

namespace OctaneGUI
{

class BoxContainer;
class Separator;
class SplitterInteraction;

/// @brief Container that holds multiple containers with a draggable splitter in between each.
///
/// Splitter containers has can container multiple sub-containers that can be populated with controls.
/// They can be oriented either horiztonally or vertically.
class Splitter : public Container
{
    friend SplitterInteraction;

    CLASS(Splitter)

public:
    Splitter(Window* InWindow);

    Splitter& SetOrientation(Orientation InOrientation);
    Orientation GetOrientation() const;

    Splitter& SetSplitterPosition(size_t Index, float Position);
    Vector2 SplitterSize() const;

    Splitter& SetFit(bool Fit);
    bool Fit() const;

    const std::shared_ptr<Container>& GetSplit(size_t Index) const;
    size_t Count() const;

    const std::shared_ptr<Container>& AddContainer();
    Splitter& AddContainers(int Count);

    virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;
    virtual Vector2 DesiredSize() const override;

    virtual void OnLoad(const Json& Root) override;

protected:
    virtual void OnLayoutComplete() override;

private:
    struct Item
    {
    public:
        std::shared_ptr<Container> Data { nullptr };
        std::shared_ptr<Separator> Handle { nullptr };
        float Position { -1.0f };
    };

    void UpdateLayout();
    void Resize();
    void Resize(const std::shared_ptr<Container>& Target, const Vector2& Size);
    std::shared_ptr<Separator> GetSeparator(const Vector2& Point) const;
    std::shared_ptr<Container> GetContainer(const std::shared_ptr<Separator>& Handle) const;
    bool IsSeparator(const std::shared_ptr<Control>& Handle) const;
    std::shared_ptr<Container> CreateContainer();
    Vector2 AvailableSize() const;

    std::shared_ptr<SplitterInteraction> m_Interaction { nullptr };
    std::shared_ptr<BoxContainer> m_Split { nullptr };
    std::vector<Item> m_Items {};
    bool m_Resize { false };
    Orientation m_Orientation { Orientation::Horizontal };
    bool m_Fit { false };
};

}
