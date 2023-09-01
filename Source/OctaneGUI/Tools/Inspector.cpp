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

#include "Inspector.h"
#include "../Application.h"
#include "../Controls/CheckBox.h"
#include "../Controls/Container.h"
#include "../Controls/ControlList.h"
#include "../Controls/MenuBar.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Splitter.h"
#include "../Controls/Tree.h"
#include "../Controls/WindowContainer.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Window.h"
#include "Properties.h"

#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

class InspectorProxy : public Control
{
    CLASS(InspectorProxy)

public:
    typedef std::function<void(const std::weak_ptr<Control>&)> OnSelectedSignature;

    InspectorProxy(Window* InWindow)
        : Control(InWindow)
    {
        SetExpand(Expand::Both);
    }

    InspectorProxy& SetRoot(const std::weak_ptr<Container>& Root)
    {
        m_Root = Root;
        return *this;
    }

    InspectorProxy& SetHovered(const std::weak_ptr<Control>& Hovered)
    {
        if (m_Hovered.lock() == Hovered.lock())
        {
            return *this;
        }

        m_Hovered = Hovered;
        Invalidate();
        return *this;
    }

    InspectorProxy& SetSelected(const std::weak_ptr<Control>& Selected)
    {
        if (m_Selected.lock() == Selected.lock())
        {
            return *this;
        }

        m_Selected = Selected;
        Invalidate();
        return *this;
    }

    InspectorProxy& SetOnSelected(OnSelectedSignature&& Fn)
    {
        m_OnSelected = std::move(Fn);
        return *this;
    }

    InspectorProxy& Clear()
    {
        m_Hovered.reset();
        m_Selected.reset();
        return *this;
    }

    InspectorProxy& SetEnabled(bool Enabled)
    {
        m_Enabled = Enabled;
        return *this;
    }

    bool Enabled() const
    {
        return m_Enabled;
    }

    virtual void OnMouseMove(const Vector2& Position) override
    {
        if (!m_Enabled || m_Root.expired())
        {
            return;
        }

        std::weak_ptr<Control> Hovered = GetControl(Position, m_Root.lock());
        if (m_Hovered.lock() != Hovered.lock())
        {
            m_Hovered = Hovered;
            Invalidate();
        }
    }

    virtual bool OnMousePressed(const Vector2&, Mouse::Button Button, Mouse::Count) override
    {
        if (m_Hovered.expired())
        {
            return false;
        }

        if (Button != Mouse::Button::Left)
        {
            return false;
        }

        m_Selected = m_Hovered;

        if (m_OnSelected)
        {
            m_OnSelected(m_Selected);
        }

        return true;
    }

    virtual void OnMouseLeave() override
    {
        m_Hovered.reset();
    }

    virtual void OnPaint(Paint& Brush) const override
    {
        if (!m_Hovered.expired())
        {
            std::shared_ptr<Control> Hovered = m_Hovered.lock();
            Brush.RectangleOutline(Hovered->GetAbsoluteBounds(), Color(255, 255, 0, 255));
        }

        if (!m_Selected.expired())
        {
            std::shared_ptr<Control> Selected = m_Selected.lock();
            Brush.RectangleOutline(Selected->GetAbsoluteBounds(), Color(0, 255, 0, 255));
        }
    }

private:
    std::weak_ptr<Control> GetControl(const Vector2& Point, const std::shared_ptr<Container>& Root) const
    {
        std::weak_ptr<Control> Result;

        if (!Root)
        {
            return Result;
        }

        const std::vector<std::shared_ptr<Control>>& Controls = Root->Controls();
        for (size_t I = Controls.size() - 1; (int)I >= 0; I--)
        {
            const std::shared_ptr<Control>& Item = Controls[I];
            const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
            if (ItemContainer)
            {
                Result = GetControl(Point, ItemContainer);

                if (Result.expired() && ItemContainer->Contains(Point))
                {
                    Result = ItemContainer;
                }
            }
            else if (Item->Contains(Point) && Item.get() != this)
            {
                Result = Item;
            }

            if (!Result.expired())
            {
                break;
            }
        }

        return Result;
    }

    bool m_Enabled { true };
    std::weak_ptr<Container> m_Root {};
    std::weak_ptr<Control> m_Hovered {};
    std::weak_ptr<Control> m_Selected {};
    OnSelectedSignature m_OnSelected { nullptr };
};

Inspector::Inspector()
{
}

void Inspector::Inspect(Window* Target)
{
#define ID "__TOOLS__.Inspector"

    if (m_Window.expired())
    {
        const char* Stream =
            R"({"Title": "Inspecting", "Width": 600, "Height": 300, "Body": {"Controls": [
    {"Type": "Panel", "Expand": "Both"},
    {"Type": "VerticalContainer", "Expand": "Both", "Controls": [
        {"Type": "CheckBox", "ID": "Picker", "Text": {"Text": "Picker"}},
        {"Type": "Splitter", "Orientation": "Vertical", "Expand": "Both", "Fit": true, "Containers": [
            {"Clip": false, "Controls": [
                {"Type": "ScrollableViewControl", "ID": "TreeView", "Expand": "Both"}
            ]},
            {"Clip": false, "Controls": [
                {"Type": "ScrollableViewControl", "ID": "PropertiesView", "Expand": "Both"}
            ]}
        ]}
    ]}
]}})";

        ControlList List;
        std::shared_ptr<Window> NewWindow = Target->App().NewWindow(ID, Stream, List);
        NewWindow
            ->SetOnClose([this](Window&) -> void
                {
                    Close();
                });
        m_Window = NewWindow;

        List.To<CheckBox>("Picker")
            ->SetState(CheckBox::State::Checked)
            .SetOnClicked([this](Button& Btn) -> void
                {
                    const CheckBox& CB = static_cast<const CheckBox&>(Btn);
                    const bool Enabled = CB.GetState() == CheckBox::State::Checked;
                    SetEnabled(m_MenuBarProxy, Enabled, m_Target->GetMenuBar()->GetSize());
                    SetEnabled(m_BodyProxy, Enabled, m_Target->GetContainer()->GetSize());
                });

        const std::shared_ptr<ScrollableViewControl> TreeView = List.To<ScrollableViewControl>("TreeView");
        const std::shared_ptr<Tree> TreeRoot = TreeView->Scrollable()->AddControl<Tree>();
        TreeRoot
            ->SetOnToggled([this, TreeView](Tree& Ref) -> void
                {
                    OnToggled(Ref);
                })
            .SetOnSelected([this](Tree& Ref) -> void
                {
                    OnSelected(Ref);
                })
            .SetOnHovered([this](Tree& Ref) -> void
                {
                    OnHovered(Ref);
                });
        m_Tree = TreeRoot;
        m_TreeView = TreeView;

        const std::shared_ptr<ScrollableViewControl> PropertiesView = List.To<ScrollableViewControl>("PropertiesView");
        m_Properties = PropertiesView->Scrollable()->AddControl<Properties>();

        m_MenuBarProxy = std::make_shared<InspectorProxy>(NewWindow.get());
        m_MenuBarProxy->SetOnSelected([this](const std::weak_ptr<Control>& Selected) -> void
            {
                OnSelected(Selected);
            });

        m_BodyProxy = std::make_shared<InspectorProxy>(NewWindow.get());
        m_BodyProxy->SetOnSelected([this](const std::weak_ptr<Control>& Selected) -> void
            {
                OnSelected(Selected);
            });
    }

    m_Window.lock()->SetTitle((std::string("Inspecting ") + Target->GetTitle()).c_str());

    if (m_Window.lock()->IsVisible())
    {
        return;
    }

    m_Target = Target;
    m_Target
        ->SetOnClose([this](Window&) -> void
            {
                Close();
            })
        .SetOnLayout([](Window&) -> void
            {
                // TODO: This causes an issue where when the inspector window is opened and this
                // callback is registered, an update is kicked off for all windows. The inspected window will
                // update first and invoke this callback. This window's layout requests have not been
                // processed at this point, and the populate function will remove controls and add
                // new ones to be processed, but the layout requests array has not been cleared.
                // A proper fix for this race condition is to convert the layout requests array to
                // be weak references to controls, as currently they are raw pointers.
                // 11/25/22: Layout requests are now weak references to controls. Need to revisit this.
                //           Need to take into account performance and if a smarter process is needed
                //           e.g. only update visible nodes.
                // Populate();
            });
    Target->App().DisplayWindow(ID);

    m_MenuBarProxy
        ->SetRoot(Target->GetMenuBar())
        .SetSize(Target->GetMenuBar()->GetSize())
        .SetWindow(Target);
    Target->GetMenuBar()->InsertControl(m_MenuBarProxy);

    m_BodyProxy
        ->SetRoot(Target->GetContainer())
        .SetSize(Target->GetSize())
        .SetWindow(Target);
    Target->GetContainer()->InsertControl(m_BodyProxy);

    Target->App().SetIgnoreModals(true);

    Populate();
}

void Inspector::Populate()
{
    const std::shared_ptr<Tree> TreeView = m_Tree.lock();

    const std::shared_ptr<WindowContainer> RootContainer = m_Target->GetRootContainer();
    TreeView->SetText(m_Target->GetRootContainer()->GetType());
    TreeView->SetMetaData(RootContainer.get());
    TreeView->ClearChildren();

    if (RootContainer->NumControls() > 0)
    {
        TreeView->AddChild("Pending");
    }
}

void Inspector::ParseProperty(Control const* Target)
{
    if (Target == nullptr || m_Properties.expired())
    {
        return;
    }

    Json Root(Json::Type::Object);
    Target->OnSave(Root);
    Root.Erase("Controls");
    m_Properties.lock()->Parse(Root);
}

void Inspector::Close()
{
    m_MenuBarProxy->Clear();
    m_BodyProxy->Clear();

    if (m_Target != nullptr)
    {
        m_Target->SetOnClose(nullptr);
        m_Target->SetOnLayout(nullptr);
        m_Target->GetMenuBar()->RemoveControl(m_MenuBarProxy);
        m_Target->GetContainer()->RemoveControl(m_BodyProxy);
        m_Target->App().SetIgnoreModals(false);
        m_Target = nullptr;
    }
}

void Inspector::SetEnabled(const std::shared_ptr<InspectorProxy>& Proxy, bool Enabled, const Vector2& Size)
{
    if (Enabled)
    {
        Proxy
            ->SetEnabled(true)
            .SetExpand(Expand::Both)
            .SetSize(Size);
    }
    else
    {
        Proxy
            ->Clear()
            .SetEnabled(false)
            .SetExpand(Expand::None)
            .SetSize({});
    }

    Proxy->Invalidate(InvalidateType::Both);
    m_Target->Update();
}

void Inspector::ExpandTree(const std::shared_ptr<Tree>& Root, std::vector<Control*>& Stack)
{
    Control const* MD = static_cast<Control const*>(Root->MetaData());
    Container const* MDContainer = dynamic_cast<Container const*>(MD);

    if (MDContainer == nullptr)
    {
        return;
    }

    for (const std::shared_ptr<Control>& Item : MDContainer->Controls())
    {
        if (Item.get() == Stack.back())
        {
            Root->SetExpanded(true);

            for (const std::shared_ptr<Tree>& Child : Root->Children())
            {
                if (Child->MetaData() == Stack.back())
                {
                    Stack.pop_back();

                    if (Stack.empty())
                    {
                        Child->SetSelected(true);
                        m_TreeView.lock()->SetPendingFocus(Child);
                    }
                    else
                    {
                        ExpandTree(Child, Stack);
                    }
                    break;
                }
            }
            break;
        }
    }
}

void Inspector::OnSelected(const std::weak_ptr<Control>& Selected)
{
    if (Selected.expired() || m_Tree.expired())
    {
        return;
    }

    std::shared_ptr<Control> Selected_ = Selected.lock();
    ParseProperty(Selected_.get());
    std::shared_ptr<Tree> Tree_ = m_Tree.lock();
    Tree_->SetExpandedAll(false);

    std::vector<Control*> Stack;
    Control* Anchor = Selected_.get();
    while (Anchor != Tree_->MetaData())
    {
        Stack.push_back(Anchor);
        Anchor = Anchor->GetParent();
    }

    ExpandTree(Tree_, Stack);
}

void Inspector::OnToggled(Tree& Ref)
{
    if (Ref.HasChildren())
    {
        const std::shared_ptr<Tree> First = Ref.Children()[0];

        // Children that are added that may have their own child controls will initialize
        // with a single child with no metadata. When that child is expanded, then the
        // tree will be populated with its child controls on-demand.
        if (First->MetaData() != nullptr)
        {
            return;
        }
    }

    Control const* MetaData = static_cast<Control const*>(Ref.MetaData());
    Container const* MetaDataContainer = dynamic_cast<Container const*>(MetaData);

    if (MetaDataContainer == nullptr)
    {
        return;
    }

    Ref.ClearChildren();

    for (const std::shared_ptr<Control>& Item : MetaDataContainer->Controls())
    {
        const std::shared_ptr<Tree> Child = Ref.AddChild(Item->GetType());
        Child->SetMetaData(Item.get());

        const std::shared_ptr<Container> ItemContainer = std::dynamic_pointer_cast<Container>(Item);
        if (ItemContainer && ItemContainer->NumControls() > 0)
        {
            Child->AddChild("Pending");
        }
    }
}

void Inspector::OnSelected(Tree& Ref)
{
    if (!m_BodyProxy->Enabled())
    {
        return;
    }

    Control* MetaData = static_cast<Control*>(Ref.MetaData());
    if (MetaData != nullptr)
    {
        std::shared_ptr<Control> MD = MetaData->Share();
        if (m_Target->GetMenuBar()->HasControlRecurse(MD))
        {
            m_MenuBarProxy->SetSelected(MD);
            m_BodyProxy->Clear();
        }
        else
        {
            m_BodyProxy->SetSelected(MD);
            m_MenuBarProxy->Clear();
        }

        ParseProperty(MetaData);
    }
}

void Inspector::OnHovered(Tree& Ref)
{
    if (!m_BodyProxy->Enabled())
    {
        return;
    }

    Control* MetaData = static_cast<Control*>(Ref.MetaData());
    if (MetaData != nullptr)
    {
        std::shared_ptr<Control> MD = MetaData->Share();
        if (m_Target->GetMenuBar()->HasControlRecurse(MD))
        {
            m_MenuBarProxy->SetHovered(MD);
            m_BodyProxy->Clear();
        }
        else
        {
            m_BodyProxy->SetHovered(MD);
            m_MenuBarProxy->Clear();
        }
    }
}

}
}
