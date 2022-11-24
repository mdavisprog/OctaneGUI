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
#include "../Controls/MenuItem.h"
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Splitter.h"
#include "../Controls/Text.h"
#include "../Controls/VerticalContainer.h"
#include "../Dialogs/FileDialog.h"
#include "../Font.h"
#include "../Icons.h"
#include "../Paint.h"
#include "../String.h"
#include "../Texture.h"
#include "../TextureCache.h"
#include "../Theme.h"
#include "../Window.h"

namespace OctaneGUI
{
namespace Tools
{

#define ID "TextureViewer"

class ImagePreview : public VerticalContainer
{
public:
    ImagePreview(Window* InWindow)
        : VerticalContainer(InWindow)
    {
        SetPosition({ 4.0f, 4.0f });
        m_Size = AddControl<Text>();
        m_Image = AddControl<Image>();
    }

    void Clear()
    {
        m_Size->SetText(U"");
        m_Image->SetTexture(std::shared_ptr<Texture> { nullptr });
        Invalidate();
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

class TextureViewerContainer : public Container
{
public:
    TextureViewerContainer(Window* InWindow)
        : Container(InWindow)
    {
        SetExpand(Expand::Both);

        AddControl<Panel>()->SetExpand(Expand::Both);

        const std::shared_ptr<Splitter> Split = AddControl<Splitter>();
        // TODO: Need to set the expansion type before setting orientation. Should look into making this order independent.
        Split->SetExpand(Expand::Both);
        Split
            ->AddContainers(2)
            .SetFit(true)
            .SetOrientation(Orientation::Vertical);

        m_List = Split->GetSplit(0)->AddControl<ListBox>();
        m_List
            ->SetOnSelect([this](int Index, std::weak_ptr<Control>) -> void
                {
                    if (Index < (int)m_Textures.size())
                    {
                        const std::weak_ptr<Texture> Selected = m_Textures[Index];
                        if (!Selected.expired())
                        {
                            m_Preview->SetTexture(Selected.lock());
                        }
                    }
                })
            .SetExpand(Expand::Both);

        const std::shared_ptr<ScrollableViewControl> PreviewView = Split->GetSplit(1)->AddControl<ScrollableViewControl>();
        PreviewView->SetExpand(Expand::Both);
        m_Preview = PreviewView->Scrollable()->AddControl<ImagePreview>();
    }

    void UpdateList()
    {
        m_List
            ->ClearItems()
            .Invalidate();
        m_Textures.clear();
        m_Preview->Clear();

        Application& App = GetWindow()->App();

        for (const std::pair<std::string, std::shared_ptr<Texture>> Item : App.GetTextureCache().Cache())
        {
            if (Item.second && Item.second->IsValid())
            {
                m_List->AddItem<Text>()->SetText(Item.first.c_str());
                m_Textures.push_back(Item.second);
            }
        }

        for (const std::shared_ptr<Font>& Item : App.GetTheme()->Fonts())
        {
            const std::string Label = std::string(Item->Path()) + " " + std::to_string(Item->Size());
            m_List->AddItem<Text>()->SetText(Label.c_str());
            m_Textures.push_back(Item->GetTexture());
        }

        m_List->AddItem<Text>()->SetText("Icons");
        m_Textures.push_back(App.GetIcons()->GetTexture());
    }

private:
    std::shared_ptr<ListBox> m_List { nullptr };
    std::shared_ptr<ImagePreview> m_Preview { nullptr };
    std::vector<std::weak_ptr<Texture>> m_Textures {};
};

void Refresh(Application& App)
{
    const std::shared_ptr<Window> TVWindow = App.GetWindow(ID);
    const std::shared_ptr<TextureViewerContainer> TVC =
        std::static_pointer_cast<TextureViewerContainer>(TVWindow->GetContainer()->Get(0));
    TVC->UpdateList();
}

void TextureViewer::Show(Application& App)
{
    if (!App.HasWindow(ID))
    {
        const char* Stream =
            R"({"Title": "Texture Viewer", "Width": 800, "Height": 400, "MenuBar": {"Items": [
    {"Text": "File", "Items": [
        {"Text": "Load", "ID": "File.Load"}
    ]}
]}})";
        ControlList List;
        std::shared_ptr<Window> TVWindow = App.NewWindow(ID, Stream, List);
        TVWindow->GetContainer()->AddControl<TextureViewerContainer>();

        const std::shared_ptr<MenuItem> Load = List.To<MenuItem>("File.Load");
        Load->SetOnPressed([&App](TextSelectable& Item) -> void
            {
                FileSystem& FS = Item.GetWindow()->App().FS();

                FS.SetOnFileDialogResult([&App](FileDialogType, const std::u32string& Path) -> void
                    {
                        if (!Path.empty())
                        {
                            const std::string UTF8Path = String::ToMultiByte(Path);
                            App.GetTextureCache().Load(UTF8Path.c_str());
                            Refresh(App);
                        }
                    });

                const std::vector<FileDialogFilter> Filters { { { U"png", U"svg" }, U"Image Files" } };
                FS.FileDialog(FileDialogType::Open, Filters);
            });
    }

    App.DisplayWindow(ID);
    Refresh(App);
}

}
}
