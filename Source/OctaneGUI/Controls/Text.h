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

#include "../TextSpan.h"
#include "Control.h"

#include <string>
#include <vector>

namespace OctaneGUI
{

class Font;

/// @brief Control that displays text with a specified font.
///
/// Text controls displays text with a font that is either
/// specified in the Theme object or a font that is specific
/// to this control. The control can either be displayed as a
/// single line or a wrap width can be set to be split
/// into multiple lines.
class Text : public Control
{
    CLASS(Text)

public:
    Text(Window* InWindow);

    Text& SetText(const char* InContents);
    Text& SetText(const char32_t* InContents);
    const char32_t* GetText() const;
    const std::u32string& GetString() const;
    size_t Length() const;
    const std::shared_ptr<Font>& GetFont() const;
    float LineHeight() const;

    Text& SetFont(const char* Path);
    Text& SetFontSize(float Size);

    Text& SetWrap(bool Wrap);
    bool Wrap() const;

    void PushSpan(const TextSpan& Span);
    void PushSpans(const std::vector<TextSpan>& Spans);
    void ClearSpans();

    virtual void Update() override;
    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;
    virtual void OnThemeLoaded() override;

protected:
    virtual bool IsFixedSize() const override;

private:
    void UpdateFont();
    void UpdateSize();

    std::u32string m_Contents {};
    Vector2 m_ContentSize {};
    std::vector<TextSpan> m_Spans {};
    std::shared_ptr<Font> m_Font { nullptr };
    bool m_Wrap { false };
};

}
