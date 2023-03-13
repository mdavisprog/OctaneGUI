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

#include "TextInput.h"

#include <unordered_map>

namespace OctaneGUI
{

class TextEditor : public TextInput
{
    CLASS(TextEditor)

public:
    TextEditor(Window* InWindow);

    TextEditor& SetMatchIndent(bool MatchIndent);
    bool MatchIndent() const;

    TextEditor& SetLineColor(const size_t Line, const Color& _Color);
    TextEditor& ClearLineColor(const size_t Line);
    TextEditor& ClearLineColors();

    TextEditor& OpenFile(const char32_t* FileName);

    virtual void OnLoad(const Json& Root) override;

protected:
    virtual void TextAdded(const std::u32string& Contents) override;

private:
    std::u32string ModifyText(const std::u32string& Pending);
    std::u32string MatchIndent(const std::u32string& Pending) const;
    std::u32string MatchCharacter(const std::u32string& Pending, char32_t Character) const;
    std::u32string ConvertTabs(const std::u32string& Pending) const;
    void PaintLineColors(Paint& Brush) const;
    bool InsertSpaces() const;

    bool m_MatchIndent { true };
    std::unordered_map<size_t, Color> m_LineColors {};
};

}
