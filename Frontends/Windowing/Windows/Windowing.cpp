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

#include "../Windowing.h"
#include "OctaneGUI/OctaneGUI.h"

#include <climits>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>

namespace Frontend
{
namespace Windowing
{

void SetMovable(void* Handle, bool Movable)
{
}

void SetEnabled(void* Handle, bool Enabled)
{
    HWND WinHandle = (HWND)Handle;
    EnableWindow(WinHandle, Enabled ? TRUE : FALSE);
}

void Focus(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    SetFocus(WinHandle);
}

std::u32string FileDialog(OctaneGUI::FileDialogType Type, void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    const bool IsOpen = Type == OctaneGUI::FileDialogType::Open;

    std::wstring FileName;
    FileName.resize(USHRT_MAX);
    FileName[0] = '\0';

    OPENFILENAMEW OpenFileName {};
    ZeroMemory(&OpenFileName, sizeof(OPENFILENAMEW));
    OpenFileName.lStructSize = sizeof(OPENFILENAMEW);
    OpenFileName.hwndOwner = WinHandle;
    OpenFileName.Flags = OFN_CREATEPROMPT | OFN_FILEMUSTEXIST;
    if (!IsOpen)
    {
        OpenFileName.Flags |= OFN_OVERWRITEPROMPT;
    }
    OpenFileName.lpstrFile = FileName.data();
    OpenFileName.nMaxFile = FileName.size();

    std::u32string Result;
    BOOL FileNameResult = FALSE;
    if (IsOpen)
    {
        FileNameResult = GetOpenFileNameW(&OpenFileName);
    }
    else
    {
        FileNameResult = GetSaveFileNameW(&OpenFileName);
    }

    if (FileNameResult == TRUE)
    {
        Result = OctaneGUI::String::ToUTF32(FileName.c_str());
    }

    return Result;
}

}
}
