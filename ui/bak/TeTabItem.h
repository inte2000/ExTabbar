#pragma once

#define MAX_LABEL_TEXT				30

typedef struct _CTC_NMHDR
{
    NMHDR hdr;
    int	nItem;
    TCHAR pszText[MAX_LABEL_TEXT];
    LPARAM lParam;
    RECT rItem;
    POINT ptHitTest;
    BOOL fSelected;
    BOOL fHighlighted;
    BOOL m_fHighlightChanged;
} CTC_NMHDR;

class CTeTabItem
{
public:
    CTeTabItem(CString sText, LPARAM lParam);
    void Draw(WTL::CDC& dc, WTL::CFontHandle& font);
    BOOL HitTest(const CPoint& pt) { return (m_rect.PtInRect(pt)) ? TRUE : FALSE; }

    CString						m_sText;
    LPARAM						m_lParam;
    CRect						m_rect;
    BOOL						m_fSelected;
    BOOL						m_fHighlighted;
    BOOL						m_fHighlightChanged;
};