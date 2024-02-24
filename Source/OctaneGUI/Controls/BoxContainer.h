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

#include "../Orientation.h"
#include "Container.h"

namespace OctaneGUI
{

enum class Grow : uint8_t
{
    Begin,
    Center,
    End
};

/// @brief Container that lays out controls sequentially for an orientation.
///
/// BoxContainers will lay out controls in either horizontal or vertical orientation
/// in sequential order. The container bounds can also grow from the beginning,
/// the center, or the end which will align the controls based on the orientation.
///
/// Controls requested to expand will also be given the space to expand based on the
/// amount of remaining space left after all other non-expanded controls are placed.
///
/// Spacing between each control can be defined or can be packed tightly if set to
/// zero.
class BoxContainer : public Container
{
    CLASS(BoxContainer)

public:
    BoxContainer(Orientation Orient, Window* InWindow);

    BoxContainer& SetGrow(Grow Direction);
    Grow GrowDirection() const;

    BoxContainer& SetSpacing(const Vector2& Spacing);
    Vector2 Spacing() const;

    BoxContainer& SetIgnoreDesiredSize(bool IgnoreDesiredSize);
    bool ShouldIgnoreDesiredSize() const;

    virtual Vector2 DesiredSize() const override;

    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;

protected:
    virtual void PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const override;

private:
    Grow m_Grow { Grow::Begin };
    Orientation m_Orient { Orientation::Horizontal };
    Vector2 m_Spacing { 4.0f, 4.0f };
    bool m_IgnoreDesiredSize { false };
};

}
