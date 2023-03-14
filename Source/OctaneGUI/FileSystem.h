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

#pragma once

#include "Dialogs/FileDialogType.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Application;
struct FileDialogFilter;

class FileSystem
{
public:
    struct DirectoryItem
    {
    public:
        std::u32string FileName {};
        uintmax_t FileSize { 0 };
    };

    typedef std::function<std::u32string(FileDialogType, const std::vector<FileDialogFilter>&)> OnFileDialogSignature;
    typedef std::function<void(FileDialogType, const std::u32string&)> OnFileDialogResultSignature;

    static std::string Extension(const std::string& Location);

    FileSystem(Application& App);
    ~FileSystem();

    FileSystem& SetUseSystemFileDialog(bool UseSystemFileDialog);
    bool UseSystemFileDialog() const;

    std::u32string CurrentDirectory() const;
    std::u32string CurrentDirectory(const std::u32string& Location) const;
    std::u32string ParentDirectory(const std::u32string& Location) const;
    std::u32string RootDirectory(const std::u32string& Location) const;
    std::u32string CombinePath(const std::u32string& Left, const std::u32string& Right) const;
    std::u32string Extension(const std::u32string& Location) const;
    std::u32string FileName(const std::u32string& Location) const;
    std::vector<DirectoryItem> DirectoryItems(const std::u32string& Location) const;

    std::string LoadContents(const std::string& Location) const;
    std::string LoadContents(const std::u32string& Location) const;

    bool WriteContents(const std::string& Location, const std::string& Contents) const;
    bool WriteContents(const std::u32string& Location, const std::u32string& Contents) const;

    bool IsFile(const std::u32string& Location) const;
    bool IsDirectory(const std::u32string& Location) const;
    bool IsEmpty(const std::u32string& Location) const;
    bool Exists(const std::u32string& Location) const;

    void FileDialog(FileDialogType Type, const std::vector<FileDialogFilter>& Filters = {}) const;

    FileSystem& SetOnFileDialog(OnFileDialogSignature&& Fn);
    FileSystem& SetOnFileDialogResult(OnFileDialogResultSignature&& Fn);

private:
    Application& m_Application;
    bool m_UseSystemFileDialog { false };

    OnFileDialogSignature m_OnFileDialog { nullptr };
    OnFileDialogResultSignature m_OnFileDialogResult { nullptr };
};

}
