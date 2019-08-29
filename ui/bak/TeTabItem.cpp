#include "pch.h"
#include "framework.h"
#include "TeTabItem.h"

CTeTabItem::CTeTabItem(CString sText, LPARAM lParam)
{
    m_sText = sText;
    m_lParam = lParam;
}

void CTeTabItem::Draw(WTL::CDC& dc, WTL::CFontHandle& font)
{
    //COLORREF bgColor = GetSysColor((m_fSelected || m_fHighlighted) ? COLOR_WINDOW : COLOR_3DFACE);
    COLORREF bgColor = RGB(0, 0, 255);
    COLORREF fgColor = GetSysColor((m_fSelected || m_fHighlighted) ? COLOR_WINDOWTEXT : COLOR_BTNTEXT);

    dc.FillSolidRect(&m_rect, bgColor);
    CFontHandle pOldFont = dc.SelectFont(font);
    COLORREF bgOldColor = dc.SetBkColor(bgColor);
    COLORREF fgOldColor = dc.SetTextColor(fgColor);
    dc.DrawText((LPCTSTR)m_sText, m_sText.GetLength(), &m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    dc.SetTextColor(fgOldColor);
    dc.SetBkColor(bgOldColor);
    dc.SelectFont(pOldFont);
}
