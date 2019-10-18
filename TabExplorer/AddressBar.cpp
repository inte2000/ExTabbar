// TabExplorerBHO.cpp : Implementation of CTabExplorerBHO

#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "AddressBar.h"
#include "ExplorerWindow.h"


LRESULT CALLBACK CAddressBar::AddressBarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CAddressBar* pThisView = (CAddressBar*)dwRefData;
    ATLASSERT(pThisView != nullptr);
 
    if (uMsg == WM_WINDOWPOSCHANGING)
    {
        LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        pThisView->OnWindowPosChanging((WINDOWPOS*)lParam);
        return lr;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

CAddressBar::CAddressBar()
{
    m_bSubclassed = FALSE;
    m_SubclassId = 0x20020202;
    m_hAddressBarWnd = NULL;
    m_pExplorerWnd = nullptr;

    m_bManaging = false;
}

BOOL CAddressBar::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hAddressBarWnd, CExplorerWindow* pExeplorerWnd)
{
    m_spShellBrowser = spShellBrowser;
    m_hAddressBarWnd = hAddressBarWnd;
    
    ::SetWindowSubclass(m_hAddressBarWnd, AddressBarSubclassProc, m_SubclassId, (DWORD_PTR)this);
    m_pExplorerWnd = pExeplorerWnd;
    m_bSubclassed = TRUE;
    return m_bSubclassed;
}

void CAddressBar::Unintialize()
{
    if(m_bSubclassed)
    {
        SetManaging(false);
        ::RemoveWindowSubclass(m_hAddressBarWnd, AddressBarSubclassProc, m_SubclassId);
        m_bSubclassed = FALSE;
    }
}

void CAddressBar::MovePosition(LPRECT pRect)
{
    ATLASSERT(m_hAddressBarWnd != NULL);

//    ::MoveWindow(m_hAddressBarWnd, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, FALSE);

    ::SetWindowPos(m_hAddressBarWnd, HWND_BOTTOM, pRect->left, pRect->top,
        pRect->right - pRect->left, pRect->bottom - pRect->top, SWP_DRAWFRAME);
    RedrawWindow(m_hAddressBarWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

bool CAddressBar::SetManaging(bool bManaging)
{
    LogTrace(_T("CShellTabWindow::SetManaging(bManaging = %d)[m_bManaging = %d]"), bManaging, m_bManaging);

    if (m_hAddressBarWnd == NULL)
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

void CAddressBar::ForceRedraw()
{
    // we want to make sure the OnWindowPosChanging handler is called
    CWindow tw(m_hAddressBarWnd);

    RECT WndRect;
    tw.GetWindowRect(&WndRect);
    tw.ScreenToClient(&WndRect);
    tw.MoveWindow(&WndRect, TRUE);
}

void CAddressBar::ReclameTabSpace()
{
    CWindow tw(m_hAddressBarWnd);

    RECT WndRect;
    tw.GetWindowRect(&WndRect);
    tw.ScreenToClient(&WndRect);

    ATLASSERT(m_pExplorerWnd != nullptr);
    WndRect.top -= m_pExplorerWnd->GetTabbarHeight();

    tw.MoveWindow(&WndRect, TRUE);
}

void CAddressBar::UpdateTabSizeAndPosition(RECT& WndRect)
{
    //ATLTRACE(_T("CAddressBar::UpdateTabSizeAndPosition(WndRect [left = %d, top = %d, right = %d, bottom = %d])\n"),
    //    WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);
    
    ATLASSERT(m_pExplorerWnd != nullptr);
    
    if (m_bManaging)
    {
        int nTabbarHeight = m_pExplorerWnd->GetTabbarHeight();
        WndRect.top += nTabbarHeight;
        WndRect.bottom += nTabbarHeight;
    }

//    m_pExplorerWnd->UpdateTabSizeAndPosition(WndRect);
}

void CAddressBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
    //ATLTRACE(_T("CAddressBar::OnWindowPosChanging(x = %d, y = %d, cx = %d, cy = %d),m_bManaging = %d"), lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy, m_bManaging);

    bool bSysCall = true;
    if (m_bManaging)
    {
        CWindow tw(m_hAddressBarWnd);
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

