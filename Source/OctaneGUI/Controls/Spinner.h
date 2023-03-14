/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

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
#include "HorizontalContainer.h"

namespace OctaneGUI
{

class BoxContainer;
class ImageButton;
class ListBox;
class Text;
class TextInput;

/// @brief Container that allows for incrementing/decrementing a numeric value.
///
/// Spinner container consists of a TextInput control and two buttons that will
/// increment or decrement the internal value. The buttons can be oriented to either
/// be horizontal or vertical.
class Spinner : public HorizontalContainer
{
    CLASS(Spinner)

public:
    Spinner(Window* InWindow);

    Spinner& SetValue(const int32_t Value);
    int32_t Value() const;

    Spinner& SetOrientation(Orientation InOrientation);
    Orientation GetOrientation() const;
    Orientation GetOppositeOrientation() const;

    virtual void OnLoad(const Json& Root) override;
    virtual void OnThemeLoaded() override;

private:
    using Container::AddControl;
    using Container::InsertControl;

    int32_t RangeValue(const int32_t InValue);
    void InternalSetText(int32_t Value);
    void UpdateLayout();

    bool m_MinSet { false };
    bool m_MaxSet { false };

    int32_t m_Min { 0 };
    int32_t m_Max { 0 };
    int32_t m_Value { 0 };

    Orientation m_ButtonOrientation { Orientation::Horizontal };

    std::shared_ptr<TextInput> m_Input { nullptr };
    std::shared_ptr<ImageButton> m_DecrementButton { nullptr };
    std::shared_ptr<ImageButton> m_IncrementButton { nullptr };
    std::shared_ptr<BoxContainer> m_ButtonContainer { nullptr };
};

}
