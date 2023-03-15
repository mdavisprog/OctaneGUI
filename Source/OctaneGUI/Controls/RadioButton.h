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

#include "Button.h"

namespace OctaneGUI
{

class Text;

/// @brief Button that can be part of a group where only one can be selected.
///
/// RadioButton consists of a circle button followed by a text control. RadioButtons
/// within a single container are considered as part of the same group. Only
/// a single RadioButton can be selected within this group. When selected, the circle
/// is filled and the previous selected RadioButton is unfilled.
class RadioButton : public Button
{
    CLASS(RadioButton)

public:
    RadioButton(Window* InWindow);

    RadioButton& SetText(const char32_t* InText);
    const char32_t* GetText() const;

    RadioButton& SetSelected(bool Selected);
    bool IsSelected() const;

    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;

protected:
    virtual void OnClicked() override;

private:
    void Layout();

    std::shared_ptr<Text> m_Text { nullptr };
    float m_Radius { 0.0f };
    bool m_Selected { false };
};

}
