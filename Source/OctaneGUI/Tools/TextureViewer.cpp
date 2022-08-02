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

#include "TextureViewer.h"
#include "../Application.h"
#include "../Controls/ControlList.h"
#include "../Controls/Image.h"
#include "../Controls/ListBox.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Text.h"
#include "../Controls/VerticalContainer.h"
#include "../Font.h"
#include "../Icons.h"
#include "../Paint.h"
#include "../Texture.h"
#include "../TextureCache.h"
#include "../Theme.h"
#include "../Window.h"

#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

class ImagePreview : public VerticalContainer
{
    CLASS(ImagePreview)

public:
    ImagePreview(Window* InWindow)
        : VerticalContainer(InWindow)
    {
        SetPosition({ 4.0f, 4.0f });
        m_Size = AddControl<Text>();
        m_Image = AddControl<Image>();
    }

    void SetTexture(const std::shared_ptr<Texture>& Texture_)
    {
        const Vector2 Size = Texture_->GetSize();
        m_Size->SetText((std::to_string(Size.X) + " x " + std::to_string(Size.Y)).c_str());
        m_Image->SetTexture(Texture_);
        m_Image->SetUVs(Rect(Vector2::Zero, Texture_->GetSize()));
        Invalidate(InvalidateType::Both);
    }

    virtual void OnPaint(Paint& Brush) const override
    {
        Container::OnPaint(Brush);
        Brush.RectangleOutline(m_Image->GetAbsoluteBounds(), Color(255, 255, 0, 255));
    }

private:
    std::shared_ptr<Text> m_Size { nullptr };
    std::shared_ptr<Image> m_Image { nullptr };
};

TextureViewer& TextureViewer::Get()
{
    static TextureViewer s_TextureViewer;
    return s_TextureViewer;
}

void TextureViewer::Show(Application& App)
{
    if (m_Window.expired())
    {
        std::stringstream Stream;
        Stream << "{\"Title\": \"Texture Viewer\", \"Width\": 800, \"Height\": 400, \"Body\": {\"Controls\": ["
               << "{\"Type\": \"Panel\", \"Expand\": \"Both\"},"
               << "{\"Type\": \"Splitter\", \"Expand\": \"Both\", \"Orientation\": \"Vertical\", \"First\": "
               << "{\"Controls\": [{\"ID\": \"List\", \"Type\": \"ListBox\", \"Expand\": \"Both\"}]},"
               << "\"Second\": {\"Controls\": [{\"ID\": \"ImageView\", \"Type\": \"ScrollableViewControl\", \"Expand\": \"Both\"}]}}"
               << "]}}";

        ControlList List;
        m_Window = App.NewWindow("TextureViewer", Stream.str().c_str(), List);
        std::shared_ptr<ListBox> Names = List.To<ListBox>("List");
        Names->SetOnSelect([this](int Index, std::weak_ptr<Control> Item) -> void
            {
                if (Index < m_Textures.size())
                {
                    const std::weak_ptr<Texture> Selected = m_Textures[Index];
                    if (!Selected.expired())
                    {
                        std::shared_ptr<ImagePreview> Preview = m_Preview.lock();
                        std::shared_ptr<Texture> PreviewTexture = Selected.lock();
                        Preview->SetTexture(PreviewTexture);
                    }
                }
            });
        m_Names = Names;

        std::shared_ptr<ScrollableViewControl> View = List.To<ScrollableViewControl>("ImageView");
        m_Preview = View->Scrollable()->AddControl<ImagePreview>();
    }

    App.DisplayWindow("TextureViewer");
    UpdateList();
}

TextureViewer::TextureViewer()
{
}

void TextureViewer::UpdateList()
{
    if (m_Names.expired())
    {
        return;
    }

    std::shared_ptr<ListBox> Names = m_Names.lock();
    Names->ClearItems();
    Names->Invalidate();
    Application& App = Names->GetWindow()->App();

    m_Textures.clear();
    for (const std::pair<std::string, std::shared_ptr<Texture>>& Item : App.GetTextureCache().Cache())
    {
        if (Item.second && Item.second->IsValid())
        {
            Names->AddItem<Text>()->SetText(Item.first.c_str());
            m_Textures.push_back(Item.second);
        }
    }

    for (const std::shared_ptr<Font>& Item : App.GetTheme()->Fonts())
    {
        const std::string Label = std::string(Item->Path()) + " " + std::to_string(Item->Size());
        Names->AddItem<Text>()->SetText(Label.c_str());
        m_Textures.push_back(Item->GetTexture());
    }

    Names->AddItem<Text>()->SetText("Icons");
    m_Textures.push_back(App.GetIcons()->GetTexture());
}

}
}
