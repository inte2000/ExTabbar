#pragma once

class CTeTabCtrl : public CWindowImpl<CTeTabCtrl, WTL::CTabCtrl>
{
public:
    CTeTabCtrl() {};
    ~CTeTabCtrl() {};

    DECLARE_WND_CLASS(_T("TabExplorer_CTeTabCtrl"))

    BOOL SubclassWindow(HWND hWnd);

protected:
    BEGIN_MSG_MAP(CTeTabCtrl)
        //CHAIN_MSG_MAP(ControlComponent<StdCtrl::CTabCtrl>)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//        MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
//        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
//        MESSAGE_HANDLER(WM_PAINT, OnPaint)
//        MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
//        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
//        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
//        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
//        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
//        MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
//        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
//        MESSAGE_HANDLER(WM_ENABLE, OnEnable)
//        MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUiState)
//        MESSAGE_HANDLER(WM_SIZE, OnSize)
    END_MSG_MAP()

    void Init();
    void DoEraseBackground(CDCHandle dc);
    void DoPaint(CDCHandle dc);
    BOOL StartTrackMouseLeave();

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnLButtonDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnUpdateUiState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
    WTL::CToolTipCtrl m_tip;
    HCURSOR m_hCursor;
    bool	m_bHover;
    bool	m_bIsUsingTip;
};