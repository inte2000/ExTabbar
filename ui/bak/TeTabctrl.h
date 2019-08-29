#pragma once

#include <vector>
#include "TeTabItem.h"
#include "MemDC.h"

#define RECALC_PREV_PRESSED			0
#define RECALC_NEXT_PRESSED			1
#define RECALC_ITEM_SELECTED		2
#define RECALC_RESIZED				3
#define RECALC_FIRST_PRESSED		4
#define RECALC_LAST_PRESSED			5
#define RECALC_EDIT_RESIZED			6

#define MAX_LABEL_TEXT				30


// styles
#define CTCS_FIXEDWIDTH			1		// Makes all tabs the same width. 
#define CTCS_FOURBUTTONS		2		// Four buttons (First, Prev, Next, Last) 
#define CTCS_AUTOHIDEBUTTONS	4		// Auto hide buttons
#define CTCS_TOOLTIPS			8		// Tooltips
#define CTCS_MULTIHIGHLIGHT		16		// Multi highlighted items
#define CTCS_EDITLABELS			32		// Allows item text to be edited in place
#define CTCS_DRAGMOVE			64		// Allows move items
#define CTCS_DRAGCOPY			128		// Allows copy items

// hit test
#define CTCHT_ONFIRSTBUTTON		-1
#define CTCHT_ONPREVBUTTON		-2
#define CTCHT_ONNEXTBUTTON		-3
#define CTCHT_ONLASTBUTTON		-4
#define CTCHT_NOWHERE			-5

// notification messages
#define CTCN_CLICK				1
#define CTCN_RCLICK				2
#define CTCN_SELCHANGE			3
#define CTCN_HIGHLIGHTCHANGE	4
#define CTCN_ITEMMOVE			5
#define CTCN_ITEMCOPY			6
#define CTCN_LABELUPDATE		7
#define CTCN_OUTOFMEMORY		8

#define CTCID_FIRSTBUTTON		-1
#define CTCID_PREVBUTTON		-2
#define CTCID_NEXTBUTTON		-3	
#define CTCID_LASTBUTTON		-4
#define CTCID_NOBUTTON			-5

#define CTCID_EDITCTRL			1

#define REPEAT_TIMEOUT			250

// error codes
#define CTCERR_NOERROR					0
#define CTCERR_OUTOFMEMORY				-1
#define CTCERR_INDEXOUTOFRANGE			-2
#define CTCERR_NOEDITLABELSTYLE			-3
#define CTCERR_NOMULTIHIGHLIGHTSTYLE	-4
#define CTCERR_ITEMNOTSELECTED			-5
#define CTCERR_ALREADYINEDITMODE		-6
#define CTCERR_TEXTTOOLONG				-7
#define CTCERR_NOTOOLTIPSSTYLE			-8
#define CTCERR_CREATETOOLTIPFAILED		-9

// button states
#define BNST_INVISIBLE			0
#define BNST_NORMAL				1
#define BNST_HOT				2
#define BNST_PRESSED			3

class CTeTabCtrl : public CWindowImpl<CTeTabCtrl, CWindow>
{
public:
    CTeTabCtrl();
    virtual ~CTeTabCtrl();

    DECLARE_WND_CLASS(_T("TabExplorer_CTeTabCtrl"))

    BOOL SubclassWindow(HWND hWnd);

    int GetItemCount() { return (int)m_TabItems.size(); }
    int GetCurSel() { return m_nItemSelected; }
    int SetCurSel(int nItem) { return SetCurSel(nItem, FALSE, FALSE); }
    int IsItemHighlighted(int nItem);
    int HighlightItem(int nItem, BOOL fHighlight);
    int GetItemData(int nItem, DWORD_PTR& dwData);
    int SetItemData(int nItem, DWORD_PTR dwData);
    int GetItemText(int nItem, CString& sText);
    int SetItemText(int nItem, const CString& sText);
    int GetItemRect(int nItem, CRect& rect) const;
    int SetItemTooltipText(int nItem, const CString& sText);
    void SetDragCursors(HCURSOR hCursorMove);
    BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);
    void SetControlFont(const LOGFONT& lf, BOOL fRedraw = FALSE);

    int	InsertItem(int nItem, const CString& sText, LPARAM lParam = 0);
    int	DeleteItem(int nItem);
    void DeleteAllItems();
    int MoveItem(int nItemSrc, int nItemDst) { return MoveItem(nItemSrc, nItemDst, FALSE); }
    int CopyItem(int nItemSrc, int nItemDst) { return CopyItem(nItemSrc, nItemDst, FALSE); }
    int HitTest(const CPoint& pt);
    int EditLabel(int nItem) { return EditLabel(nItem, FALSE); }

protected:
    //BEGIN_MSG_MAP(CTeTabCtrl)
        //CHAIN_MSG_MAP(ControlComponent<StdCtrl::CTabCtrl>)

//        MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
//        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
//        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
//        MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
//        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
//        MESSAGE_HANDLER(WM_ENABLE, OnEnable)
//        MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUiState)
    //END_MSG_MAP()

    BEGIN_MSG_MAP_EX(CTeTabCtrl)
        //CHAIN_MSG_MAP(ControlComponent<StdCtrl::CTabCtrl>)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        //MSG_WM_PAINT(OnPaint)
        MSG_WM_ERASEBKGND(OnEraseBackground)
        MSG_WM_SIZE(OnSize)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_TIMER(OnTimer)
        MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(CTCN_SELCHANGE, OnSelectionChanged)
    END_MSG_MAP()

    BOOL Init();
    void DoEraseBackground(CMemDC& dc);
    void DoPaint(CMemDC& dc);
    BOOL StartTrackMouseLeave();
    int	HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
    int SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
    void RecalcEditResized(int nOffset, int nItem);
    void RecalcOffset(int nOffset);
    int RecalcAllTabItems(const CRect& rcClient);
    void RecalcLayout(int nRecalcType, int nItem);
    int MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
    int CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
    BOOL NotifyParent(UINT code, int nItem, const CPoint& pt);
    int EditLabel(int nItem, BOOL fMouseSel);
    
    int RecalcPosition(CRect& rcPrev, CRect& rcNext, CRect& rcTab);

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    BOOL OnEraseBackground(CDCHandle dc);
    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //void OnPaint(CDCHandle dc);
    LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnMouseLeave();
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDblClick(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnUpdateUiState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSize(UINT uType, CSize size);
    LRESULT OnTimer(UINT_PTR timerID);
    LRESULT OnSelectionChanged(LPNMHDR);

protected:
    WTL::CToolTipCtrl m_tipCtrl;
    WTL::CEdit m_editCtrl;
    std::vector<CTeTabItem> m_TabItems;
    CFont m_textFont;
    HCURSOR m_hCursor;
    HCURSOR m_hCursorMove;
    int m_nItemSelected;
    int m_nItemNdxOffset;
    int m_nPrevState;
    int m_nNextState;

    int m_nButtonIDDown;
    int m_dwLastRepeatTime;
    int m_nItemDragDest;
};