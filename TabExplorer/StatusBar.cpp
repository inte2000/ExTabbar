// TabExplorerBHO.cpp : Implementation of CTabExplorerBHO

#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include <Ntquery.h>
#include "StatusBar.h"


extern BOOL g_bForceRefreshWin7;
extern BOOL g_bShowFreespaceInStatusBar;

void CStatusBar::ForceRefresh()
{
    if (m_bForceRefresh)
    {
        m_bForceRefresh = false;
        if (::SendMessage(m_hStatusbarWnd, SB_GETPARTS, 0, 0) <= 1)
            ::PostMessage(::GetParent(m_hStatusbarWnd), WM_COMMAND, 41504, 0); // Refresh command
    }
}

__int64 CStatusBar::GetCurrentSelectedItemSize()
{
    if (m_spShellBrowser == nullptr)
        return 0;
    
    __int64 size = 0;

    CComPtr<IShellView> pView;
    if (SUCCEEDED(m_spShellBrowser->QueryActiveShellView(&pView)))
    {
        CComQIPtr<IFolderView> pView2 = pView;
        CComPtr<IPersistFolder2> pFolder;
        LPITEMIDLIST pidl;
        if (pView2 && SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)& pFolder)) && SUCCEEDED(pFolder->GetCurFolder(&pidl)))
        {
            CComQIPtr<IShellFolder2> pFolder2 = pFolder;
            UINT type = SVGIO_SELECTION;
            int count;
            if ((FAILED(pView2->ItemCount(SVGIO_SELECTION, &count)) || count == 0))
                type = SVGIO_ALLVIEW;
            CComPtr<IEnumIDList> pEnum;
            if (SUCCEEDED(pView2->Items(type, IID_IEnumIDList, (void**)& pEnum)) && pEnum)
            {
                PITEMID_CHILD child;
                SHCOLUMNID column = { PSGUID_STORAGE,PID_STG_SIZE };
                while (pEnum->Next(1, &child, NULL) == S_OK)
                {
                    CComVariant var;
                    if (SUCCEEDED(pFolder2->GetDetailsEx(child, &column, &var)) && var.vt == VT_UI8)
                    {
                        if (size < 0)
                            size = var.ullVal;
                        else
                            size += var.ullVal;
                    }
                    ILFree(child);
                }
            }
            ILFree(pidl);
        }
    }

    return size;
}

void CStatusBar::ResetStatus()
{
    if (m_bResetStatus && SendMessage(m_hStatusbarWnd, SB_GETPARTS, 0, 0) <= 1)
    {
        LogInfo(_T("CStatusBar::ResetStatus hack statusbar space for win 7"));
        // HACK! there is a bug in Win7 and when the Explorer window is created it doesn't correctly
        // initialize the status bar to have 3 parts. as soon as the user resizes the window the
        // 3 parts appear. so here we resize the parent of the status bar to create the 3 parts.
        HWND parent = ::GetParent(m_hStatusbarWnd);
        RECT rc;
        ::GetWindowRect(parent, &rc);
        ::SetWindowPos(parent, NULL, 0, 0, rc.right - rc.left + 1, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);
        ::SetWindowPos(parent, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);
        // the first time the status text is set it is too early. so we do this until we get at lest 2 parts
        if (::SendMessage(m_hStatusbarWnd, SB_GETPARTS, 0, 0) > 1)
            m_bResetStatus = false;
    }
}

BOOL CStatusBar::GetFreeSpaceOfCurFolderLocation(ULARGE_INTEGER& size)
{
    if (m_spShellBrowser == nullptr)
        return FALSE;

    BOOL bSuccess = FALSE;
    CComPtr<IShellView> pView;
    if (SUCCEEDED(m_spShellBrowser->QueryActiveShellView(&pView)))
    {
        CComQIPtr<IFolderView> pView2 = pView;
        CComPtr<IPersistFolder2> pFolder;
        if (pView2 && SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)& pFolder)))
        {
            LPITEMIDLIST pidl;
            if (SUCCEEDED(pFolder->GetCurFolder(&pidl)))
            {
                TCHAR szPathname[MAX_PATH];
                if (SHGetPathFromIDList(pidl, szPathname))
                {
                    ULARGE_INTEGER size;
                    if (GetDiskFreeSpaceEx(szPathname, NULL, NULL, &size))
                    {
                        bSuccess = TRUE;
                    }
                }
                ILFree(pidl);
            }
        }
    }

    return bSuccess;
}



LRESULT CALLBACK CStatusBar::StatusbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CStatusBar* pStatusbar = (CStatusBar*)dwRefData;
    ATLASSERT(pStatusbar != nullptr);

    wchar_t buf[1024];
    if (uMsg == WM_PAINT)
    {
        // sometimes Explorer doesn't fully initialize the status bar on Windows 7 and leaves it with 1 part
        // in such case force the view to refresh after the status bar is fully visible
        pStatusbar->ForceRefresh();
    }
    if (uMsg == SB_SETTEXT && LOWORD(wParam) == 0)
    {
        // when the text of the first part is changing
        // recalculate the selection size on a timer. this way if the status text is changed frequently
        // the recalculation will not happen every time
        SetTimer(hWnd, uIdSubclass, 10, NULL);

        if (IsWindows7())
        {
            pStatusbar->ResetStatus();

            LogInfo(_T("ExplorerWindow hack statusbar calculate free space"));
            // find the current folder and show the free space of the drive containing the current folder
            // also works for network locations
            ULARGE_INTEGER size = { 0 };
            if (pStatusbar->GetFreeSpaceOfCurFolderLocation(size))
            {
                const wchar_t* text = (wchar_t*)lParam;
                wchar_t str[100];
                StrFormatByteSize64(size.QuadPart, str, _countof(str));
                swprintf_s(buf, _T("%s (Disk free space: %s)"), text, str);
                lParam = (LPARAM)buf;
            }
        }
    }
    if (uMsg == SB_SETTEXT && LOWORD(wParam) == 1)
    {
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == uIdSubclass)
    {
        // recalculate the total size of the selected files and show it in part 2 of the status bar
        KillTimer(hWnd, wParam);
        __int64 size = pStatusbar->GetCurrentSelectedItemSize();
        if (size >= 0)
        {
            // format the file size as KB, MB, etc
            StrFormatByteSize64(size, buf, _countof(buf));
        }
        else
            buf[0] = 0;
        DefSubclassProc(hWnd, SB_SETTEXT, 1, (LPARAM)buf);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CStatusBar::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hStatusbarWnd)
{
    m_spShellBrowser = spShellBrowser;
    m_hStatusbarWnd = hStatusbarWnd;
    
    LogInfo(_T("CStatusBar subclass statusbar... "));
    ::SetWindowSubclass(m_hStatusbarWnd, StatusbarSubclassProc, m_StatusbarSubclassId, (DWORD_PTR)this);
    m_bForceRefresh = (IsWindows7() && g_bForceRefreshWin7);

    return TRUE;


    m_bStatusbarSubclass = TRUE;
    return m_bStatusbarSubclass;
}

void CStatusBar::Unintialize()
{
    if(m_bStatusbarSubclass)
    {
        ::RemoveWindowSubclass(m_hStatusbarWnd, StatusbarSubclassProc, m_StatusbarSubclassId);
        m_bStatusbarSubclass = FALSE;
    }
}


