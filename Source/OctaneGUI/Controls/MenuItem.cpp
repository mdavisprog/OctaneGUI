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

#include "MenuItem.h"
#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Profiler.h"
#include "../Window.h"
#include "Menu.h"
#include "Text.h"

namespace OctaneGUI
{

MenuItem::MenuItem(Window* InWindow)
    : TextSelectable(InWindow)
{
}

std::shared_ptr<Menu> MenuItem::CreateMenu()
{
    if (!m_Menu)
    {
        m_Menu = std::make_shared<Menu>(GetWindow());
        m_Menu->SetParent(this);
    }

    return m_Menu;
}

std::shared_ptr<Menu> MenuItem::GetMenu() const
{
    return m_Menu;
}

MenuItem& MenuItem::DestroyMenu()
{
    m_Menu = nullptr;
    return *this;
}

MenuItem& MenuItem::SetIsMenuBar(bool IsMenuBar)
{
    m_IsMenuBar = IsMenuBar;
    return *this;
}

MenuItem& MenuItem::SetChecked(bool Checked)
{
    m_IsChecked = Checked;
    return *this;
}

bool MenuItem::IsChecked() const
{
    return m_IsChecked;
}

void MenuItem::OpenMenu(const Vector2& Position) const
{
    if (!m_Menu)
    {
        return;
    }

    m_Menu->SetPosition(Position);
    m_Menu->Resize();

    if (m_OnOpenMenu)
    {
        m_OnOpenMenu(TShare<MenuItem>());
    }
}

MenuItem& MenuItem::SetOnOpenMenu(OnMenuItemSignature&& Fn)
{
    m_OnOpenMenu = std::move(Fn);
    return *this;
}

void MenuItem::OnPaint(Paint& Brush) const
{
    TextSelectable::OnPaint(Brush);

    PROFILER_SAMPLE_GROUP("MenuItem::OnPaint");

    const Color IconColor = GetProperty(ThemeProperties::Check).ToColor();
    if (m_IsChecked)
    {
        const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check);
        const Vector2 Position = GetAbsolutePosition() + Vector2(6.0f, GetSize().Y * 0.5f - TexCoords.GetSize().Y * 0.5f);

        Brush.Image(
            Rect(Position, Position + TexCoords.GetSize()),
            GetWindow()->GetIcons()->GetUVsNormalized(Icons::Type::Check),
            GetWindow()->GetIcons()->GetTexture(),
            IconColor);
    }

    if (m_Menu && !m_IsMenuBar)
    {
        const Rect TexCoords = GetWindow()->GetIcons()->GetUVs(Icons::Type::ArrowRight);
        const Vector2 Position = GetAbsolutePosition()
            + Vector2(GetSize().X - TexCoords.GetSize().X * 1.5f, GetSize().Y * 0.5f - TexCoords.GetSize().Y * 0.5f);

        Brush.Image(
            Rect(Position, Position + TexCoords.GetSize()),
            GetWindow()->GetIcons()->GetUVsNormalized(Icons::Type::ArrowRight),
            GetWindow()->GetIcons()->GetTexture(),
            IconColor);
    }
}

void MenuItem::Update()
{
    TextSelectable::Update();

    if (!m_IsMenuBar)
    {
        const Vector2 TextPosition = GetTextControl()->GetPosition();
        const Vector2 CheckSize = GetWindow()->GetIcons()->GetUVs(Icons::Type::Check).GetSize();
        GetTextControl()->SetPosition({ CheckSize.X + 12.0f, TextPosition.Y });
    }
}

void MenuItem::OnLoad(const Json& Root)
{
    // No need to use TextSelectable::OnLoad as we don't want to override the
    // text, which is set by the owning Menu class.
    Control::OnLoad(Root);

    const Json& Items = Root["Items"];
    if (!Items.IsNull())
    {
        Json Copy = Root;
        Copy["ID"] = Json();
        const std::shared_ptr<Menu>& NewMenu = CreateMenu();
        NewMenu->OnLoad(Copy);
    }
}

void MenuItem::OnSave(Json& Root) const
{
    TextSelectable::OnSave(Root);

    Root["IsChecked"] = m_IsChecked;
    Root["HasMenu"] = m_Menu != nullptr;
}

bool MenuItem::OnMousePressed(const Vector2& Position, Mouse::Button Button, Mouse::Count Count)
{
    TextSelectable::OnMousePressed(Position, Button, Count);

    if (m_Menu)
    {
        return true;
    }

    GetWindow()->ClosePopup();
    return true;
}

}
