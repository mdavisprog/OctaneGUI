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
#include "Windowing.h"

#include <climits>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#include <windowsx.h>

namespace Frontend
{
namespace Windowing
{

OnHitTestSignature OnHitTest { nullptr };
LONG_PTR OriginalEvent { 0 };

LRESULT CALLBACK OnEvent(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch (Message)
    {
    case WM_NCHITTEST:
    {
        POINT Point { GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam) };
        if (ScreenToClient(Handle, &Point))
        {
            if (OnHitTest != nullptr)
            {
                HitTestResult Result = OnHitTest((void*)Handle, { (float)Point.x, (float)Point.y });
                switch (Result)
                {
                case HitTestResult::Draggable: return HTCAPTION;
                case HitTestResult::TopLeft: return HTTOPLEFT;
                case HitTestResult::Top: return HTTOP;
                case HitTestResult::TopRight: return HTTOPRIGHT;
                case HitTestResult::Right: return HTRIGHT;
                case HitTestResult::BottomRight: return HTBOTTOMRIGHT;
                case HitTestResult::Bottom: return HTBOTTOM;
                case HitTestResult::BottomLeft: return HTBOTTOMLEFT;
                case HitTestResult::Left: return HTLEFT;
                case HitTestResult::Normal:
                default: break;
                }
            }

            return HTCLIENT;
        }
    }
    break;

    default: break;
    }

    if (OriginalEvent != 0)
    {
        return CallWindowProc(reinterpret_cast<WNDPROC>(OriginalEvent), Handle, Message, WParam, LParam);
    }

    return DefWindowProc(Handle, Message, WParam, LParam);
}

void RegisterWndProc(HWND Handle)
{
    LONG_PTR Proc = GetWindowLongPtr(Handle, GWLP_WNDPROC);
    if (reinterpret_cast<WNDPROC>(Proc) != OnEvent)
    {
        OriginalEvent = SetWindowLongPtr(Handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&OnEvent));
    }
}

void ShowMinimize(void* Handle, bool Show)
{
    HWND WinHandle = (HWND)Handle;
    LONG_PTR Flags = GetWindowLongPtr(WinHandle, GWL_STYLE);
    Flags = Show ? Flags | WS_MINIMIZEBOX : Flags & ~WS_MINIMIZEBOX;
    SetWindowLongPtr(WinHandle, GWL_STYLE, Flags);
}

void Minimize(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    ShowWindow(WinHandle, SW_MINIMIZE);
}

void SetOnHitTest(void* Handle, OnHitTestSignature&& Fn)
{
    HWND WinHandle = (HWND)Handle;
    RegisterWndProc(WinHandle);
    OnHitTest = std::move(Fn);
}

void Focus(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    SetFocus(WinHandle);
}

std::u32string FileDialog(OctaneGUI::FileDialogType Type, const std::vector<OctaneGUI::FileDialogFilter>& Filters, void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    const bool IsOpen = Type == OctaneGUI::FileDialogType::Open;

    std::wstring FileName;
    FileName.resize(USHRT_MAX);
    FileName[0] = '\0';

    std::wstring WFilters;
    for (const OctaneGUI::FileDialogFilter& Filter : Filters)
    {
        WFilters += OctaneGUI::String::ToWide(Filter.Description);
        WFilters.push_back(L'\0');

        std::wstring Extensions;
        for (size_t I = 0; I < Filter.Extensions.size(); I++)
        {
            const std::u32string& Extension = Filter.Extensions[I];
            Extensions += L"*.";
            Extensions += OctaneGUI::String::ToWide(Extension);
            if (I < Filter.Extensions.size() - 1)
            {
                Extensions += L";";
            }
        }

        if (!Extensions.empty())
        {
            WFilters += Extensions;
        }

        WFilters.push_back(L'\0');
    }

    WFilters.push_back(L'\0');
    WFilters.push_back(L'\0');

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
    if (!WFilters.empty())
    {
        OpenFileName.lpstrFilter = &WFilters[0];
        OpenFileName.nFilterIndex = 1;
    }

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

namespace Windows
{

OctaneGUI::Rect GetWorkingArea(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    HMONITOR Monitor = MonitorFromWindow(WinHandle, MONITOR_DEFAULTTONEAREST);
    MONITORINFO Info {};
    Info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(Monitor, &Info);
    return { (float)Info.rcWork.left, (float)Info.rcWork.top, (float)Info.rcWork.right, (float)Info.rcWork.bottom};
}

void MaximizeWindow(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    ShowWindow(WinHandle, SW_MAXIMIZE);
}

void RestoreWindow(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    ShowWindow(WinHandle, SW_NORMAL);
}

void SetEnabled(void* Handle, bool Enabled)
{
    HWND WinHandle = (HWND)Handle;
    EnableWindow(WinHandle, Enabled ? TRUE : FALSE);
}

void SetAlwaysOnTop(void* Handle)
{
    HWND WinHandle = (HWND)Handle;
    SetWindowPos(WinHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

}

}
}
