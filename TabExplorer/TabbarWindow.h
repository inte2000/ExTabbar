#pragma once

#include "TeTabctrl.h"
#include "ShellWrapper.h"
#include "ShellBrowserEx.h"
#include "SystemFolders.h"
#include "WzToolBar.h"


enum
{
    ID_SEPARATOR_MY = 0,

    // standard toolbar commands
    ID_SETTINGS = 1,
    ID_NEW_TAB, 
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

class CExplorerWindow;

class CTabbarWindow : public CWindowImpl<CTabbarWindow>
{
public:
    CTabbarWindow();

    DECLARE_WND_CLASS(L"TabExplorer.CTabbarWindow")

    HWND GetToolbar(void) { return m_Toolbar.m_hWnd; }
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, CExplorerWindow* pExplorerWnd);
    void Unintialize();
    void UpdateToolbar(void);

    void MovePosition(const RECT& TabsRect);
    BOOL AddNewTab(const TString& path);
    BOOL NavigateCurrentTab(bool bBack);
    BOOL BeforeNavigate(CIDLEx& target, bool bAutoNav);
    void OnBeforeNavigate(const TString& strUrl);
    void OnNavigateComplete(const TString& strUrl);
protected:

    BEGIN_MSG_MAP(CTabbarWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_CLEAR, OnUpdateUI)
        COMMAND_ID_HANDLER(ID_SETTINGS, OnSettings)
        COMMAND_ID_HANDLER(ID_NEW_TAB, OnNewTab)
        COMMAND_ID_HANDLER(ID_GOUP, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOBACK, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOFORWARD, OnNavigate)
        COMMAND_ID_HANDLER(ID_EMAIL, OnEmail)
        COMMAND_RANGE_HANDLER(ID_CUT, ID_CUSTOM + 100, OnToolbarCommand)
        NOTIFY_CODE_HANDLER(CTCN_NEWTAB, OnTabctrlNewTabButton)
        NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabctrlSelChange)
        NOTIFY_CODE_HANDLER(CTCN_DELETEITEM, OnTabctrlDeleteItem)
        NOTIFY_CODE_HANDLER(NM_RCLICK, OnRClick)
        NOTIFY_CODE_HANDLER(TBN_GETINFOTIP, OnGetInfoTip)
        //NOTIFY_HANDLER_EX(IDC_LIST, LVN_ITEMCHANGED, OnListItemchanged)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

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
    LRESULT OnNewTab(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    LRESULT OnTabctrlNewTabButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnTabctrlSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnTabctrlDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnRClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnGetInfoTip(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    void DrawBackground(CDCHandle& dc, const CRect& rcClient);
    int GetToolbarIconSize();
    BOOL InitToolbarImageList();
    void ReleaseToolbarImageList();

    BOOL CreateToolbarWnd();
    BOOL CreateTabctrlWnd();
    BOOL CreateBarWnd(HWND hParent);
    void DestroyBarWnd();
    void ShowBarWnd(BOOL bShow);
    void GetBarWndRect(RECT& rc);
    void SaveRebarBreakState();

    void InitializeFirstTabOnStartup(const TString& strUrl);
private:
    //CWindow m_Toolbar;
    CWzToolbar m_Toolbar;
    CTeTabCtrl m_TabCtrl;
    CComPtr<IShellBrowser> m_pBrowser;
    //CComPtr<IWebBrowser2> m_spWebBrowser;
    HIMAGELIST m_imgEnabled;
    HIMAGELIST m_imgDisabled;
    bool m_bSubclassRebar; // the rebar needs subclassing
    bool m_bSubclassedRebar; // the rebar is subclassed
    bool m_bBandNewLine; // our band is on a new line (has RBBS_BREAK style)
    CSystemFolders m_sysFolder;
    CShellBrowserEx m_ShellBrowser;
    
    //for parent explorer window
    CExplorerWindow *m_pExplorerWnd;

    bool m_bNavigatedByTab;
    bool m_bInitFirstTabs;
    CIDListData m_curBrowserObj; //get at BeforeNavigate(), and used at OnNavigateComplete()

    void SendShellTabCommand(int command);

    static LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

//class name: UIRibbonCommandBarDock, caption: UIRibbonDockTop
//class name: ShellTabWindowClass, caption: ´ËµçÄÔ


