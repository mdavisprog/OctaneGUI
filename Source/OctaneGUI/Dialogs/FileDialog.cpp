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

#include "FileDialog.h"
#include "../Application.h"
#include "../Controls/HorizontalContainer.h"
#include "../Controls/ListBox.h"
#include "../Controls/MarginContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Splitter.h"
#include "../Controls/TextButton.h"
#include "../Controls/TextSelectable.h"
#include "../Controls/Tree.h"
#include "../Controls/VerticalContainer.h"
#include "../String.h"
#include "../Window.h"

namespace OctaneGUI
{

const char* ID = "OctaneGUI.FileDialog";

void FileDialog::Show(Application& App, OnCloseSignature&& OnClose)
{
    if (!App.HasWindow(ID))
    {
        std::shared_ptr<Window> Dialog = App.NewWindow(ID, "{\"Title\": \"File Dialog\", \"Width\": 800, \"Height\": 500, \"Modal\": true}");
        std::shared_ptr<FileDialog> FD = Dialog->GetContainer()->AddControl<FileDialog>();
        FD->SetOnClose(std::move(OnClose));
    }

    App.DisplayWindow(ID);
}

FileDialog::FileDialog(Window* InWindow)
    : Container(InWindow)
{
    SetExpand(Expand::Both);

    const std::shared_ptr<Panel> Background = AddControl<Panel>();
    Background->SetExpand(Expand::Both);

    // The root list of all controls in a VerticalContainer.
    const std::shared_ptr<VerticalContainer> Root = AddControl<VerticalContainer>();
    Root->SetExpand(Expand::Both);

    // The body of the dialog, which will contain the directory tree and directory list.
    const std::shared_ptr<MarginContainer> Body = Root->AddControl<MarginContainer>();
    Body
        ->SetMargins({ 4.0f, 0.0f, 4.0f, 0.0f })
        .SetExpand(Expand::Both);

    const std::shared_ptr<Panel> BodyBackground = Body->AddControl<Panel>();
    BodyBackground
        ->SetProperty(ThemeProperties::Panel, Color::Black)
        .SetExpand(Expand::Both);

    const std::shared_ptr<Splitter> BodySplitter = Body->AddControl<Splitter>();
    BodySplitter
        ->SetOrientation(Orientation::Vertical)
        .SetSplitterPosition(0.3f)
        .SetExpand(Expand::Both);

    // The left pane that contains the directory tree.
    const std::shared_ptr<ScrollableViewControl> DirectoryTreeView = BodySplitter->First()->AddControl<ScrollableViewControl>();
    DirectoryTreeView->SetExpand(Expand::Both);
    m_DirectoryTree = DirectoryTreeView->Scrollable()->AddControl<Tree>();
    m_DirectoryTree
        ->SetOnSelected([this](Tree& Selected) -> void
            {
                m_Directory = Path(Selected.TShare<Tree>());
                PopulateList();
            })
        .SetOnToggled([this](Tree& Toggled) -> void
            {
                const std::shared_ptr<Tree> Item = Toggled.TShare<Tree>();
                if (IsEmpty(Item))
                {
                    Item->ClearChildren();
                    PopulateChildren(Item, Path(Item));
                }
            });

    // The right pane that contains the list of files in the selected directory.
    m_DirectoryList = BodySplitter->Second()->AddControl<ListBox>();
    m_DirectoryList
        ->SetOnSelect([this](int, std::weak_ptr<Control> Item) -> void
            {
                if (Item.expired())
                {
                    return;
                }

                const std::shared_ptr<TextSelectable> Selected = std::static_pointer_cast<TextSelectable>(Item.lock());
                m_Selected = String::ToMultiByte(Selected->GetText());
            })
        .SetExpand(Expand::Both);

    // The confirm and cancel button container which should be right-aligned.
    const std::shared_ptr<MarginContainer> Buttons = Root->AddControl<MarginContainer>();
    Buttons
        ->SetMargins({ 4.0f, 4.0f, 4.0f, 4.0f })
        .SetExpand(Expand::Width);

    const std::shared_ptr<HorizontalContainer> ButtonsLayout = Buttons->AddControl<HorizontalContainer>();
    ButtonsLayout
        ->SetGrow(Grow::End)
        ->SetExpand(Expand::Width);

    const std::shared_ptr<TextButton> OpenBtn = ButtonsLayout->AddControl<TextButton>();
    OpenBtn
        ->SetText("Open")
        ->SetOnClicked([this](const Button&) -> void
            {
                Close(true);
            });

    const std::shared_ptr<TextButton> CancelBtn = ButtonsLayout->AddControl<TextButton>();
    CancelBtn
        ->SetText("Cancel")
        ->SetOnClicked([this](const Button&) -> void
            {
                Close(false);
            });

    m_Directory = GetWindow()->App().FS().CurrentDirectory();
    PopulateTree();
}

FileDialog& FileDialog::SetOnClose(OnCloseSignature&& Fn)
{
    m_OnClose = std::move(Fn);
    return *this;
}

void FileDialog::PopulateTree()
{
    m_DirectoryTree->ClearChildren();

    const FileSystem& FS = GetWindow()->App().FS();

    std::vector<std::string> Stack;
    const std::string RootDir = FS.RootDirectory(m_Directory);
    std::string Current = m_Directory;
    while (!Current.empty())
    {
        const std::string Name = FS.CurrentDirectory(Current);
        Stack.insert(Stack.begin(), Name);

        const std::string Parent = FS.ParentDirectory(Current);
        if (Parent == RootDir)
        {
            Stack.insert(Stack.begin(), Parent);
            Current.clear();
        }
        else
        {
            Current = Parent;
        }
    }

    if (Stack.empty())
    {
        return;
    }

    std::shared_ptr<Tree> Root = m_DirectoryTree;
    Root->SetText(Stack.front().c_str());
    std::string Path = Stack.front();
    for (size_t I = 1; I < Stack.size(); I++)
    {
        PopulateChildren(Root, Path);

        const std::string& Name = Stack[I];
        const std::shared_ptr<Tree> Child = Root->GetChild(String::ToUTF32(Name).c_str());
        if (Child)
        {
            Root->SetExpanded(true);
            Root = Child;

            if (IsEmpty(Root))
            {
                Root->ClearChildren();
            }
        }
        Path = FS.CombinePath(Path, Name);
    }

    PopulateChildren(Root, Path);
    Root->SetSelected(true);
}

void FileDialog::PopulateChildren(const std::shared_ptr<Tree>& Parent, const std::string& Directory) const
{
    const FileSystem& FS = GetWindow()->App().FS();

    const std::vector<std::string> Items = FS.DirectoryItems(Directory.c_str());
    for (const std::string& Item : Items)
    {
        const std::string Path = FS.CombinePath(Directory, Item);

        if (FS.IsDirectory(Path.c_str()))
        {
            const std::shared_ptr<Tree> Child = Parent->AddChild(Item.c_str());

            if (HasDirectories(Path.c_str()))
            {
                Child->AddChild("");
            }
        }
    }
}

void FileDialog::PopulateList()
{
    m_DirectoryList->ClearItems();

    const std::vector<std::string> Items = GetWindow()->App().FS().DirectoryItems(m_Directory.c_str());

    for (const std::string& Item : Items)
    {
        m_DirectoryList->AddItem<TextSelectable>()->SetText(Item.c_str());
    }
}

void FileDialog::Close(bool Success)
{
    std::string Result;

    if (!m_Selected.empty())
    {
        Result = Success
            ? GetWindow()->App().FS().CombinePath(m_Directory, m_Selected)
            : "";
    }

    if (m_OnClose)
    {
        m_OnClose(Result);
    }

    GetWindow()->App().CloseWindow(ID);
}

std::string FileDialog::Path(const std::shared_ptr<Tree>& Item) const
{
    std::string Result;

    std::vector<std::string> Stack;
    std::shared_ptr<Tree> Parent = Item;
    while (Parent)
    {
        const std::string Name = String::ToMultiByte(Parent->GetText());
        Stack.insert(Stack.begin(), Name);
        Parent = Parent->ParentTree().lock();
    }

    for (const std::string& Sub : Stack)
    {
        if (Result.empty())
        {
            Result = Sub;
        }
        else
        {
            Result = GetWindow()->App().FS().CombinePath(Result, Sub);
        }
    }

    return Result;
}

bool FileDialog::IsEmpty(const std::shared_ptr<Tree>& Item) const
{
    const std::vector<std::shared_ptr<Tree>> Children = Item->Children();
    if (Children.size() == 0)
    {
        return true;
    }

    if (Children.size() > 1)
    {
        return false;
    }

    const std::u32string Text = Children[0]->GetText();
    if (!Text.empty())
    {
        return false;
    }

    return true;
}

bool FileDialog::HasDirectories(const std::string& Directory) const
{
    const FileSystem& FS = GetWindow()->App().FS();
    const std::vector<std::string> Items = FS.DirectoryItems(Directory.c_str());

    for (const std::string& Item : Items)
    {
        const std::string Path { FS.CombinePath(Directory, Item) };
        if (FS.IsDirectory(Path.c_str()))
        {
            return true;
        }
    }

    return false;
}

}
