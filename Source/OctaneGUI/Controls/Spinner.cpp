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

#include "Spinner.h"
#include "../Icons.h"
#include "../Json.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ImageButton.h"
#include "Text.h"
#include "TextInput.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

Spinner::Spinner(Window* InWindow)
    : HorizontalContainer(InWindow)
{
    SetSpacing({ 0.0f, 0.0f });

    m_Input = AddControl<TextInput>();
    m_Input
        ->SetReadOnly(true)
        .SetMultiline(false)
        .SetExpand(Expand::Width);
}

Spinner& Spinner::SetValue(const int32_t Value)
{
    m_Value = RangeValue(Value);
    InternalSetText(m_Value);
    return *this;
}

int32_t Spinner::Value() const
{
    return m_Value;
}

Spinner& Spinner::SetOrientation(Orientation InOrientation)
{
    if (GetOrientation() == InOrientation)
    {
        return *this;
    }

    m_ButtonOrientation = InOrientation;
    UpdateLayout();
    return *this;
}

Orientation Spinner::GetOrientation() const
{
    return m_ButtonOrientation;
}

Orientation Spinner::GetOppositeOrientation() const
{
    return m_ButtonOrientation == Orientation::Horizontal ? Orientation::Vertical : Orientation::Horizontal;
}

void Spinner::OnLoad(const Json& Root)
{
    HorizontalContainer::OnLoad(Root);

    m_MinSet = (Root["Range"].Contains("Min"));
    m_Min = Root["Range"]["Min"].Number();

    m_MaxSet = (Root["Range"].Contains("Max"));
    m_Max = Root["Range"]["Max"].Number();

    SetValue(static_cast<int32_t>(Root["Value"].Number()));

    m_ButtonOrientation = ToOrientation(Root["Orientation"].String());

    UpdateLayout();

    // Setup the color to use by default for the image button.
    OnThemeLoaded();
}

void Spinner::OnThemeLoaded()
{
    HorizontalContainer::OnThemeLoaded();
    m_DecrementButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
    m_IncrementButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
}

int32_t Spinner::RangeValue(const int32_t InValue)
{
    int32_t Value { InValue };

    if (m_MinSet && InValue < m_Min)
    {
        Value = m_Min;
    }
    else if (m_MaxSet && InValue > m_Max)
    {
        Value = m_Max;
    }

    return Value;
}

void Spinner::InternalSetText(int32_t Value)
{
    m_Input->SetText(std::to_string(m_Value).c_str());
}

void Spinner::UpdateLayout()
{
    Icons::Type IncrementIcon;
    Icons::Type DecrementIcon;
    Vector2 ButtonSize;

    if (m_ButtonContainer)
    {
        m_ButtonContainer->ClearControls();
    }

    if (m_ButtonOrientation == Orientation::Vertical)
    {
        m_ButtonContainer = AddControl<VerticalContainer>();

        IncrementIcon = Icons::Type::ArrowUp;
        DecrementIcon = Icons::Type::ArrowDown;
        ButtonSize = Vector2(m_Input->GetSize() * Vector2 { 0.50f, 0.50f });

        m_IncrementButton = m_ButtonContainer->AddControl<ImageButton>();
        m_DecrementButton = m_ButtonContainer->AddControl<ImageButton>();
    }
    else
    {
        m_ButtonContainer = AddControl<HorizontalContainer>();

        IncrementIcon = Icons::Type::ArrowRight;
        DecrementIcon = Icons::Type::ArrowLeft;
        ButtonSize = Vector2(m_Input->GetSize() * Vector2 { 0.25f, 1.00f });

        m_DecrementButton = m_ButtonContainer->AddControl<ImageButton>();
        m_IncrementButton = m_ButtonContainer->AddControl<ImageButton>();
    }

    m_ButtonContainer->SetSpacing({ 0.0f, 0.0f });

    m_IncrementButton->SetTexture(GetWindow()->GetIcons()->GetTexture())
        .SetUVs(GetWindow()->GetIcons()->GetUVs(IncrementIcon));
    m_DecrementButton->SetTexture(GetWindow()->GetIcons()->GetTexture())
        .SetUVs(GetWindow()->GetIcons()->GetUVs(DecrementIcon));

    m_DecrementButton->SetSize(ButtonSize);
    m_IncrementButton->SetSize(ButtonSize);

    m_DecrementButton->SetOnPressed([this](const Button&)
        {
            SetValue(m_Value - 1);
        });

    m_IncrementButton->SetOnPressed([this](const Button&)
        {
            SetValue(m_Value + 1);
        });
}

}
