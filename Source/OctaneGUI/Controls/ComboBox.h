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

#include "HorizontalContainer.h"

namespace OctaneGUI
{

class ImageButton;
class ListBox;
class Text;
class TextInput;

class ComboBox : public HorizontalContainer
{
    CLASS(ComboBox)

public:
    typedef std::function<void(const std::u32string&)> OnSelectedSignature;

    ComboBox(Window* InWindow);

    ComboBox& SetExpand(Expand InExpand);
    ComboBox& SetWidth(float Width);
    ComboBox& SetSelected(const char32_t* InText);
    ComboBox& SetSelectedIndex(int Index);
    ComboBox& Clear();

    std::shared_ptr<Text> AddItem(const char* Item);
    std::shared_ptr<Text> AddItem(const char32_t* Item);

    int SelectedIndex() const;

    void Close();
    bool IsOpen() const;

    ComboBox& SetOnSelected(OnSelectedSignature&& Fn);

    virtual void Update() override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnThemeLoaded() override;

private:
    using Container::AddControl;
    using Container::InsertControl;
    using Control::SetExpand;

    int m_SelectedIndex { -1 };
    std::shared_ptr<TextInput> m_Input { nullptr };
    std::shared_ptr<ImageButton> m_Button { nullptr };
    std::shared_ptr<ListBox> m_List { nullptr };
    OnSelectedSignature m_OnSelected { nullptr };
};

}
