#pragma once

#include "TeTabctrl.h"
#include "ShellWrapper.h"
#include "ShellBrowserEx.h"
#include "SystemFolders.h"


enum
{
    ID_SEPARATOR_MY = 0,

    // standard toolbar commands
    ID_SETTINGS = 1,
    ID_GOUP,
    ID_CUT,
    ID_COPY,
    ID_PASTE,
    ID_DELETE,
    ID_PROPERTIES,
    ID_EMAIL,

    ID_LAST, // last standard command

    // additional supported commands
    ID_MOVETO,
    ID_COPYTO,
    ID_UNDO,
    ID_REDO,
    ID_SELECTALL,
    ID_INVERT,
    ID_GOBACK,
    ID_GOFORWARD,
    ID_REFRESH,

    ID_CUSTOM = 100,
};


class CTabbarBandWindow : public CWindowImpl<CTabbarBandWindow>
{
public:
    CTabbarBandWindow();

    DECLARE_WND_CLASS(L"TabExplorer.CTabbarBandWindow")

    BEGIN_MSG_MAP(CTabbarBandWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_CLEAR, OnUpdateUI)
        COMMAND_ID_HANDLER(ID_SETTINGS, OnSettings)
        COMMAND_ID_HANDLER(ID_GOUP, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOBACK, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOFORWARD, OnNavigate)
        COMMAND_ID_HANDLER(ID_EMAIL, OnEmail)
        COMMAND_RANGE_HANDLER(ID_CUT, ID_CUSTOM + 100, OnToolbarCommand)
        NOTIFY_CODE_HANDLER(NM_RCLICK, OnRClick)
        NOTIFY_CODE_HANDLER(TBN_GETINFOTIP, OnGetInfoTip)
        //NOTIFY_HANDLER_EX(IDC_LIST, LVN_ITEMCHANGED, OnListItemchanged)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    HWND GetToolbar(void) { return m_Toolbar.m_hWnd; }
    BOOL OnAttachExplorer(CComPtr<IShellBrowser>& spShellBrowser, CComPtr<IWebBrowser2>& spWebBrowser);
    void OnDetachExplorer();
    BOOL CreateToolbarWnd();
    void UpdateToolbar(void);

    BOOL CreateTabctrlWnd();
    BOOL CreateBarWnd(HWND hParent);
    void DestroyBarWnd();
    void ShowBarWnd(BOOL bShow);
    void GetBarWndRect(RECT& rc);
    void SaveRebarBreakState();

    BOOL AddNewTab(const TString& path, const CIDLEx& cidl);

protected:
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnUpdateUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnNavigate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnToolbarCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnEmail(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnRClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnGetInfoTip(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    int GetToolbarIconSize();
    BOOL InitToolbarImageList();
    void ReleaseToolbarImageList();
private:
    CWindow m_Toolbar;
    //WTL::CTabCtrl m_TabCtrl;
    CTeTabCtrl m_TabCtrl;
    CComPtr<IShellBrowser> m_pBrowser;
    CComPtr<IWebBrowser2> m_spWebBrowser;
    HIMAGELIST m_imgEnabled;
    HIMAGELIST m_imgDisabled;
    bool m_bSubclassRebar; // the rebar needs subclassing
    bool m_bSubclassedRebar; // the rebar is subclassed
    bool m_bBandNewLine; // our band is on a new line (has RBBS_BREAK style)
    CSystemFolders m_sysFolder;
    CShellBrowserEx m_ShellBrowser;
    //for explorer
    HWND m_hExplorerWnd;

    void SendShellTabCommand(int command);

    static LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};
