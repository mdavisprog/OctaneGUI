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
