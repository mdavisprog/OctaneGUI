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

std::string FileSystem::CurrentDirectory() const
{
    return std::filesystem::current_path().u8string();
}

std::string FileSystem::CurrentDirectory(const std::string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.filename().u8string();
}

std::string FileSystem::ParentDirectory(const std::string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.parent_path().u8string();
}

std::string FileSystem::RootDirectory(const std::string& Location) const
{
    const std::filesystem::path Path { Location };
    return Path.root_path().u8string();
}

std::string FileSystem::CombinePath(const std::string& Left, const std::string& Right) const
{
    std::filesystem::path Path { Left };
    Path.append(Right);
    return Path.u8string();
}

std::vector<std::string> FileSystem::DirectoryItems(const char* Location) const
{
    std::vector<std::string> Result;

    const std::filesystem::directory_options Options { std::filesystem::directory_options::skip_permission_denied };
    std::error_code Error;
    for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(Location, Options, Error))
    {
        Result.push_back(Entry.path().filename().u8string());
    }

    return Result;
}

bool FileSystem::IsFile(const char* Location) const
{
    return std::filesystem::is_regular_file(Location);
}

bool FileSystem::IsDirectory(const char* Location) const
{
    return std::filesystem::is_directory(Location);
}

bool FileSystem::IsEmpty(const char* Location) const
{
    std::error_code Error;
    bool Result = std::filesystem::is_empty(Location, Error);
    return Result && Error.value() == 0;
}

void FileSystem::OpenFileDialog() const
{
    if (m_UseSystemFileDialog)
    {
        if (m_OnFileDialog)
        {
            std::string Result = m_OnFileDialog();

            if (m_OnFileDialogResult)
            {
                m_OnFileDialogResult(Result);
            }
        }
    }
}

FileSystem& FileSystem::SetOnFileDialog(OnGetStringSignature&& Fn)
{
    m_OnFileDialog = std::move(Fn);
    return *this;
}

FileSystem& FileSystem::SetOnFileDialogResult(OnStringResultSignature&& Fn)
{
    m_OnFileDialogResult = std::move(Fn);
    return *this;
}

}
