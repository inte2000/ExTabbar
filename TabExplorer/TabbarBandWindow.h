#pragma once

enum
{
    ID_SEPARATOR = 0,

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

    DECLARE_WND_CLASS(L"TabExplorer.CTabbarBandWindow")

    BEGIN_MSG_MAP(CTabbarBandWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_CLEAR, OnUpdateUI)
        COMMAND_ID_HANDLER(ID_SETTINGS, OnSettings)
        COMMAND_ID_HANDLER(ID_GOUP, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOBACK, OnNavigate)
        COMMAND_ID_HANDLER(ID_GOFORWARD, OnNavigate)
        COMMAND_ID_HANDLER(ID_EMAIL, OnEmail)
        COMMAND_RANGE_HANDLER(ID_CUT, ID_CUSTOM + 100, OnToolbarCommand)
        NOTIFY_CODE_HANDLER(NM_RCLICK, OnRClick)
        NOTIFY_CODE_HANDLER(TBN_GETINFOTIP, OnGetInfoTip)
    END_MSG_MAP()

    CTabbarBandWindow(void);

    HWND GetToolbar(void) { return m_Toolbar.m_hWnd; }
    void OnAttachExplorer(CComPtr<IShellBrowser>& spShellBrowser, CComPtr<IWebBrowser2>& spWebBrowser)
    {
        m_pBrowser = spShellBrowser;
        m_spWebBrowser = spWebBrowser;
    }

    void OnDetachExplorer()
    {
        m_pBrowser.Release();
        m_spWebBrowser.Release();
    }
    void UpdateToolbar(void);

    BOOL CreateBarWnd(HWND hParent);
    void DestroyBarWnd();
    void GetBarWndRect(RECT& rc);
    void SaveRebarBreakState();

protected:
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
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
    CComPtr<IShellBrowser> m_pBrowser;
    CComPtr<IWebBrowser2> m_spWebBrowser;
    HIMAGELIST m_imgEnabled;
    HIMAGELIST m_imgDisabled;
    bool m_bSubclassRebar; // the rebar needs subclassing
    bool m_bSubclassedRebar; // the rebar is subclassed
    bool m_bBandNewLine; // our band is on a new line (has RBBS_BREAK style)

    void SendShellTabCommand(int command);

    static LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};
