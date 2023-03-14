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

#include "MessageBox.h"
#include "../Application.h"
#include "../Controls/HorizontalContainer.h"
#include "../Controls/MarginContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/Text.h"
#include "../Controls/TextButton.h"
#include "../Controls/VerticalContainer.h"
#include "../Font.h"
#include "../String.h"
#include "../Window.h"

namespace OctaneGUI
{
namespace MessageBox
{

#define ID "MessageBox"

class MB : public Container
{
public:
    MB(Window* InWindow)
        : Container(InWindow)
    {
        SetExpand(Expand::Both);

        const std::shared_ptr<Panel> Background = AddControl<Panel>();
        Background->SetExpand(Expand::Both);

        const std::shared_ptr<MarginContainer> Margins = AddControl<MarginContainer>();
        Margins
            ->SetMargins({ 8.0f, 8.0f, 8.0f, 8.0f })
            .SetExpand(Expand::Both);

        const std::shared_ptr<VerticalContainer> Layout = Margins->AddControl<VerticalContainer>();
        Layout
            ->SetSpacing({ 0.0f, 0.0f })
            ->SetExpand(Expand::Both);

        const std::shared_ptr<VerticalContainer> VMessageContainer = Layout->AddControl<VerticalContainer>();
        VMessageContainer
            ->SetGrow(Grow::Center)
            ->SetExpand(Expand::Both);

        const std::shared_ptr<HorizontalContainer> HMessageContainer = VMessageContainer->AddControl<HorizontalContainer>();
        HMessageContainer
            ->SetGrow(Grow::Center)
            ->SetExpand(Expand::Width);

        m_Message = HMessageContainer->AddControl<Text>();
        m_Message->SetWrap(true);

        m_Buttons = Layout->AddControl<HorizontalContainer>();
        m_Buttons
            ->SetGrow(Grow::End)
            ->SetExpand(Expand::Width);
    }

    MB& SetButtons(Buttons::Type ButtonTypes)
    {
        if (ButtonTypes & Buttons::Type::OK)
        {
            AddButton("OK", Response::Accept);
        }

        if (ButtonTypes & Buttons::Type::Yes)
        {
            AddButton("Yes", Response::Accept);
        }

        if (ButtonTypes & Buttons::Type::No)
        {
            AddButton("No", Response::Reject);
        }

        if (ButtonTypes & Buttons::Type::Cancel)
        {
            AddButton("Cancel", Response::Cancel);
        }

        return *this;
    }

    MB& SetResponse(OnResponseSignature&& Fn)
    {
        m_OnResponse = std::move(Fn);
        return *this;
    }

    MB& SetMessage(const char32_t* Message)
    {
        m_Message->SetText(Message);
        Vector2 Size = m_Message->GetFont()->Measure(m_Message->GetString());
        const Vector2 WindowSize = GetWindow()->App().EventFocus()->DeviceSize();
        Size.X = std::max<float>(Size.X, WindowSize.X * 0.10f);
        Size.X = std::min<float>(Size.X, WindowSize.X * 0.60f);
        Size.Y = std::max<float>(Size.Y, WindowSize.Y * 0.10f);
        Size.Y = std::min<float>(Size.Y, WindowSize.Y * 0.75f);
        GetWindow()->SetSize(Size + Vector2(0.0f, m_Buttons->DesiredSize().Y));
        return *this;
    }

private:
    void AddButton(const char* Label, Response InResponse)
    {
        const std::shared_ptr<TextButton> Btn = m_Buttons->AddControl<TextButton>();
        Btn
            ->SetText(Label)
            ->SetOnClicked([this, InResponse](Button&) -> void
                {
                    if (m_OnResponse)
                    {
                        m_OnResponse(InResponse);
                    }
                    Close();
                });
    }

    void Close()
    {
        GetWindow()->App().CloseWindow(ID);
    }

    std::shared_ptr<BoxContainer> m_Buttons { nullptr };
    std::shared_ptr<Text> m_Message { nullptr };
    OnResponseSignature m_OnResponse { nullptr };
};

void Show(Application& App, const char32_t* Title, const char32_t* Message, OnResponseSignature&& Fn, Buttons::Type ButtonTypes)
{
    if (!App.HasWindow(ID))
    {
        App.NewWindow(ID, "{\"Width\": 200, \"Height\": 200, \"Modal\": true, \"Resizable\": false, \"CanMinimize\": false}");
    }

    const std::shared_ptr<Window> MBWindow { App.GetWindow(ID) };
    MBWindow->SetTitle(String::ToMultiByte(Title).c_str());

    MBWindow->GetContainer()->ClearControls();
    const std::shared_ptr<MB> MBContainer = MBWindow->GetContainer()->AddControl<MB>();
    MBContainer
        ->SetButtons(ButtonTypes)
        .SetMessage(Message)
        .SetResponse(std::move(Fn))
        .InvalidateLayout();
    MBWindow->Update();

    App.DisplayWindow(ID);
}

}
}
