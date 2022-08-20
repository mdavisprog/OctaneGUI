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

#pragma once

#include "../Controls/Container.h"

namespace OctaneGUI
{

class Application;
class ListBox;
class Tree;

class FileDialog : public Container
{
    CLASS(FileDialog)

public:
    typedef std::function<void(const std::string&)> OnCloseSignature;

    static void Show(Application& App, OnCloseSignature&& OnClose);

    FileDialog(Window* InWindow);

    FileDialog& SetOnClose(OnCloseSignature&& Fn);

private:
    void PopulateTree();
    void PopulateChildren(const std::shared_ptr<Tree>& Parent, const std::string& Directory) const;
    void PopulateList();
    void Close(bool Success);

    std::string Path(const std::shared_ptr<Tree>& Item) const;
    bool IsEmpty(const std::shared_ptr<Tree>& Item) const;
    bool HasDirectories(const std::string& Directory) const;

    std::string m_Directory {};
    std::string m_Selected {};

    std::shared_ptr<Tree> m_DirectoryTree { nullptr };
    std::shared_ptr<ListBox> m_DirectoryList { nullptr };

    OnCloseSignature m_OnClose { nullptr };
};

}
