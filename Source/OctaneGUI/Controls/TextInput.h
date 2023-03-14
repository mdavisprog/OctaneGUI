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

#include "ScrollableViewControl.h"

namespace OctaneGUI
{

class Text;
class TextInputInteraction;
class Timer;

namespace Syntax
{
class Highlighter;
}

/// @brief Control that displays text and allows for user interaction.
///
/// TextInput controls accept text input from the user when the control is
/// focused. The text can be selected by mouse or keyboard. By default,
/// this control is set to be a single line input but can be set to allow
/// multi-line editing. The control can be scrolled if the contents do not
/// fit within the visible bounds. In single-line input mode, no scroll bars
/// are displayed while they are visible in multi-line input mode.
class TextInput : public ScrollableViewControl
{
    friend TextInputInteraction;

    CLASS(TextInput)

public:
    typedef std::function<void(TextInput&)> OnTextInputSignature;
    typedef std::function<std::u32string(TextInput&, const std::u32string&)> OnModifyTextSignature;
    typedef std::function<void(const TextInput&, Paint&)> OnPaintSignature;
    typedef std::function<void(TextInput&, const std::u32string&)> OnTextInputTextSignature;

    class TextPosition
    {
    public:
        TextPosition();
        TextPosition(size_t Line, size_t Column, size_t Index);

        bool operator==(const TextPosition& Other) const;
        bool operator!=(const TextPosition& Other) const;
        bool operator<(const TextPosition& Other) const;

        void Invalidate();
        bool IsValid() const;
        bool IsValidIndex() const;

        void SetLine(size_t Line);
        size_t Line() const;

        void SetColumn(size_t Column);
        size_t Column() const;

        void SetIndex(size_t Index);
        size_t Index() const;

    private:
        size_t m_Line { (size_t)-1 };
        size_t m_Column { (size_t)-1 };
        size_t m_Index { (size_t)-1 };
    };

    TextInput(Window* InWindow);
    virtual ~TextInput();

    TextInput& SetText(const char* InText);
    TextInput& SetText(const char32_t* InText);
    const char32_t* GetText() const;
    const std::u32string& GetString() const;
    const std::u32string_view Line() const;
    const std::u32string_view VisibleText() const;
    const std::u32string_view SelectedText() const;

    TextInput& SelectAll();

    char32_t Left() const;
    char32_t Right() const;

    size_t LineNumber() const;
    size_t Column() const;
    size_t Index() const;

    size_t FirstVisibleIndex() const;
    size_t LastVisibleIndex() const;

    size_t NumVisibleLines() const;

    TextInput& SetReadOnly(bool Value);
    bool ReadOnly() const;

    TextInput& SetMultiline(bool Multiline);
    bool Multiline() const;

    TextInput& SetHighlighter(const std::shared_ptr<Syntax::Highlighter>& Value);
    Color TextColor() const;

    TextInput& SetNumbersOnly(bool NumbersOnly);
    bool NumbersOnly() const;

    TextInput& SetMaxCharacters(uint32_t MaxCharacters);
    uint32_t MaxCharacters() const;

    TextInput& SetWordDelimiters(const char32_t* Delimiters);
    TextInput& AddWordDelimiters(const char32_t* Delimiters);

    template <class T>
    TextInput& CreateHighlighter()
    {
        return SetHighlighter(std::make_shared<T>());
    }

    void Focus();
    void Unfocus();

    TextInput& SetOnTextChanged(OnTextInputSignature&& Fn);
    TextInput& SetOnConfirm(OnTextInputSignature&& Fn);
    TextInput& SetOnModifyText(OnModifyTextSignature&& Fn);
    TextInput& SetOnPrePaintText(OnPaintSignature&& Fn);
    TextInput& SetOnTextAdded(OnTextInputTextSignature&& Fn);

    TextInput& SetFontSize(float FontSize);
    float LineHeight() const;

    virtual void OnPaint(Paint& Brush) const override;
    virtual void OnLoad(const Json& Root) override;
    virtual void OnSave(Json& Root) const override;
    virtual void OnResized() override;
    virtual void OnThemeLoaded() override;

protected:
    void MovePosition(int32_t Line, int32_t Column, bool UseAnchor = false, bool SkipWords = false);

    virtual void TextAdded(const std::u32string& Contents);
    virtual void TextDeleted(const std::u32string_view& Contents);

private:
    void MouseMove(const Vector2& Position);
    bool MousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count);
    void MouseReleased(const Vector2& Position, Mouse::Button Button);

    void AddText(uint32_t Code);
    void AddText(const std::u32string& Contents);
    void EnterPressed();

    void Delete(int32_t Range);
    void MoveHome();
    void MoveEnd();
    void SetPosition(size_t Line, size_t Column, size_t Index);
    Vector2 GetPositionLocation(const TextPosition& Position, bool OffsetFirstLine = true) const;
    TextPosition GetPosition(const Vector2& Position) const;
    bool IsShiftPressed() const;
    bool IsCtrlPressed() const;
    bool IsAltPressed() const;
    bool ShouldSkipWords() const;
    int32_t GetRangeOr(int32_t Value) const;
    size_t LineStartIndex(size_t Index) const;
    size_t LineEndIndex(size_t Index) const;
    size_t LineSize(size_t Index) const;
    void ScrollIntoView();
    void UpdateSpans();
    void InternalSetText(const char32_t* InText);
    void ResetCursorTimer();
    void UpdateVisibleLines();
    void SetVisibleLineSpan();
    void SelectWord();
    void SelectLine();

    std::shared_ptr<Text> m_Text { nullptr };
    TextPosition m_Position {};
    TextPosition m_Anchor {};
    bool m_Focused { false };
    bool m_Drag { false };
    bool m_Multiline { false };
    bool m_ReadOnly { false };
    bool m_NumbersOnly { false };
    uint32_t m_MaxCharacters { 0 };
    std::u32string m_WordDelimiters { U" \t\r\n" };

    std::shared_ptr<Timer> m_BlinkTimer { nullptr };
    bool m_DrawCursor { false };

    TextPosition m_FirstVisibleLine { 0, 0, 0 };
    TextPosition m_LastVisibleLine {};

    std::shared_ptr<Syntax::Highlighter> m_Highlighter { nullptr };

    OnTextInputSignature m_OnTextChanged { nullptr };
    OnTextInputSignature m_OnConfirm { nullptr };
    OnModifyTextSignature m_OnModifyText { nullptr };
    OnPaintSignature m_OnPrePaintText { nullptr };
    OnTextInputTextSignature m_OnTextAdded { nullptr };
};

}
