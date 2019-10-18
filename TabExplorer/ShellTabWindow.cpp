// ShellTabWindow.cpp : Implementation of CShellTabWindow

#include "pch.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "ShellTabWindow.h"
#include "ExplorerWindow.h"


LRESULT CALLBACK CShellTabWindow::ShellTabWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CShellTabWindow* pThisView = (CShellTabWindow*)dwRefData;
    ATLASSERT(pThisView != nullptr);
 
    if (uMsg == 1792)
    {
        int sss = 0;
    }
    if (uMsg == WM_WINDOWPOSCHANGING)
    {
        LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        pThisView->OnWindowPosChanging((WINDOWPOS*)lParam);
        return lr;
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

CShellTabWindow::CShellTabWindow()
{
    m_bSubclassed = FALSE;
    m_SubclassId = 0x20010342;
    m_hShellTabWnd = NULL;
    m_pExplorerWnd = nullptr;

    SetRectEmpty(&m_LastParentPos);
    m_bManaging = false;
}

BOOL CShellTabWindow::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hShellTabWnd, CExplorerWindow* pExeplorerWnd)
{
    m_spShellBrowser = spShellBrowser;
    m_hShellTabWnd = hShellTabWnd;
    
    ::SetWindowSubclass(m_hShellTabWnd, ShellTabWndSubclassProc, m_SubclassId, (DWORD_PTR)this);
    m_pExplorerWnd = pExeplorerWnd;
    m_bSubclassed = TRUE;
    return m_bSubclassed;
}

void CShellTabWindow::Unintialize()
{
    if(m_bSubclassed)
    {
        SetManaging(false);
        ::RemoveWindowSubclass(m_hShellTabWnd, ShellTabWndSubclassProc, m_SubclassId);
        m_bSubclassed = FALSE;
    }
}

bool CShellTabWindow::SetManaging(bool bManaging)
{
    LogTrace(_T("CShellTabWindow::SetManaging(bManaging = %d)[m_bManaging = %d]"), bManaging, m_bManaging);

    if (m_hShellTabWnd == NULL)
        return m_bManaging;

    if (bManaging == m_bManaging)
        return m_bManaging;

    bool bOldVal = m_bManaging;
    m_bManaging = bManaging;

    // add/remove the tabs
    if (m_bManaging)
    {
        ForceRedraw();
    }
    else
    {
        ReclameTabSpace();
    }

    return bOldVal;
}

void CShellTabWindow::ForceRedraw()
{
    // we want to make sure the OnWindowPosChanging handler is called
    CWindow tw(m_hShellTabWnd);

    RECT WndRect;
    tw.GetWindowRect(&WndRect);
    tw.ScreenToClient(&WndRect);
    //::ScreenToClient(::GetParent(m_hExplorerWnd), &WndRect);
    //::MoveWindow(m_hExplorerWnd, &WndRect, TRUE);
    tw.MoveWindow(&WndRect, TRUE);
}

void CShellTabWindow::ReclameTabSpace()
{
    CWindow tw(m_hShellTabWnd);

    RECT WndRect;
    tw.GetWindowRect(&WndRect);
    tw.ScreenToClient(&WndRect);
    //::ScreenToClient(GetParent(), &WndRect);

    ATLASSERT(m_pExplorerWnd != nullptr);
    WndRect.top -= m_pExplorerWnd->GetTabbarHeight();

    tw.MoveWindow(&WndRect, TRUE);
    //::MoveWindow(m_hWnd, &WndRect, TRUE);
}

void CShellTabWindow::UpdateTabSizeAndPosition(RECT& WndRect)
{
    //ATLTRACE(_T("CShellTabWindow::UpdateTabSizeAndPosition(WndRect [left = %d, top = %d, right = %d, bottom = %d])\n"),
    //    WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);
    
    ATLASSERT(m_pExplorerWnd != nullptr);

    if (m_bManaging)
    {
        m_pExplorerWnd->UpdateTabSizeAndPosition(WndRect);
    }
}

void CShellTabWindow::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
//    LogTrace(_T("CShellTabWindow::OnWindowPosChanging(x = %d, y = %d, cx = %d, cy = %d),m_bManaging = %d"), lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy, m_bManaging);
    ATLTRACE(_T("CShellTabWindow::OnWindowPosChanging(x = %d, y = %d, cx = %d, cy = %d),m_bManaging = %d\n"), lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy, m_bManaging);

    bool bSysCall = true;
    if (m_bManaging)
    {
        CWindow tw(m_hShellTabWnd);
        RECT WndRect = { lpwndpos->x,lpwndpos->y,lpwndpos->x + lpwndpos->cx,lpwndpos->y + lpwndpos->cy };
        if (lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE))
        {
            RECT WndRectOld;
            tw.GetWindowRect(&WndRectOld);
            tw.ScreenToClient(&WndRectOld);
            if (lpwndpos->flags & SWP_NOSIZE)
            {
                WndRect.bottom = WndRect.top + (WndRectOld.bottom - WndRectOld.top);
                WndRect.right = WndRect.left + (WndRectOld.right - WndRectOld.left);
            }
            if (lpwndpos->flags & SWP_NOMOVE)
            {
                OffsetRect(&WndRect, WndRectOld.left - lpwndpos->x, WndRectOld.top - lpwndpos->y);
            }
            bSysCall = false;
        }

        if (bSysCall)
        {
            m_LastParentPos.left = lpwndpos->x;
            m_LastParentPos.top = lpwndpos->y;
            m_LastParentPos.right = m_LastParentPos.left + lpwndpos->cx;
            m_LastParentPos.bottom = m_LastParentPos.top + lpwndpos->cy;

            UpdateTabSizeAndPosition(WndRect);
        }

        ATLASSERT(WndRect.left >= 0);
        ATLASSERT(WndRect.top >= 0);
        lpwndpos->x = WndRect.left;
        lpwndpos->y = WndRect.top;
        lpwndpos->cx = WndRect.right - WndRect.left;
        lpwndpos->cy = WndRect.bottom - WndRect.top;
    }
}

