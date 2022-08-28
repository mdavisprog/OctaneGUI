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

#include "FileSystem.h"
#include "Dialogs/FileDialog.h"

#include <filesystem>
#include <system_error>

namespace OctaneGUI
{

FileSystem::FileSystem(Application& App)
    : m_Application(App)
{
}

FileSystem::~FileSystem()
{
}

FileSystem& FileSystem::SetUseSystemFileDialog(bool UseSystemFileDialog)
{
    m_UseSystemFileDialog = UseSystemFileDialog;
    return *this;
}

bool FileSystem::UseSystemFileDialog() const
{
    return m_UseSystemFileDialog;
}

std::u32string FileSystem::CurrentDirectory() const
{
    return std::filesystem::current_path().u32string();
}

std::u32string FileSystem::CurrentDirectory(const std::u32string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.filename().u32string();
}

std::u32string FileSystem::ParentDirectory(const std::u32string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.parent_path().u32string();
}

std::u32string FileSystem::RootDirectory(const std::u32string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.root_path().u32string();
}

std::u32string FileSystem::CombinePath(const std::u32string& Left, const std::u32string& Right) const
{
    std::filesystem::path Path { Left };
    Path.append(Right);
    return Path.u32string();
}

std::u32string FileSystem::Extension(const std::u32string& Location) const
{
    std::filesystem::path Path { Location };
    return Path.extension().u32string();
}

std::vector<std::u32string> FileSystem::DirectoryItems(const std::u32string& Location) const
{
    std::vector<std::u32string> Result;

    const std::filesystem::directory_options Options { std::filesystem::directory_options::skip_permission_denied };
    std::error_code Error;
    for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(Location, Options, Error))
    {
        Result.push_back(Entry.path().filename().u32string());
    }

    return Result;
}

bool FileSystem::IsFile(const std::u32string& Location) const
{
    return std::filesystem::is_regular_file(Location);
}

bool FileSystem::IsDirectory(const std::u32string& Location) const
{
    return std::filesystem::is_directory(Location);
}

bool FileSystem::IsEmpty(const std::u32string& Location) const
{
    std::error_code Error;
    bool Result = std::filesystem::is_empty(Location, Error);
    return Result && Error.value() == 0;
}

void FileSystem::FileDialog(FileDialogType Type, const std::vector<FileDialogFilter>& Filters) const
{
    if (m_UseSystemFileDialog)
    {
        if (m_OnFileDialog)
        {
            std::u32string Result = m_OnFileDialog(Type);

            if (m_OnFileDialogResult)
            {
                m_OnFileDialogResult(Type, Result);
            }
        }
    }
    else
    {
        // TODO: Look into not having the 'FileDialog' as a dependency for FileSystem.
        FileDialog::Show(m_Application, Type, Filters, [this](FileDialogType Type, const std::u32string& FileName) -> void
            {
                if (m_OnFileDialogResult)
                {
                    m_OnFileDialogResult(Type, FileName);
                }
            });
    }
}

FileSystem& FileSystem::SetOnFileDialog(OnFileDialogSignature&& Fn)
{
    m_OnFileDialog = std::move(Fn);
    return *this;
}

FileSystem& FileSystem::SetOnFileDialogResult(OnFileDialogResultSignature&& Fn)
{
    m_OnFileDialogResult = std::move(Fn);
    return *this;
}

}
