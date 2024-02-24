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

#include "OctaneGUI/OctaneGUI.h"
#include "TestSuite.h"
#include "Utility.h"

namespace Tests
{

TEST_SUITE(CustomControl,

TEST_CASE(Update,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool Updated = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnUpdate([&](OctaneGUI::CustomControl&) -> void
            {
                Updated = true;
            });
    Application.Update();
    return Updated;
})

TEST_CASE(OnPaint,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool Painted = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnPaint([&](const OctaneGUI::CustomControl&, OctaneGUI::Paint&) -> void
            {
                Painted = true;
            });
    Application.Update();
    return Painted;
})

TEST_CASE(OnFocused,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnFocused = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMousePressed([](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                return true;
            })
        .SetOnFocused([&](OctaneGUI::Control&) -> void
            {
                OnFocused = true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    return OnFocused;
})

TEST_CASE(OnUnfocused,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnFocused = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMousePressed([](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                return true;
            })
        .SetOnFocused([&](OctaneGUI::Control&) -> void
            {
                OnFocused = true;
            })
        .SetOnUnfocused([&](OctaneGUI::Control&) -> void
            {
                OnFocused = false;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    VERIFYF(OnFocused, "Custom control did not receive the OnFocused event!");
    Utility::MousePress(Application, {105.0f, 105.0f});
    return !OnFocused;
})

TEST_CASE(OnLoad,
{
    printf("'OnLoad' test is currently empty. Control::OnLoad is only called after the control is created. "
        "There is currently no way to hook into the callback without a created control.\n");
    return true;
})

TEST_CASE(OnSave,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnSave = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnSave([&](const OctaneGUI::CustomControl&, OctaneGUI::Json&) -> void
            {
                OnSave = true;
            });
    Application.Update();
    OctaneGUI::Json Root{ OctaneGUI::Json::Type::Object };
    Application.GetMainWindow()->GetContainer()->OnSave(Root);
    return OnSave;
})

TEST_CASE(OnKeyPressed,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnKeyPressed = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnKeyPressed([&](OctaneGUI::CustomControl&, OctaneGUI::Keyboard::Key) -> bool
            {
                OnKeyPressed = true;
                return true;
            })
        .SetOnMousePressed([](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                return true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    Utility::KeyEvent(Application, OctaneGUI::Keyboard::Key::Enter);
    return OnKeyPressed;
})

TEST_CASE(OnKeyReleased,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnKeyPressed = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnKeyPressed([&](OctaneGUI::CustomControl&, OctaneGUI::Keyboard::Key) -> bool
            {
                OnKeyPressed = true;
                return true;
            })
        .SetOnKeyReleased([&](OctaneGUI::CustomControl&, OctaneGUI::Keyboard::Key) -> void
            {
                OnKeyPressed = false;
            })
        .SetOnMousePressed([](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                return true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    Application.GetMainWindow()->OnKeyPressed(OctaneGUI::Keyboard::Key::Enter);
    VERIFYF(OnKeyPressed, "Custon control did not receive the OnKeyPressed callback!");
    Application.GetMainWindow()->OnKeyReleased(OctaneGUI::Keyboard::Key::Enter);
    return !OnKeyPressed;
})

TEST_CASE(OnMouseMove,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMouseMoved = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMouseMove([&](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&) -> void
            {
                OnMouseMoved = true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MouseMove(Application, {5.0f, 5.0f});
    Application.Update();
    return OnMouseMoved;
})

TEST_CASE(OnMousePressed,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMousePressed = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMousePressed([&](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                OnMousePressed = true;
                return true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    return OnMousePressed;
})

TEST_CASE(OnMouseReleased,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMousePressed = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMousePressed([&](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                OnMousePressed = true;
                return true;
            })
        .SetOnMouseReleased([&](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button) -> void
            {
                OnMousePressed = false;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MousePress(Application, {5.0f, 5.0f});
    Application.Update();
    VERIFYF(OnMousePressed, "Custom control did not receive the OnMousePressed event.");
    Utility::MouseRelease(Application, {5.0f, 5.0f});
    return !OnMousePressed;
})

TEST_CASE(OnMouseWheel,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMouseWheel = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMouseWheel([&](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&) -> void
            {
                OnMouseWheel = true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MouseMove(Application, {5.0f, 5.0f});
    Application.Update();
    Application.GetMainWindow()->OnMouseWheel({ 0.0f, 1.0f });
    return OnMouseWheel;
})

TEST_CASE(OnMouseEnter,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMouseEnter = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMouseEnter([&](OctaneGUI::CustomControl&) -> void
            {
                OnMouseEnter = true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MouseMove(Application, {5.0f, 5.0f});
    Application.Update();
    return OnMouseEnter;
})

TEST_CASE(OnMouseLeave,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnMouseEnter = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnMouseEnter([&](OctaneGUI::CustomControl&) -> void
            {
                OnMouseEnter = true;
            })
        .SetOnMouseLeave([&](OctaneGUI::CustomControl&) -> void
            {
                OnMouseEnter = false;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MouseMove(Application, {5.0f, 5.0f});
    Application.Update();
    VERIFYF(OnMouseEnter, "Custom control did not receive the OnMouseEnter event.");
    Utility::MouseMove(Application, {105.0f, 105.0f});
    return !OnMouseEnter;
})

TEST_CASE(OnResized,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnResized = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnResized([&](OctaneGUI::CustomControl&) -> void
            {
                OnResized = true;
            })
        .SetSize({ 100.0f, 100.0f });
    Application.Update();
    return OnResized;
})

TEST_CASE(OnText,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnText = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnText([&](OctaneGUI::CustomControl&, uint32_t) -> void
            {
                OnText = true;
            })
        .SetOnMousePressed([](OctaneGUI::CustomControl&, const OctaneGUI::Vector2&, OctaneGUI::Mouse::Button, OctaneGUI::Mouse::Count) -> bool
            {
                return true;
            })
        .SetSize({ 100.0f, 100.0f });
    Utility::MouseClick(Application, {5.0f, 5.0f});
    Application.Update();
    Application.GetMainWindow()->OnText(U'A');
    return OnText;
})

TEST_CASE(OnThemeLoaded,
{
    OctaneGUI::ControlList List;
    Utility::Load(Application, "", "", List, 200, 200);
    bool OnThemeLoaded = false;
    Application.GetMainWindow()->GetContainer()->AddControl<OctaneGUI::CustomControl>()
        ->SetOnThemeLoaded([&](OctaneGUI::CustomControl&) -> void
            {
                OnThemeLoaded = true;
            });
    Application.Update();
    const OctaneGUI::Json Root { OctaneGUI::Json::Type::Object };
    Application.GetTheme()->Load(Root);
    return OnThemeLoaded;
})

)

}
