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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"
#include "Utility.h"

namespace Tests
{

TEST_SUITE(TextInput,

TEST_CASE(SingleLine_NoScrollBars,
{
    OctaneGUI::ControlList List;
    Utility::Load(
        Application,
        R"({"Type": "TextInput", "ID": "TextInput", "Text": {"Text": "Well Hello Friends! Welcome to the program!"}})",
        List);
    
    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    return !TextInput->Scrollable()->HorizontalScrollBar()->ShouldPaint() && !TextInput->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(MultiLine_ScrollBars,
{
    OctaneGUI::ControlList List;
    Utility::Load(
        Application,
        R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Size": [80, 30], "Text": {"Text": "Well Hello Friends!\n Welcome to the program!"}})",
        List);
    
    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    return TextInput->Scrollable()->HorizontalScrollBar()->ShouldPaint() && TextInput->Scrollable()->VerticalScrollBar()->ShouldPaint();
})

TEST_CASE(TextEvent,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput"})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    Utility::TextEvent(Application, U"Well Hello Friends");

    return TextInput->GetString() == U"Well Hello Friends";
})

TEST_CASE(MoveCursorRightNextLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Text": {"Text": "Hello\nFriends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    for (int I = 0; I < 5; I++)
    {
        Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Right);

        VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());
        VERIFYF(TextInput->Column() == (size_t)(I + 1), "TextInput cursor column (%zu) is not %d", TextInput->Column(), I + 1);
    }

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Right);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor line (%zu) is not 0", TextInput->Column());

    return true;
})

TEST_CASE(MoveCursorLeftPrevLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Text": {"Text": "Hello\nFriends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    for (int I = 0; I < 6; I++)
    {
        Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Right);
    }

    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor line (%zu) is not 0", TextInput->Column());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Left);
    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 5, "TextInput cursor line (%zu) is not 5", TextInput->Column());

    return true;
})

TEST_CASE(MoveCursorDownLastLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Text": {"Text": "Hello\nFriends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Down);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1!", TextInput->LineNumber());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Down);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1 again!", TextInput->LineNumber());

    return true;
})

TEST_CASE(MoveCursorUpFirstLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Text": {"Text": "Hello\nFriends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Down);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1!", TextInput->LineNumber());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Up);
    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Up);
    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0 again!", TextInput->LineNumber());

    return true;
})

TEST_CASE(HomeEnd_SingleLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Text": {"Text": "Hello World"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->Column() == 0, "TextInput cursor (%zu) is not at 0!", TextInput->Column());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::End);
    VERIFYF(TextInput->Column() == 11, "TextInput cursor (%zu) is not at 11!", TextInput->Column());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Home);
    VERIFYF(TextInput->Column() == 0, "TextInput cursor (%zu) is not at 0!", TextInput->Column());

    return true;
})

TEST_CASE(HomeEnd_MultiLine,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Text": {"Text": "Hello\nFriends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line number (%zu) is not 0!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor column (%zu) is not 0!", TextInput->Column());
    VERIFYF(TextInput->Index() == 0, "TextInput index (%zu) is not 0!", TextInput->Index());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::End);
    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line number (%zu) is not 0!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 5, "TextInput cursor column (%zu) is not 5!", TextInput->Column());
    VERIFYF(TextInput->Index() == 5, "TextInput index (%zu) is not 5!", TextInput->Index());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Down);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line number (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 5, "TextInput cursor column (%zu) is not 5!", TextInput->Column());
    VERIFYF(TextInput->Index() == 11, "TextInput index (%zu) is not 11!", TextInput->Index());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::End);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line number (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 7, "TextInput cursor column (%zu) is not 7!", TextInput->Column());
    VERIFYF(TextInput->Index() == 13, "TextInput index (%zu) is not 13!", TextInput->Index());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Home);
    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line number (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor column (%zu) is not 0!", TextInput->Column());
    VERIFYF(TextInput->Index() == 6, "TextInput index (%zu) is not 6!", TextInput->Index());

    return true;
})

TEST_CASE(NumbersOnly,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "NumbersOnly": true})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    Utility::TextEvent(Application, U"Hello");
    VERIFYF(TextInput->GetString() == U"", "TextInput contents should be empty, but contains '%s'!", OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    Utility::TextEvent(Application, U"3-.14");
    VERIFYF(TextInput->GetString() == U"3.14", "TextInput contents should be '3.14', but contains '%s'!", OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    TextInput->SetText(U"");
    Utility::TextEvent(Application, U"-3.14");
    VERIFYF(TextInput->GetString() == U"-3.14", "TextInput contents should be '-3.14', but contains '%s'!", OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    return true;
})

TEST_CASE(ReadOnly,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "ReadOnly": true, "Text": {"Text": "Well Hello Friends"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");
    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->GetString() == U"Well Hello Friends", "TextInput contents should be 'Well Hello Friends', but contains '%s'!", OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    Utility::TextEvent(Application, U"Welcome to the program");
    VERIFYF(TextInput->GetString() == U"Well Hello Friends", "TextInput contents should be 'Well Hello Friends', but contains '%s'!", OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    return true;
})

TEST_CASE(MouseSelect,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Text": {"Text": "Hello World"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    VERIFYF(TextInput->SelectedText() == U"", "TextInput has selected text '%s' when it shouldn't!",
        OctaneGUI::String::ToMultiByte(TextInput->SelectedText()).c_str());

    Utility::MousePress(Application, TextInput->GetAbsolutePosition() + OctaneGUI::Vector2(2.0f, 0.0f));
    Application.Update();

    Utility::MouseRelease(Application, TextInput->GetAbsolutePosition() + OctaneGUI::Vector2(TextInput->GetSize().X - 2.0f, 0.0f));
    Application.Update();

    VERIFYF(TextInput->SelectedText() == TextInput->GetText(), "TextInput has selected text '%s' when it should have '%s'!",
        OctaneGUI::String::ToMultiByte(TextInput->SelectedText()).c_str(), OctaneGUI::String::ToMultiByte(TextInput->GetText()).c_str());

    return true;
})

TEST_CASE(PageDown,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    std::string Text;
    const int NumLines = 100;
    for (int I = 0; I < NumLines; I++)
    {
        Text += "Item " + std::to_string(I) + "\n";
    }
    TextInput->SetText(Text.c_str());

    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();
    const size_t NumVisibleLines = TextInput->NumVisibleLines();
    size_t CurrentLine = 0;

    VERIFYF(TextInput->LineNumber() == CurrentLine, "TextInput cursor line (%zu) is not %zu!", TextInput->LineNumber(), CurrentLine);

    for (int I = 1; I < 10; I++)
    {
        Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::PageDown);
        CurrentLine = std::min<size_t>(NumLines, CurrentLine + NumVisibleLines);
        VERIFYF(TextInput->LineNumber() == CurrentLine, "TextInput cursor line (%zu) is not %zu after %d page down!", TextInput->LineNumber(), CurrentLine, I);
    }

    return true;
})

TEST_CASE(PageUp,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    std::string Text;
    const int NumLines = 100;
    for (int I = 0; I < NumLines; I++)
    {
        Text += "Item " + std::to_string(I) + "\n";
    }
    TextInput->SetText(Text.c_str());

    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();
    const size_t NumVisibleLines = TextInput->NumVisibleLines();
    size_t CurrentLine = 0;

    VERIFYF(TextInput->LineNumber() == CurrentLine, "TextInput cursor line (%zu) is not %zu!", TextInput->LineNumber(), CurrentLine);

    for (int I = 1; I < 10; I++)
    {
        Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::PageDown);
        CurrentLine = std::min<size_t>(NumLines, CurrentLine + NumVisibleLines);
        VERIFYF(TextInput->LineNumber() == CurrentLine, "TextInput cursor line (%zu) is not %zu after %d page down!", TextInput->LineNumber(), CurrentLine, I);
    }

    for (int I = 1; I < 10; I++)
    {
        Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::PageUp);
        CurrentLine = CurrentLine > NumVisibleLines ? CurrentLine - NumVisibleLines : 0;
        VERIFYF(TextInput->LineNumber() == CurrentLine, "TextInput cursor line (%zu) is not %zu after %d page up!", TextInput->LineNumber(), CurrentLine, I);
    }

    return true;
})

TEST_CASE(NewLineZeroIndex,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, R"({"Type": "TextInput", "ID": "TextInput", "Multiline": true, "Text": {"Text": "Hello\nWorld"}})", List);

    const std::shared_ptr<OctaneGUI::TextInput> TextInput = List.To<OctaneGUI::TextInput>("TextInput");

    Utility::MouseClick(Application, TextInput->GetAbsolutePosition());
    Application.Update();

    VERIFYF(TextInput->LineNumber() == 0, "TextInput cursor line (%zu) is not 0!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor column (%zu) is not 0!", TextInput->Column());
    VERIFYF(TextInput->Index() == 0, "TextInput cursor index (%zu) is not 0!", TextInput->Index());

    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Enter);
    Application.Update();

    VERIFYF(TextInput->LineNumber() == 1, "TextInput cursor line (%zu) is not 1!", TextInput->LineNumber());
    VERIFYF(TextInput->Column() == 0, "TextInput cursor column (%zu) is not 0!", TextInput->Column());
    VERIFYF(TextInput->Index() == 1, "TextInput cursor index (%zu) is not 1!", TextInput->Index());

    return true;
})

)

}
