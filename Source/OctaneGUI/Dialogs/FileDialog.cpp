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
#include "../Controls/ComboBox.h"
#include "../Controls/HorizontalContainer.h"
#include "../Controls/ListBox.h"
#include "../Controls/MarginContainer.h"
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Splitter.h"
#include "../Controls/Table.h"
#include "../Controls/Text.h"
#include "../Controls/TextButton.h"
#include "../Controls/TextInput.h"
#include "../Controls/TextSelectable.h"
#include "../Controls/Tree.h"
#include "../Controls/VerticalContainer.h"
#include "../String.h"
#include "../Window.h"

#include <algorithm>

namespace OctaneGUI
{

const char* ID = "OctaneGUI.FileDialog";

void SetFileDialogData(const std::shared_ptr<Window>& Dialog, FileDialogType Type, const std::vector<FileDialogFilter>& Filters)
{
    const std::shared_ptr<FileDialog>& FD = std::static_pointer_cast<FileDialog>(Dialog->GetContainer()->Get(0));
    FD->SetType(Type);
    FD->SetFilters(Filters);
}

void FileDialog::Show(Application& App, FileDialogType Type, const std::vector<FileDialogFilter>& Filters, OnCloseSignature&& OnClose)
{
    if (!App.HasWindow(ID))
    {
        std::shared_ptr<Window> Dialog = App.NewWindow(ID, "{\"Title\": \"File Dialog\", \"Width\": 800, \"Height\": 500, \"Modal\": true}");
        std::shared_ptr<FileDialog> FD = Dialog->GetContainer()->AddControl<FileDialog>();
        FD->SetOnClose(std::move(OnClose));
    }

    const std::shared_ptr<Window>& Dialog = App.GetWindow(ID);
    Dialog->SetTitle(Type == FileDialogType::Open ? "Open File" : "Save File");
    SetFileDialogData(Dialog, Type, Filters);
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

    const std::shared_ptr<Splitter> BodySplitter = Body->AddControl<Splitter>();
    // TODO: Need to set the expansion type before setting orientation. Should look into making this order independent.
    BodySplitter->SetExpand(Expand::Both);
    BodySplitter
        ->AddContainers(2)
        .SetFit(true)
        .SetSplitterPosition(0, 0.3f)
        .SetOrientation(Orientation::Vertical);

    // The left pane that contains the directory tree.
    m_DirectoryView = BodySplitter->GetSplit(0)->AddControl<ScrollableViewControl>();
    m_DirectoryView->SetExpand(Expand::Both);
    m_DirectoryTree = m_DirectoryView->Scrollable()->AddControl<Tree>();
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
    const std::shared_ptr<ScrollableViewControl> DirectoryScrollable = BodySplitter->GetSplit(1)->AddControl<ScrollableViewControl>();
    DirectoryScrollable->SetExpand(Expand::Both);
    m_DirectoryList = DirectoryScrollable->Scrollable()->AddControl<Table>();
    m_DirectoryList
        ->AddColumn(U"Name")
        .AddColumn(U"Size")
        .SetRowSelectable(true)
        .SetOnSelected([this](Table& Ref, size_t Selected) -> void
            {
                const std::shared_ptr<Container> Cell = Ref.Cell(Selected, 0);
                const std::shared_ptr<Text>& Contents = std::static_pointer_cast<Text>(Cell->Get(0));
                m_Selected = Contents->GetText();
                m_FileName->SetText(GetWindow()->App().FS().CombinePath(m_Directory, m_Selected).c_str());
            });

    const std::shared_ptr<MarginContainer> FileInfo = Root->AddControl<MarginContainer>();
    FileInfo
        ->SetMargins({ 4.0f, 4.0f, 16.0f, 4.0f })
        .SetExpand(Expand::Width);

    const std::shared_ptr<HorizontalContainer> FileInfoLayout = FileInfo->AddControl<HorizontalContainer>();
    FileInfoLayout
        ->SetGrow(Grow::End)
        ->SetExpand(Expand::Width);

    m_FileName = FileInfoLayout->AddControl<TextInput>();
    m_FileName
        ->SetReadOnly(true)
        .SetMultiline(false)
        .SetExpand(Expand::Width);

    m_FilterBox = FileInfoLayout->AddControl<ComboBox>();
    m_FilterBox->SetWidth(200.0f);

    // The confirm and cancel button container which should be right-aligned.
    const std::shared_ptr<MarginContainer> Buttons = Root->AddControl<MarginContainer>();
    Buttons
        ->SetMargins({ 4.0f, 4.0f, 16.0f, 12.0f })
        .SetExpand(Expand::Width);

    const std::shared_ptr<HorizontalContainer> ButtonsLayout = Buttons->AddControl<HorizontalContainer>();
    ButtonsLayout
        ->SetSpacing({ 12.0f, 4.0f })
        ->SetGrow(Grow::End)
        ->SetExpand(Expand::Width);

    m_ConfirmButton = ButtonsLayout->AddControl<TextButton>();
    m_ConfirmButton
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

FileDialog& FileDialog::SetType(FileDialogType Type)
{
    m_Type = Type;
    if (m_Type == FileDialogType::Open)
    {
        m_ConfirmButton->SetText("Open");
    }
    else
    {
        m_ConfirmButton->SetText("Save");
    }
    return *this;
}

FileDialog& FileDialog::SetFilters(const std::vector<FileDialogFilter>& Filters)
{
    m_Filters = std::move(Filters);
    if (m_Filters.empty())
    {
        m_Filters.push_back({ { U"*" }, U"All Files" });
    }

    m_FilterBox->Clear();
    for (const FileDialogFilter& Filter : m_Filters)
    {
        std::u32string Display = Filter.Description + U" (";
        for (size_t I = 0; I < Filter.Extensions.size(); I++)
        {
            Display += Filter.Extensions[I];
            if (I < Filter.Extensions.size() - 1)
            {
                Display += U";";
            }
        }
        Display += U")";
        m_FilterBox->AddItem(Display.c_str());
    }
    m_FilterBox->SetSelectedIndex(0);
    PopulateList();

    return *this;
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

    std::vector<std::u32string> Stack;
    const std::u32string RootDir = FS.RootDirectory(m_Directory);
    std::u32string Current = m_Directory;
    while (!Current.empty())
    {
        const std::u32string Name = FS.CurrentDirectory(Current);
        Stack.insert(Stack.begin(), Name);

        const std::u32string Parent = FS.ParentDirectory(Current);
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
    std::u32string Path = Stack.front();
    for (size_t I = 1; I < Stack.size(); I++)
    {
        PopulateChildren(Root, Path);

        const std::u32string& Name = Stack[I];
        const std::shared_ptr<Tree> Child = Root->GetChild(Name.c_str());
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
    m_DirectoryView->SetPendingFocus(Root);
}

void FileDialog::PopulateChildren(const std::shared_ptr<Tree>& Parent, const std::u32string& Directory) const
{
    const FileSystem& FS = GetWindow()->App().FS();

    std::vector<FileSystem::DirectoryItem> Items = FS.DirectoryItems(Directory.c_str());
    std::sort(Items.begin(), Items.end(), [](const FileSystem::DirectoryItem& A, const FileSystem::DirectoryItem& B) -> bool
        {
            return A.FileName < B.FileName;
        });
    for (const FileSystem::DirectoryItem& Item : Items)
    {
        const std::u32string Path = FS.CombinePath(Directory, Item.FileName);

        if (FS.IsDirectory(Path))
        {
            const std::shared_ptr<Tree> Child = Parent->AddChild(Item.FileName.c_str());

            if (HasDirectories(Path.c_str()))
            {
                Child->AddChild("");
            }
        }
    }
}

void FileDialog::PopulateList()
{
    m_DirectoryList->ClearRows();

    FileDialogFilter Filter {};
    const int Index = m_FilterBox->SelectedIndex();
    if (Index >= 0)
    {
        Filter = m_Filters[Index];
    }

    const FileSystem& FS = GetWindow()->App().FS();

    std::vector<FileSystem::DirectoryItem> Items = FS.DirectoryItems(m_Directory);
    std::sort(Items.begin(), Items.end(), [](const FileSystem::DirectoryItem& A, const FileSystem::DirectoryItem& B) -> bool
        {
            return A.FileName < B.FileName;
        });
    for (const FileSystem::DirectoryItem& Item : Items)
    {
        std::u32string Extension = String::ToLower(FS.Extension(Item.FileName));
        if (!Extension.empty())
        {
            Extension = Extension.substr(1);
        }

        bool IsValidExtension = false;
        for (const std::u32string& Test : Filter.Extensions)
        {
            if (Test == U"*" || Extension == String::ToLower(Test))
            {
                IsValidExtension = true;
                break;
            }
        }

        if (IsValidExtension)
        {
            AddListItem(Item.FileName.c_str(), (size_t)Item.FileSize);
        }
    }
}

void FileDialog::AddListItem(const char32_t* Name, size_t Size)
{
    const size_t Row = m_DirectoryList->Rows();
    m_DirectoryList->AddRow();

    // Set name
    m_DirectoryList
        ->Cell(Row, 0)
        ->AddControl<Text>()
        ->SetText(Name);

    // Set size
    m_DirectoryList
        ->Cell(Row, 1)
        ->AddControl<Text>()
        ->SetText(std::to_string(Size).c_str());
}

void FileDialog::Close(bool Success)
{
    if (!Success)
    {
        m_FileName->SetText(U"");
    }

    if (m_OnClose)
    {
        m_OnClose(m_Type, m_FileName->GetText());
    }

    GetWindow()->App().CloseWindow(ID);
}

std::u32string FileDialog::Path(const std::shared_ptr<Tree>& Item) const
{
    std::u32string Result;

    std::vector<std::u32string> Stack;
    std::shared_ptr<Tree> Parent = Item;
    while (Parent)
    {
        const std::u32string Name = Parent->GetText();
        Stack.insert(Stack.begin(), Name);
        Parent = Parent->ParentTree().lock();
    }

    for (const std::u32string& Sub : Stack)
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

bool FileDialog::HasDirectories(const std::u32string& Directory) const
{
    const FileSystem& FS = GetWindow()->App().FS();
    const std::vector<FileSystem::DirectoryItem> Items = FS.DirectoryItems(Directory.c_str());

    for (const FileSystem::DirectoryItem& Item : Items)
    {
        const std::u32string Path { FS.CombinePath(Directory, Item.FileName) };
        if (FS.IsDirectory(Path))
        {
            return true;
        }
    }

    return false;
}

}
