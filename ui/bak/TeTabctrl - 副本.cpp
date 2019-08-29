#include "pch.h"
#include "framework.h"
#include "TeTabctrl.h"

void CTeTabCtrl::Init()
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    ATLASSERT(::IsWindow(pT->m_hWnd));
    // create or load a cursor
#if (WINVER >= 0x0500) || defined(_WIN32_WCE)
    m_hCursor = ::LoadCursor(NULL, IDC_HAND);
#else
    m_hCursor = ::CreateCursor(ModuleHelper::GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
#endif
    ATLASSERT(m_hCursor != NULL);

#ifndef _WIN32_WCE
    m_tip.Create(pT->m_hWnd);
    ATLASSERT(m_tip.IsWindow());
#endif // !_WIN32_WCE
}

BOOL CTeTabCtrl::SubclassWindow(HWND hWnd)
 {
     ATLASSERT(m_hWnd == NULL);
     ATLASSERT(::IsWindow(hWnd));
#if (_MSC_VER >= 1300)
     BOOL bRet = CWindowImpl< CTeTabCtrl, WTL::CTabCtrl>::SubclassWindow(hWnd);
#else // !(_MSC_VER >= 1300)
     typedef CWindowImpl< CTeTabCtrl, WTL::CTabCtrl>   _baseClass;
     BOOL bRet = _baseClass::SubclassWindow(hWnd);
#endif // !(_MSC_VER >= 1300)
     if (bRet)
     {
         CTeTabCtrl* pT = static_cast<CTeTabCtrl *>(this);
         pT->Init();
     }

     return bRet;
 }

LRESULT CTeTabCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    pT->Init();
    return 0;
}

LRESULT CTeTabCtrl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    if (m_tip.IsWindow())
    {
        m_tip.DestroyWindow();
        m_tip.m_hWnd = NULL;
    }
    bHandled = FALSE;
    return 1;
}

LRESULT CTeTabCtrl::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);

    MSG msg = { pT->m_hWnd, uMsg, wParam, lParam };
    if (m_tip.IsWindow() && m_bIsUsingTip)
        m_tip.RelayEvent(&msg);
    
    bHandled = FALSE;
    return 1;
}

LRESULT CTeTabCtrl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CDCHandle dc((HDC)wParam);

    RECT rcClient;
    GetClientRect(&rcClient);
    //FillSolidRect(hDC, &rcClient, RGB(0,0,255));
    DrawThemeParentBackground(m_hWnd, dc, &rcClient);
    bHandled = TRUE;
    return 1;
}

LRESULT CTeTabCtrl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT CTeTabCtrl::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    if (wParam != NULL)
    {
        pT->DoEraseBackground((HDC)wParam);
        pT->DoPaint((HDC)wParam);
    }
    else
    {
        CPaintDC dc(pT->m_hWnd);
        pT->DoEraseBackground(dc.m_hDC);
        pT->DoPaint(dc.m_hDC);
    }

    bHandled = FALSE;
    return 0;
}

LRESULT CTeTabCtrl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    return 0;
}

LRESULT CTeTabCtrl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    return 0;
}

LRESULT CTeTabCtrl::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    if (m_bHover)
    {
        pT->Invalidate();
        pT->UpdateWindow();
    }
    return 0;
}

LRESULT CTeTabCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    return 0;
}

LRESULT CTeTabCtrl::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    return 0;
}

LRESULT CTeTabCtrl::OnLButtonDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    return 0;
}

LRESULT CTeTabCtrl::OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return DLGC_WANTCHARS;
}

LRESULT CTeTabCtrl::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    return FALSE;
}

LRESULT CTeTabCtrl::OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    pT->Invalidate();
    pT->UpdateWindow();
    return 0;
}

LRESULT CTeTabCtrl::OnUpdateUiState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // If the control is subclassed or superclassed, this message can cause
    // repainting without WM_PAINT. We don't use this state, so just do nothing.
    return 0;
}

LRESULT CTeTabCtrl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    return 0;
}

void CTeTabCtrl::DoEraseBackground(CDCHandle dc)
{
}

void CTeTabCtrl::DoPaint(CDCHandle dc)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    RECT rect;
    pT->GetClientRect(&rect);
    dc.FillSolidRect(&rect, RGB(72, 34, 102));
}

BOOL CTeTabCtrl::StartTrackMouseLeave()
{
    TRACKMOUSEEVENT tme = { 0 };
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    return _TrackMouseEvent(&tme);
}
