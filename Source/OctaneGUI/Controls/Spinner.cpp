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
#include "../String.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "Text.h"
#include "TextInput.h"

namespace OctaneGUI
{

Spinner::Spinner(Window* InWindow)
    : HorizontalContainer(InWindow)
{
    SetSpacing({ 0.0f, 0.0f });

    m_Input = AddControl<TextInput>();
    m_Input->SetReadOnly(true);
    
    std::shared_ptr<ImageButton> button { nullptr };
    
    m_DecrementButton = AddControl<ImageButton>();
    m_IncrementButton = AddControl<ImageButton>();
    
    m_DecrementButton->SetTexture(InWindow->GetIcons()->GetTexture())
        .SetUVs(InWindow->GetIcons()->GetUVs(Icons::Type::ArrowLeft))
        .SetOnPressed([this](const Button&)
            {
                auto v = RangeValue(m_Value - 1);
                InternalSetText(v);
            }
        );

    m_IncrementButton->SetTexture(InWindow->GetIcons()->GetTexture())
        .SetUVs(InWindow->GetIcons()->GetUVs(Icons::Type::ArrowRight))
        .SetOnPressed([this](const Button&)
            {
                auto v = RangeValue(m_Value + 1);
                InternalSetText(v);
            }
        );
    
    Vector2 ButtonSize = Vector2(m_IncrementButton->GetSize().X * 0.75f, m_Input->GetSize().Y);
    
    m_DecrementButton->SetSize(ButtonSize);
    m_IncrementButton->SetSize(ButtonSize);

    // Setup the color to use by default for the image button.
    OnThemeLoaded();
}

void Spinner::Update()
{
}

void Spinner::OnLoad(const Json& Root)
{
    m_MinSet = (Root["Range"].Contains("Min") );
    m_Min = Root["Range"]["Min"].Number();
    
    m_MaxSet = (Root["Range"].Contains("Max") );
    m_Max = Root["Range"]["Max"].Number();

    InternalSetText(RangeValue(static_cast<int32_t>(Root["Value"].Number())));
    
    auto p =1;
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

void Spinner::OnThemeLoaded()
{
    m_DecrementButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
    m_DecrementButton->OnThemeLoaded();
    
    m_IncrementButton->SetProperty(ThemeProperties::ImageButton, GetProperty(ThemeProperties::Check).ToColor());
    m_IncrementButton->OnThemeLoaded();
}

void Spinner::InternalSetText(int32_t InValue)
{
    m_Value = InValue;
    m_Input->SetText(std::to_string(m_Value).c_str());
}

int32_t Spinner::GetValue() const
{
    return m_Value;
}

}
