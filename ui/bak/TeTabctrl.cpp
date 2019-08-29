#include "pch.h"
#include "framework.h"
#include "TeTabctrl.h"

LOGFONT lf_default = { 12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
            DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Microsoft Sans Serif") };

static int TabbarBtnCount = 2;

CTeTabCtrl::CTeTabCtrl()
{
    m_nButtonIDDown = CTCID_NOBUTTON;
    m_nPrevState = BNST_INVISIBLE;
    m_nNextState = BNST_INVISIBLE;
    m_nItemSelected = -1;
    m_nItemNdxOffset = 0;
    m_dwLastRepeatTime = 0;
    m_hCursorMove = NULL;
    m_hCursor = NULL;
    m_nItemDragDest = 0;
}

CTeTabCtrl::~CTeTabCtrl() 
{
    m_TabItems.clear();

    ::DestroyCursor(m_hCursor);
    m_hCursor = NULL;
    ::DestroyCursor(m_hCursorMove);
    m_hCursorMove = NULL;

    m_textFont.DeleteObject();
}

BOOL CTeTabCtrl::Init()
{
    // create or load a cursor
#if (WINVER >= 0x0500) || defined(_WIN32_WCE)
    m_hCursor = ::LoadCursor(NULL, IDC_HAND);
#else
    m_hCursor = ::CreateCursor(ModuleHelper::GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
#endif
    if (m_hCursor == NULL)
        return FALSE;

    LOGFONT lf = lf_default;
    CFontHandle hSysFont = WTL::AtlGetStockFont(DEFAULT_GUI_FONT); //SYSTEM_FONT
    hSysFont.GetLogFont(lf);
    m_textFont.CreateFontIndirect(&lf);

#ifndef _WIN32_WCE
    m_tipCtrl.Create(m_hWnd);
    if (!m_tipCtrl.IsWindow())
        return FALSE;
#endif // !_WIN32_WCE

    return TRUE;
}

BOOL CTeTabCtrl::SubclassWindow(HWND hWnd)
 {
     ATLASSERT(m_hWnd == NULL);
     ATLASSERT(::IsWindow(hWnd));

#if (_MSC_VER >= 1300)
     BOOL bRet = CWindowImpl< CTeTabCtrl, CWindow>::SubclassWindow(hWnd);
#else // !(_MSC_VER >= 1300)
     typedef CWindowImpl< CTeTabCtrl, CWindow>   _baseClass;
     BOOL bRet = _baseClass::SubclassWindow(hWnd);
#endif // !(_MSC_VER >= 1300)
     if (bRet)
     {
         bRet = Init();
         if (bRet)
         {
             //OnThemeChanged(0, 0);
             CWindowImpl< CTeTabCtrl, CWindow>::ModifyStyle(0, WS_CLIPCHILDREN);
             RecalcLayout(RECALC_RESIZED, m_nItemSelected);
         }
     }

     return bRet;
 }

int CTeTabCtrl::IsItemHighlighted(int nItem)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    return (m_TabItems[nItem].m_fHighlighted) ? 1 : 0;
}

int CTeTabCtrl::HighlightItem(int nItem, BOOL fHighlight)
{
    if (!(GetStyle() & CTCS_MULTIHIGHLIGHT))
        return CTCERR_NOMULTIHIGHLIGHTSTYLE;
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    if (m_nItemSelected == -1 && !fHighlight)
        return CTCERR_NOERROR;
    if (m_nItemSelected == -1)
    {
        SetCurSel(nItem);
        return CTCERR_NOERROR;
    }
    if (fHighlight == m_TabItems[nItem].m_fHighlighted || nItem == m_nItemSelected)
        return CTCERR_NOERROR;

    m_TabItems[nItem].m_fHighlighted = fHighlight;
    return CTCERR_NOERROR;
}

int CTeTabCtrl::GetItemData(int nItem, DWORD_PTR& dwData)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    dwData = m_TabItems[nItem].m_lParam;
    return CTCERR_NOERROR;
}

int CTeTabCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    m_TabItems[nItem].m_lParam = dwData;
    return CTCERR_NOERROR;
}

int CTeTabCtrl::GetItemText(int nItem, CString& sText)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    sText = m_TabItems[nItem].m_sText;
    return CTCERR_NOERROR;
}

int CTeTabCtrl::SetItemText(int nItem, const CString& sText)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    m_TabItems[nItem].m_sText = sText;
    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    Invalidate(FALSE);
    return CTCERR_NOERROR;
}

int CTeTabCtrl::GetItemRect(int nItem, CRect& rect) const
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    rect = m_TabItems[nItem].m_rect;
    return CTCERR_NOERROR;
}

int CTeTabCtrl::SetItemTooltipText(int nItem, const CString& sText)
{
    if (!(GetStyle() & CTCS_TOOLTIPS))
        return CTCERR_NOTOOLTIPSSTYLE;
    if (nItem >= CTCID_LASTBUTTON && nItem < (int)m_TabItems.size())
    {
        if (!m_tipCtrl.IsWindow())
        {
            if (!m_tipCtrl.Create(m_hWnd))
                return CTCERR_CREATETOOLTIPFAILED;
            m_tipCtrl.Activate(TRUE);
        }
        if (nItem >= 0)
            nItem++;
        m_tipCtrl.DelTool(m_hWnd, nItem);
        m_tipCtrl.AddTool(m_hWnd, (LPCTSTR)sText, CRect(0, 0, 0, 0), nItem);
        RecalcLayout(RECALC_RESIZED, m_nItemSelected);
        Invalidate(FALSE);
        return CTCERR_NOERROR;
    }
    return CTCERR_INDEXOUTOFRANGE;
}

void CTeTabCtrl::SetDragCursors(HCURSOR hCursorMove)
{
    ::DestroyCursor(m_hCursorMove);
    m_hCursorMove = NULL;
    m_hCursorMove = ((HCURSOR)::CopyIcon((HICON)(hCursorMove)));
}

BOOL CTeTabCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    if (dwRemove & CTCS_TOOLTIPS)
        m_tipCtrl.DestroyWindow();
    if (dwRemove & CTCS_MULTIHIGHLIGHT)
    {
        for (int i = 0; i < (int)m_TabItems.size(); i++)
            m_TabItems[i].m_fHighlighted = FALSE;
    }
    if (dwAdd & CTCS_MULTIHIGHLIGHT)
    {
        for (int i = 0; i < (int)m_TabItems.size(); i++)
        {
            if (i == m_nItemSelected)
                m_TabItems[i].m_fHighlighted = TRUE;
        }
    }
    CWindowImpl< CTeTabCtrl, CWindow>::ModifyStyle(dwRemove, dwAdd, nFlags);
    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    Invalidate(FALSE);
    return TRUE;
}

void CTeTabCtrl::SetControlFont(const LOGFONT& lf, BOOL fRedraw)
{
    if (m_textFont.m_hFont)
    {
        DeleteObject(m_textFont);
        m_textFont.m_hFont = NULL;
    }

    if (!m_textFont.CreateFontIndirect(&lf))
        m_textFont.CreateFontIndirect(&lf_default);

    if (fRedraw)
    {
        RecalcLayout(RECALC_RESIZED, m_nItemSelected);
        Invalidate();
    }
}

int CTeTabCtrl::InsertItem(int nItem, const CString& sText, LPARAM lParam)
{
    if (nItem<0 || nItem > (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    if (sText.GetLength() > MAX_LABEL_TEXT - 1)
        return CTCERR_TEXTTOOLONG;

    CTeTabItem item(sText, lParam);
    m_TabItems.insert(m_TabItems.begin() + nItem, item);

    if (m_nItemSelected >= nItem)
        m_nItemSelected++;

    if (m_tipCtrl.IsWindow())
    {
        for (int i = (int)m_TabItems.size() - 1; i > nItem; i--)
        {
            TCHAR s[256];
            m_tipCtrl.GetText(s, m_hWnd, i);
            m_tipCtrl.DelTool(m_hWnd, i);
            m_tipCtrl.AddTool(m_hWnd, s, CRect(0, 0, 0, 0), i + 1);
        }
        m_tipCtrl.DelTool(m_hWnd, nItem + 1);
    }

    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    Invalidate(FALSE);

    return nItem;
}

int CTeTabCtrl::DeleteItem(int nItem)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    if (m_tipCtrl.IsWindow())
    {
        for (int i = nItem + 1; i < (int)m_TabItems.size(); i++)
        {
            TCHAR s[256];
            m_tipCtrl.GetText(s, m_hWnd, i + 1);
            m_tipCtrl.DelTool(m_hWnd, i);
            m_tipCtrl.AddTool(m_hWnd, s, CRect(0, 0, 0, 0), i);
        }
    }

    if ((int)m_TabItems.size() == 1)
        m_nItemSelected = -1;
    else if (m_nItemSelected == nItem)
    {
        if (m_nItemSelected == (int)m_TabItems.size() - 1) // last item
        {
            m_nItemSelected--;
            m_TabItems[m_nItemSelected].m_fSelected = TRUE;
        }
        else
            m_TabItems[m_nItemSelected + 1].m_fSelected = TRUE;
    }
    else if (m_nItemSelected > nItem)
        m_nItemSelected--;

    m_TabItems.erase(m_TabItems.begin() + nItem);

    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    Invalidate(FALSE);
    return CTCERR_NOERROR;
}

void CTeTabCtrl::DeleteAllItems()
{
    if (m_tipCtrl.IsWindow())
    {
        for (int i = 0; i < (int)m_TabItems.size(); i++)
        {
            m_tipCtrl.DelTool(m_hWnd, i + 1);
        }
    }

    m_TabItems.clear();

    m_nItemSelected = -1;

    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    Invalidate(FALSE);
}

int	CTeTabCtrl::HitTest(const CPoint& pt)
{
    CRect rCl;
    GetClientRect(&rCl);

    int nBtns = 2;
    int nA = rCl.Height() - 3;

    CRect rPrev, rNext;

    rPrev.SetRect(0, 0, nA + 1, rCl.Height());
    rNext.SetRect(nA + 1, 0, 2 * nA + 3, rCl.Height());

    if (m_nPrevState && rPrev.PtInRect(pt))
        return CTCHT_ONPREVBUTTON;

    if (m_nNextState && rNext.PtInRect(pt))
        return CTCHT_ONNEXTBUTTON;

    for (int i = 0; i < (int)m_TabItems.size(); i++)
    {
        if (m_TabItems[i].HitTest(pt))
            return i;
    }
    return CTCHT_NOWHERE;
}


//message handler
int CTeTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!Init())
        return -1;
    
    return 0;
}

void CTeTabCtrl::OnDestroy()
{
    if (m_tipCtrl.IsWindow())
    {
        m_tipCtrl.DestroyWindow();
        m_tipCtrl.m_hWnd = NULL;
    }
}

LRESULT CTeTabCtrl::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    MSG msg = { m_hWnd, uMsg, wParam, lParam };

    if (m_tipCtrl.IsWindow() && (GetStyle() & CTCS_TOOLTIPS))
        m_tipCtrl.RelayEvent(&msg);
/*
    if (GetStyle() & CTCS_TOOLTIPS && m_tip.m_hWnd &&
        (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP || pMsg->message == WM_MOUSEMOVE))
    {
        m_tip.RelayEvent(pMsg);
    }
*/
    bHandled = FALSE;
    return 1;
}

BOOL CTeTabCtrl::OnEraseBackground(CDCHandle dc)
{
#if 0
    RECT rcClient;
    GetClientRect(&rcClient);
    //FillSolidRect(hDC, &rcClient, RGB(0,0,255));
    DrawThemeParentBackground(m_hWnd, dc, &rcClient);
#endif    
    return TRUE;
}

LRESULT CTeTabCtrl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT CTeTabCtrl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (wParam != NULL)
    {
        CMemDC memdc((HDC)wParam);
        DoEraseBackground(memdc);
        DoPaint(memdc);
    }
    else
    {
        CPaintDC dc(m_hWnd);
        {
            CMemDC memdc(dc);
            DoEraseBackground(memdc);
            DoPaint(memdc);
        }
    }

    bHandled = TRUE;
    return 0;
}
/*
void CTeTabCtrl::OnPaint(CDCHandle dc)
{
    CMemDC memdc(dc.m_hDC);
    DoEraseBackground(memdc);
    DoPaint(memdc);
}
*/
LRESULT CTeTabCtrl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    CTeTabCtrl* pT = static_cast<CTeTabCtrl*>(this);
    return 0;
}

void CTeTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    //POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
}

void CTeTabCtrl::OnMouseLeave()
{
/*
    if (m_bHover)
    {
        Invalidate();
        UpdateWindow();
    }
*/
}


void CTeTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    //POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    
}

void CTeTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    //POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
}

void CTeTabCtrl::OnLButtonDblClick(UINT nFlags, CPoint point)
{
    //POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
}

void CTeTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
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

LRESULT CTeTabCtrl::OnSize(UINT uType, CSize size)
{
    if ((size.cx > 0) && (size.cy > 0))
        RecalcLayout(RECALC_RESIZED, m_nItemSelected);

    return 0;
}

LRESULT CTeTabCtrl::OnTimer(UINT_PTR timerID)
{
    return 0;
}

LRESULT CTeTabCtrl::OnSelectionChanged(LPNMHDR)
{
    //SetActiveTab(GetCurSel());

    return 0;
}

void CTeTabCtrl::DoEraseBackground(CMemDC& dc)
{
    RECT rcClient;
    GetClientRect(&rcClient);

    dc.FillSolidRect(&rcClient, RGB(0,255,255));
    //DrawThemeParentBackground(m_hWnd, dc, &rcClient);
}

void CTeTabCtrl::DoPaint(CMemDC& dc)
{
    CRect rect, rPrev, rNext;
    GetClientRect(&rect);

    if (m_nPrevState && m_nNextState)
    {
        int nA = rect.Height() - 3;
        rPrev.SetRect(1, 2, nA + 1, rect.Height() - 1);
        rNext.SetRect(nA + 2, 2, 2 * nA + 2, rect.Height() - 1);

        dc.FillSolidRect(rPrev, RGB(0, 0, 255));
        dc.FillSolidRect(rNext, RGB(255, 255, 0));
    }
    // draw tab items visible and not selected
    WTL::CFontHandle hFont = m_textFont.m_hFont;
    for (int i = 0; i < (int)m_TabItems.size(); i++)
    {
        m_TabItems[i].Draw(dc, hFont);
    }


}

BOOL CTeTabCtrl::StartTrackMouseLeave()
{
    TRACKMOUSEEVENT tme = { 0 };
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    return _TrackMouseEvent(&tme);
}

int CTeTabCtrl::HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
    if (!(GetStyle() & CTCS_MULTIHIGHLIGHT))
        return CTCERR_NOMULTIHIGHLIGHTSTYLE;

    for (int i = 0; i < (int)m_TabItems.size(); i++)
        m_TabItems[i].m_fHighlightChanged = FALSE;

    if (fCtrlPressed)
    {
        if (nItem != m_nItemSelected)
        {
            m_TabItems[nItem].m_fHighlighted = !m_TabItems[nItem].m_fHighlighted;
            if (fMouseSel)
                m_TabItems[nItem].m_fHighlightChanged = TRUE;
        }
    }
    else if (!m_TabItems[nItem].m_fHighlighted)
    {
        m_TabItems[nItem].m_fHighlighted = TRUE;
        m_TabItems[nItem].m_fHighlightChanged = TRUE;
        for (int i = 0; i < (int)m_TabItems.size(); i++)
        {
            if (i != m_nItemSelected)
            {
                if (m_TabItems[i].m_fHighlighted)
                {
                    m_TabItems[i].m_fHighlighted = FALSE;
                    if (fMouseSel)
                        m_TabItems[i].m_fHighlightChanged = TRUE;
                }
            }
        }
    }
    if (fMouseSel)
        RecalcLayout(RECALC_ITEM_SELECTED, nItem);
    
    Invalidate(FALSE);
    return CTCERR_NOERROR;
}

int CTeTabCtrl::SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    if (m_nItemSelected != -1)
        m_TabItems[m_nItemSelected].m_fSelected = FALSE;

    m_nItemSelected = nItem;

    if (m_nItemSelected != -1)
        m_TabItems[m_nItemSelected].m_fSelected = TRUE;

    if (fMouseSel)
        RecalcLayout(RECALC_ITEM_SELECTED, m_nItemSelected);
    else
    {
        m_nItemNdxOffset = nItem;
        RecalcLayout(RECALC_RESIZED, m_nItemSelected);
    }
    Invalidate(FALSE);
    HighlightItem(nItem, fMouseSel, fCtrlPressed);
    
    return CTCERR_NOERROR;
}

void CTeTabCtrl::RecalcEditResized(int nOffset, int nItem)
{
    CRect rCl;
    GetClientRect(rCl);
    do
    {
        CRect r;
        CDCHandle dc = GetDC();
        CFontHandle hOldFont = dc.SelectFont(m_textFont);
        int h = dc.DrawText(m_TabItems[nItem].m_sText + _T("X"), -1, r, DT_CALCRECT);
        dc.SelectFont(hOldFont);
        ReleaseDC(dc);
        r = m_TabItems[nItem].m_rect;
        if (r.Height() > h)
        {
            r.top += (r.Height() - h) / 2;
            r.bottom -= (r.Height() - h) / 2;
        }
        r.left += 2;
        if (r.right > rCl.right && m_nItemSelected > m_nItemNdxOffset)
        {
            m_nItemNdxOffset++;
            RecalcAllTabItems(rCl);
            RecalcOffset(nOffset);
        }
        else
        {
            if (r.right > rCl.right)
                r.right = rCl.right;
            m_editCtrl.MoveWindow(r);
            int n = m_TabItems[nItem].m_sText.GetLength();
            int nStart, nEnd;
            m_editCtrl.GetSel(nStart, nEnd);
            if (nStart == nEnd && nStart == n)
            {
                m_editCtrl.SetSel(0, 0);
                m_editCtrl.SetSel(n, n);
            }
            return;
        }
    } while (1);
}

void CTeTabCtrl::RecalcOffset(int nOffset)
{
    CRect rCl;
    GetClientRect(&rCl);

    int nBtns = 2;
    if (GetStyle() & CTCS_FOURBUTTONS)
        nBtns = 4;
    int nBnWidth = nBtns * (rCl.Height() - 3) + 3;

    for (int i = 0; i < (int)m_TabItems.size(); i++)
    {
        m_TabItems[i].m_rect.OffsetRect(nOffset, 0);
        if (m_tipCtrl.IsWindow())
        {
            WTL::CToolInfo ti(0, m_hWnd, i + 1, (LPRECT)&m_TabItems[i].m_rect, (LPTSTR)(LPCTSTR)m_TabItems[i].m_sText);
            m_tipCtrl.SetToolRect(ti);
        }
    }
}

int CTeTabCtrl::RecalcAllTabItems(const CRect& rcClient)
{
    int nWidth = 0;

    // calculate width
    int nOffset = 0;
    CRect rcText;
    CDCHandle dc = GetDC();
    CFontHandle hOldFont = dc.SelectFont(m_textFont);
    if (GetStyle() & CTCS_FIXEDWIDTH)
    {
        int nMaxWidth = 0;
        int i;
        for (i = 0; i < (int)m_TabItems.size(); i++)
        {
            int w = 0;
            int h = dc.DrawText((LPCTSTR)m_TabItems[i].m_sText, -1, rcText, DT_CALCRECT);
            if (h > 0)
                w = rcText.Width();
            if (w > nMaxWidth)
                nMaxWidth = w;
        }
        for (i = 0; i < (int)m_TabItems.size(); i++)
        {
            m_TabItems[i].m_rect = CRect(0, 1, nMaxWidth + rcClient.Height() + 4, rcClient.Height() - 1);
            m_TabItems[i].m_rect += CPoint(nOffset, 0);

            nOffset += m_TabItems[i].m_rect.Width() - rcClient.Height() / 2;
            nWidth = m_TabItems[i].m_rect.right;
        }
    }
    else
    {
        for (int i = 0; i < (int)m_TabItems.size(); i++)
        {
            int w = 0;
            int h = dc.DrawText(m_TabItems[i].m_sText, -1, rcText, DT_CALCRECT);
            if (h > 0)
                w = rcText.Width();
            m_TabItems[i].m_rect = CRect(0, 1, w + rcClient.Height() + 4, rcClient.Height() - 1);
            m_TabItems[i].m_rect += CPoint(nOffset, 0);

            nOffset += m_TabItems[i].m_rect.Width() - rcClient.Height() / 2;
            nWidth = m_TabItems[i].m_rect.right;

        }
    }
    dc.SelectFont(hOldFont);
    ReleaseDC(dc);

    return nWidth;
}

void CTeTabCtrl::RecalcLayout(int nRecalcType, int nItem)
{
    CRect rCl;
    GetClientRect(&rCl);

    int nBtns = 2;

    int nWidth = RecalcAllTabItems(rCl);

    if ((GetStyle() & CTCS_AUTOHIDEBUTTONS) && (nWidth <= rCl.Width()))
    {
        m_nPrevState = BNST_INVISIBLE;
        m_nNextState = BNST_INVISIBLE;
        m_nItemNdxOffset = 0;
        RecalcOffset(0);
        if (nRecalcType == RECALC_EDIT_RESIZED)
            RecalcEditResized(0, nItem);

        if (m_tipCtrl.IsWindow())
        {
            m_tipCtrl.SetToolRect(m_hWnd, CTCID_PREVBUTTON, CRect(0, 0, 0, 0));
            m_tipCtrl.SetToolRect(m_hWnd, CTCID_NEXTBUTTON, CRect(0, 0, 0, 0));
        }
        return;
    }

    if (m_tipCtrl.IsWindow())
    {
        int nA = rCl.Height() - 3;
        m_tipCtrl.SetToolRect(m_hWnd, CTCID_PREVBUTTON, CRect(0, 0, 0, 0));
        m_tipCtrl.SetToolRect(m_hWnd, CTCID_NEXTBUTTON, CRect(0, 0, nA + 1, rCl.Height()));
    }

    int nBnWidth = nBtns * (rCl.Height() - 3) + 3;

    if (m_nPrevState == BNST_INVISIBLE)
        m_nPrevState = BNST_NORMAL;
    if (m_nNextState == BNST_INVISIBLE)
        m_nNextState = BNST_NORMAL;

    if ((int)m_TabItems.size() == 0)
        return;

    switch (nRecalcType)
    {
    case RECALC_PREV_PRESSED:
    {
        RecalcOffset(nBnWidth);
        if (m_nItemNdxOffset > 0)
        {
            m_nItemNdxOffset--;
            RecalcAllTabItems(rCl);
            RecalcOffset(nBnWidth);
        }
    }
    break;
    case RECALC_NEXT_PRESSED:
    {
        RecalcOffset(nBnWidth);
        if (m_TabItems[(int)m_TabItems.size() - 1].m_rect.right > rCl.Width() && m_nItemNdxOffset != (int)m_TabItems.size() - 1)
        {
            m_nItemNdxOffset++;
            RecalcAllTabItems(rCl);
            RecalcOffset(nBnWidth);
        }
    }
    break;
    case RECALC_EDIT_RESIZED:
    {
        RecalcOffset(nBnWidth);
        RecalcEditResized(nBnWidth, nItem);
    }
    break;
    case RECALC_LAST_PRESSED:
    {
        m_nItemNdxOffset = (int)m_TabItems.size() - 1;
    }
    default:	// window resized
    {
        BOOL bNdxOffsetChanged = FALSE;
        RecalcOffset(nBnWidth);
        while (m_nItemNdxOffset >= 0 && m_TabItems[(int)m_TabItems.size() - 1].m_rect.right < rCl.Width())
        {
            m_nItemNdxOffset--;
            if (m_nItemNdxOffset >= 0)
            {
                RecalcAllTabItems(rCl);
                RecalcOffset(nBnWidth);
            }
            bNdxOffsetChanged = TRUE;
        }
        if (bNdxOffsetChanged)
        {
            m_nItemNdxOffset++;
            RecalcAllTabItems(rCl);
            RecalcOffset(nBnWidth);
        }
    }
    break;
    }
}

int CTeTabCtrl::MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
    if (nItemSrc < 0 || nItemSrc >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    if (nItemDst<0 || nItemDst> (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    if (nItemSrc == nItemDst || nItemSrc == nItemDst - 1)
        return nItemSrc;

    CTeTabItem& Item = m_TabItems[nItemSrc];

    // remove item from old place
    TCHAR sOldTooltip[256];
    if (m_tipCtrl.IsWindow())
    {
        m_tipCtrl.GetText(sOldTooltip, m_hWnd, nItemSrc + 1);
        for (int i = nItemSrc + 1; i < (int)m_TabItems.size(); i++)
        {
            TCHAR s[256];
            m_tipCtrl.GetText(s, m_hWnd, i + 1);
            m_tipCtrl.DelTool(m_hWnd, i);
            m_tipCtrl.AddTool(m_hWnd, s, CRect(0, 0, 0, 0), i);
        }
    }

    m_TabItems.erase(m_TabItems.begin() + nItemSrc);

    // insert item in new place
    if (nItemDst > nItemSrc)
        nItemDst--;

    m_TabItems.insert(m_TabItems.begin() + nItemDst, Item);

    if (m_tipCtrl.IsWindow())
    {
        for (int i = (int)m_TabItems.size() - 1; i > nItemDst; i--)
        {
            TCHAR s[256];
            m_tipCtrl.GetText(s, m_hWnd, i);
            m_tipCtrl.DelTool(m_hWnd, i + 1);
            m_tipCtrl.AddTool(m_hWnd, s, CRect(0, 0, 0, 0), i + 1);
        }
        m_tipCtrl.DelTool(m_hWnd, nItemDst + 1);
        m_tipCtrl.AddTool(m_hWnd, sOldTooltip, CRect(0, 0, 0, 0), nItemDst + 1);
    }

    m_nItemSelected = nItemDst;

    RecalcLayout(RECALC_ITEM_SELECTED, m_nItemSelected);
    Invalidate(FALSE);
    if (fMouseSel)
        NotifyParent(CTCN_ITEMMOVE, m_nItemSelected, CPoint(0, 0));
    return nItemDst;
}

int CTeTabCtrl::CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
    if (nItemSrc < 0 || nItemSrc >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    if (nItemDst<0 || nItemDst > (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;

    CString sDst;
    BOOL bAppendFlag = TRUE;
    int n = m_TabItems[nItemSrc].m_sText.GetLength();
    if (n >= 4)
    {
        if (m_TabItems[nItemSrc].m_sText[n - 1] == _T(')') &&
            m_TabItems[nItemSrc].m_sText[n - 2] > _T('1') &&
            m_TabItems[nItemSrc].m_sText[n - 2] <= _T('9') &&
            m_TabItems[nItemSrc].m_sText[n - 3] == _T('('))
        {
            n = m_TabItems[nItemSrc].m_sText.GetLength() - 3;
            bAppendFlag = FALSE;
        }
        else if (m_TabItems[nItemSrc].m_sText[n - 1] == _T(')') &&
            m_TabItems[nItemSrc].m_sText[n - 2] >= _T('0') &&
            m_TabItems[nItemSrc].m_sText[n - 2] <= _T('9') &&
            m_TabItems[nItemSrc].m_sText[n - 3] >= _T('1') &&
            m_TabItems[nItemSrc].m_sText[n - 3] <= _T('9') &&
            m_TabItems[nItemSrc].m_sText[n - 4] == _T('('))
        {
            n = m_TabItems[nItemSrc].m_sText.GetLength() - 4;
            bAppendFlag = FALSE;
        }
    }
    int ndx = 1;
    while (1)
    {
        ndx++;
        if (bAppendFlag)
            sDst.Format(_T("%s (%d)"), (LPCTSTR)m_TabItems[nItemSrc].m_sText, ndx);
        else
            sDst.Format(_T("%s(%d)"), (LPCTSTR)m_TabItems[nItemSrc].m_sText.Left(n), ndx);

        int i;
        for (i = 0; i < (int)m_TabItems.size(); i++)
        {
            if (m_TabItems[i].m_sText == sDst)
                break;
        }
        if (i == (int)m_TabItems.size())
            break;
    }


    int nRetItem = InsertItem(nItemDst, sDst, m_TabItems[nItemSrc].m_lParam);
    if (nRetItem >= 0)
    {
        SetCurSel(nRetItem);
        if (fMouseSel)
            NotifyParent(CTCN_ITEMCOPY, nRetItem, CPoint(0, 0));
    }
    else if (fMouseSel && nRetItem == CTCERR_OUTOFMEMORY)
        NotifyParent(CTCN_OUTOFMEMORY, nRetItem, CPoint(0, 0));

    return nRetItem;
}

BOOL CTeTabCtrl::NotifyParent(UINT code, int nItem, const CPoint& pt)
{
    CTC_NMHDR nmh;
    memset(&nmh, 0, sizeof(CTC_NMHDR));
    nmh.hdr.hwndFrom = m_hWnd;
    nmh.hdr.idFrom = GetDlgCtrlID();
    nmh.hdr.code = code;
    nmh.nItem = nItem;
    nmh.ptHitTest = pt;
    if (nItem >= 0)
    {
        _tcscpy_s(nmh.pszText, MAX_LABEL_TEXT, m_TabItems[nItem].m_sText);
        nmh.lParam = m_TabItems[nItem].m_lParam;
        nmh.rItem = m_TabItems[nItem].m_rect;
        nmh.fSelected = m_TabItems[nItem].m_fSelected;
        nmh.fHighlighted = m_TabItems[nItem].m_fHighlighted;
    }
    return (BOOL)GetParent().SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)& nmh);
}

int CTeTabCtrl::EditLabel(int nItem, BOOL fMouseSel)
{
    if (nItem < 0 || nItem >= (int)m_TabItems.size())
        return CTCERR_INDEXOUTOFRANGE;
    if (!(GetStyle() & CTCS_EDITLABELS))
        return CTCERR_NOEDITLABELSTYLE;
    if (nItem != m_nItemSelected)
        return CTCERR_ITEMNOTSELECTED;
    if (m_editCtrl.IsWindow())
        return CTCERR_ALREADYINEDITMODE;

    CRect r;
    CDCHandle dc = GetDC();
    CFontHandle hOldFont = dc.SelectFont(m_textFont);
    int h = dc.DrawText(m_TabItems[nItem].m_sText, -1, r, DT_CALCRECT);
    dc.SelectFont(hOldFont);
    ReleaseDC(dc);

    r = m_TabItems[nItem].m_rect;
    if (r.Height() > h)
    {
        r.top += (r.Height() - h) / 2;
        r.bottom -= (r.Height() - h) / 2;
    }
    r.left += 2;
    if (m_editCtrl.Create(m_hWnd, r, NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, CTCID_EDITCTRL))
    {
        CString sOld = m_TabItems[nItem].m_sText;
        m_editCtrl.SetFont(m_textFont, FALSE);
        m_editCtrl.SetLimitText(MAX_LABEL_TEXT);
        m_editCtrl.SetWindowText(m_TabItems[nItem].m_sText);
        m_editCtrl.SetFocus();
        m_editCtrl.SetSel(0, -1);
        if (fMouseSel)
            ReleaseCapture();
        for (;;)
        {
            MSG msg;
            ::GetMessage(&msg, NULL, 0, 0);

            switch (msg.message)
            {
            case WM_KEYDOWN:
            {
                if (msg.wParam == VK_ESCAPE)
                {
                    m_TabItems[nItem].m_sText = sOld;
                    m_editCtrl.DestroyWindow();
                    RecalcLayout(RECALC_RESIZED, m_nItemSelected);
                    Invalidate(FALSE);
                    return CTCERR_NOERROR;
                }
                if (msg.wParam == VK_RETURN)
                {
                    if (NotifyParent(CTCN_LABELUPDATE, nItem, CPoint(0, 0)))
                        break;
                    m_editCtrl.GetWindowText(m_TabItems[nItem].m_sText);
                    m_editCtrl.DestroyWindow();
                    RecalcLayout(RECALC_RESIZED, nItem);
                    Invalidate(FALSE);
                    return CTCERR_NOERROR;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            break;
            case WM_LBUTTONDOWN:
            {
                if (msg.hwnd == m_hWnd)
                {
                    POINTS pt = MAKEPOINTS(msg.lParam);
                    if (HitTest(CPoint(pt.x, pt.y)) != m_nItemSelected)
                    {
                        if (NotifyParent(CTCN_LABELUPDATE, nItem, CPoint(0, 0)))
                            break;
                        m_editCtrl.GetWindowText(m_TabItems[m_nItemSelected].m_sText);
                        m_editCtrl.DestroyWindow();
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                        return CTCERR_NOERROR;
                    }
                }
                else if (msg.hwnd == m_editCtrl.m_hWnd)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                {
                    if (NotifyParent(CTCN_LABELUPDATE, nItem, CPoint(0, 0)))
                        break;
                    m_editCtrl.GetWindowText(m_TabItems[m_nItemSelected].m_sText);
                    m_editCtrl.DestroyWindow();
                    return CTCERR_NOERROR;
                }
            }
            break;
            case WM_LBUTTONUP:
            {
                if (msg.hwnd == m_editCtrl.m_hWnd)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            break;
            case WM_NCLBUTTONDOWN:
            {
                if (NotifyParent(CTCN_LABELUPDATE, nItem, CPoint(0, 0)))
                    break;
                m_editCtrl.GetWindowText(m_TabItems[m_nItemSelected].m_sText);
                m_editCtrl.DestroyWindow();
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                return CTCERR_NOERROR;
            }
            break;
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }

    return CTCERR_NOERROR;
}

int CTeTabCtrl::RecalcPosition(CRect& rcPrev, CRect& rcNext, CRect& rcTab)
{
    CRect rcClient;
    GetClientRect(&rcClient);

    return 0;
}
