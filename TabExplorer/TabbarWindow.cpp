#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "WspFunctions.h"
#include "DebugLog.h"
#include "ShellFunctions.h"
#include "ShellGuids.h"
#include "GuidEx.h"
#include "ShellBrowserEx.h"
#include "TabbarWindow.h"
#include "ShellTabItem.h"


BOOL g_bUsingLargeButton = FALSE;
int g_nSmallIconSize = 16;
int g_nLargeIconSize = 24;
BOOL g_bToolbarListMode = TRUE; //工具栏按钮上显示文本
BOOL g_bToolbarSameSize = FALSE;
BOOL g_bEnableSettings = TRUE; //显示“设置”按钮
BOOL g_bBandNewLine = FALSE;
BOOL g_bTabFixWidth = TRUE;
BOOL g_bTabNewButton = TRUE;
BOOL g_bTabCloseButton = TRUE;
BOOL g_bTabAutoHideButtons = TRUE;
BOOL g_bForceSysListView = TRUE;

// CTabbarBandWindow - the parent window of the toolbar
struct StdToolbarItem
{
    const wchar_t* name; // default name
    int id;
    bool bShellIcon; //index in shell32.dll or in local dll
    int icon; // icon ID
    const wchar_t* tip; // default tooltip
};

static StdToolbarItem s_StdItems[] = 
{
    {L"NewTab", ID_NEW_TAB, true, 46, L"New Tab"},
    {L"GoUp", ID_GOUP, true, 46, L"Up One Level"},
    {L"Settings", ID_SETTINGS, true, 210, L"Toolbar.Settings"}
};

CTabbarWindow::CTabbarWindow()
{ 
    m_bSubclassRebar = IsWindows7(); // Windows 7
    m_bSubclassedRebar = false;
    m_imgEnabled = NULL;
    m_imgDisabled = NULL;
    m_bNavigatedByTab = false;
    m_bInitFirstTabs = true;
}

BOOL CTabbarWindow::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hExplorerWnd)
{
    m_pBrowser = spShellBrowser;
    m_hExplorerWnd = hExplorerWnd;

    m_ShellBrowser.SetIShellBrowser(spShellBrowser);
    if (g_bForceSysListView)
    {
        m_ShellBrowser.SetUsingListView(true);
    }

    if (!CreateBarWnd(hExplorerWnd))
    {
        return FALSE;
    }

    SetWindowPos(::GetWindow(m_hExplorerWnd, GW_HWNDPREV), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return TRUE;
}

void CTabbarWindow::Unintialize()
{
    DestroyBarWnd();
    m_pBrowser.Release();
}

BOOL CTabbarWindow::CreateToolbarWnd()
{
    // create buttons
    int iconCount = sizeof(s_StdItems) / sizeof(s_StdItems[0]);
    std::vector<TBBUTTON> buttons(iconCount);
    for (int i = 0; i < iconCount; i++)
    {
        TBBUTTON& button = buttons[i];

        button.idCommand = s_StdItems[i].id;
        button.dwData = i;

        if (s_StdItems[i].id == ID_SEPARATOR_MY)
            button.fsStyle = BTNS_SEP;
        else
        {
            button.iBitmap = i;
            button.fsState = TBSTATE_ENABLED;//0
            button.fsStyle = BTNS_BUTTON | BTNS_NOPREFIX;
            if (!g_bToolbarSameSize)
                button.fsStyle |= BTNS_AUTOSIZE;
            if (!g_bToolbarListMode)
            {
                button.fsStyle |= BTNS_SHOWTEXT;
                button.iString = (INT_PTR)s_StdItems[i].name;
            }
        }
    }

    DWORD dwStyle = WS_CHILD | TBSTYLE_CUSTOMERASE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE;// | TBSTYLE_TRANSPARENT;
    if (!g_bToolbarListMode)
        m_Toolbar.Create(m_hWnd, NULL, NULL, dwStyle, TBSTYLE_EX_MIXEDBUTTONS);
    else
        m_Toolbar.Create(m_hWnd, NULL, NULL, dwStyle | TBSTYLE_LIST, TBSTYLE_EX_MIXEDBUTTONS);

    if (!m_Toolbar.IsWindow())
        return FALSE;

    m_Toolbar.SetButtonStructSize(sizeof(TBBUTTON));
    m_Toolbar.SetMaxTextRows(1);

    LogInfo(_T("CTabbarBandWindow create toolbar window = 0x%08x...!"), m_Toolbar.m_hWnd);

    HIMAGELIST old = m_Toolbar.SetImageList(m_imgEnabled, 0);
    if (old)
        ImageList_Destroy(old);

    old = m_Toolbar.SetDisabledImageList(m_imgDisabled, 0);
    if (old)
        ImageList_Destroy(old);

    m_Toolbar.AddButtons((int)buttons.size(), &buttons[0]);
    m_Toolbar.ShowWindow(SW_SHOW);

#if 0
    // create the toolbar
    if (!g_bToolbarListMode)
        m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"", WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE, 0, 0, 10, 10, m_hWnd, (HMENU)101, g_Instance, NULL);
    else
        m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"", WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE, 0, 0, 10, 10, m_hWnd, (HMENU)101, g_Instance, NULL);

    if (!m_Toolbar.IsWindow())
        return FALSE;

    m_Toolbar.SendMessage(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);
    m_Toolbar.SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON));
    m_Toolbar.SendMessage(TB_SETMAXTEXTROWS, 1);

    LogInfo(_T("CTabbarBandWindow create toolbar window = 0x%08x...!"), m_Toolbar.m_hWnd);

    // add buttons
    HIMAGELIST old = (HIMAGELIST)m_Toolbar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)m_imgEnabled);
    if (old) 
        ImageList_Destroy(old);
    old = (HIMAGELIST)m_Toolbar.SendMessage(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)m_imgDisabled);
    if (old) 
        ImageList_Destroy(old);
    
    m_Toolbar.SendMessage(TB_ADDBUTTONS, buttons.size(), (LPARAM)& buttons[0]);
    m_Toolbar.ShowWindow(SW_SHOW);
#endif
    SendMessage(WM_CLEAR);

    return TRUE;
}

void CTabbarWindow::UpdateToolbar(void)
{
    // disable the Up button if we are at the top level
    bool bDesktop = false;
    if (m_pBrowser)
    {
        CComPtr<IShellView> pView;
        m_pBrowser->QueryActiveShellView(&pView);
        if (pView)
        {
            CComQIPtr<IFolderView> pView2 = pView;
            if (pView2)
            {
                CComPtr<IPersistFolder2> pFolder;
                pView2->GetFolder(IID_IPersistFolder2, (void**)& pFolder);
                if (pFolder)
                {
                    LPITEMIDLIST pidl;
                    pFolder->GetCurFolder(&pidl);
                    if (ILIsEmpty(pidl))
                        bDesktop = true; // only the top level has empty PIDL
                    CoTaskMemFree(pidl);
                }
            }
        }
    }
    
    m_Toolbar.SendMessage(TB_ENABLEBUTTON, ID_GOUP, bDesktop ? 0 : 1);
}

BOOL CTabbarWindow::CreateTabctrlWnd()
{
/*
    DWORD dwTabStyles = 
    if (controlsSettings.TabsOnBottom()) dwTabStyles |= CTCS_BOTTOM;
    if (!controlsSettings.HideTabCloseButton()) dwTabStyles |= ;
    if (!controlsSettings.HideTabNewButton()) dwTabStyles |= ;
    if (g_settingsHandler->GetBehaviorSettings2().closeSettings.bAllowClosingLastView) dwTabStyles |= CTCS_CLOSELASTTAB;
*/

    //CTCS_EDITLABELS
    RECT rcTabCtrl = { 0, 0, 10, 10 };
    DWORD dwTabStyle = CTCS_TOOLTIPS | CTCS_FIXEDWIDTH | CTCS_SCROLL | CTCS_HOTTRACK;// | CTCS_BUTTONS;
/*
    if (g_bTabFixWidth)
        dwTabStyle |= CTCS_FIXEDWIDTH;
*/
    if (g_bTabNewButton)
        dwTabStyle |= CTCS_NEWTABBUTTON;
    if (g_bTabCloseButton)
        dwTabStyle |= CTCS_CLOSEBUTTON;

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwTabStyle;
    if (m_TabCtrl.Create(m_hWnd, rcTabCtrl, NULL, dwStyle) == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CTabbarWindow::CreateBarWnd(HWND hParent)
{
    LogTrace(_T("CTabbarBandWindow::CreateBarWnd() invoked"));
    if (Create(hParent, NULL, NULL, WS_CHILD|WS_VISIBLE) != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

void CTabbarWindow::DestroyBarWnd()
{
    LogTrace(_T("CTabbarBandWindow::DestroyBarWnd() invoked"));
    if (m_bSubclassedRebar)
    {
        RemoveWindowSubclass(::GetParent(m_Toolbar.m_hWnd), RebarSubclassProc, (UINT_PTR)this);
        m_bSubclassedRebar = false;
    }

    if (IsWindow())
        DestroyWindow();
}

void CTabbarWindow::ShowBarWnd(BOOL bShow)
{
    //ShowWindow(m_BandWindow.GetToolbar(),fShow?SW_SHOW:SW_HIDE);
    RECT rc = { 0 };
    GetWindowRect(&rc);
    ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    m_Toolbar.ShowWindow(bShow?SW_SHOW:SW_HIDE);
    m_TabCtrl.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

void CTabbarWindow::GetBarWndRect(RECT& rc)
{
    // initializes the band
    if (m_bSubclassRebar && !m_bSubclassedRebar)
    {
        m_bBandNewLine = g_bBandNewLine;

        HWND hRebarWnd = ::GetParent(m_Toolbar.m_hWnd);
        wchar_t className[256];
        GetClassName(hRebarWnd, className, _countof(className));
        if (_wcsicmp(className, REBARCLASSNAME) == 0)
        {
            SetWindowSubclass(hRebarWnd, RebarSubclassProc, (UINT_PTR)this, (DWORD_PTR)m_Toolbar.m_hWnd);
            m_bSubclassedRebar = true;
        }
    }

    if (m_Toolbar.m_hWnd != NULL)
    {
        int count = (int)SendMessage(m_Toolbar.m_hWnd, TB_BUTTONCOUNT, 0, 0);
        SendMessage(m_Toolbar.m_hWnd, TB_GETITEMRECT, count - 1, (LPARAM)& rc);
    }

    rc.left = 0;
    rc.top = 0;
    rc.right = 200;
    rc.bottom = g_nSmallIconSize ? 26 : 34;
}

void CTabbarWindow::SaveRebarBreakState()
{
    if (m_bSubclassedRebar)
    {
        HWND hRebarWnd = ::GetParent(m_Toolbar.m_hWnd);
        int n = (int)SendMessage(hRebarWnd, RB_GETBANDCOUNT, 0, 0);
        for (int i = 0; i < n; i++)
        {
            REBARBANDINFO info = { sizeof(info),RBBIM_STYLE | RBBIM_CHILD };
            SendMessage(hRebarWnd, RB_GETBANDINFO, i, (LPARAM)& info);
            if (info.hwndChild == m_Toolbar.m_hWnd)
            {
                m_bBandNewLine = (info.fStyle & RBBS_BREAK) != 0;
                g_bBandNewLine = m_bBandNewLine;   //保存全局变量的值

                break;
            }
        }
    }
}

BOOL CTabbarWindow::GetCIDLDataByParseName(const TString& parseName, CIDLEx& cidl, CIDListData& IdlData)
{
    if (IsNamespacePath(parseName))
    {
        if (!m_shlFolderMap.FindFolder(parseName, IdlData))
        {
            //log here
        }
        
        auto rawIdl = IdlData.GetIDLData();
        cidl.CreateByIdListData(std::get<0>(rawIdl), std::get<1>(rawIdl));
    }
    else
    {
        cidl = CIDLEx::CIDLFromFullPath(parseName);
        IdlData = CIDListData(&cidl, parseName);
    }

    return TRUE;
}

void CTabbarWindow::InitializeFirstTabOnStartup(const TString& strUrl)
{
    if (!m_bInitFirstTabs)
        return;

    if (!AddNewTab(strUrl))
    {
    }

    m_bInitFirstTabs = false;
}

void CTabbarWindow::MovePosition(const RECT& TabsRect)
{
    LogTrace(_T("CTabbarWnd::MovePosition(TabPos [left = %d, top = %d, right = %d, bottom = %d])"),
        TabsRect.left, TabsRect.top, TabsRect.right, TabsRect.bottom);

    ::SetWindowPos(m_hWnd, HWND_BOTTOM, TabsRect.left, TabsRect.top,
            TabsRect.right - TabsRect.left, TabsRect.bottom - TabsRect.top, SWP_DRAWFRAME);
    RedrawWindow();
}

BOOL CTabbarWindow::AddNewTab(const TString& path)
{
    CShellTabItem* psti = new CShellTabItem();
    if (psti != nullptr)
    {
        CIDLEx cidl;
        CIDListData IdlData;
        if (!GetCIDLDataByParseName(path, cidl, IdlData))
        {
        }
        
        psti->NavigatedTo(IdlData, cidl, -1, true);
        int iconIdx = GetShellObjectIcon(cidl);
        int index = m_TabCtrl.InsertItem(0, psti->GetTitle().c_str(), iconIdx, psti->GetTooltip().c_str());
        if (index >= 0)
        {
            m_TabCtrl.SetItemData(index, (ULONG_PTR)psti);
        }
    }

    return TRUE;
}

BOOL CTabbarWindow::NavigateCurrentTab(bool bBack)
{
    int nItem = m_TabCtrl.GetCurSel();
    if (nItem >= 0)
    {
        CShellTabItem* psti = (CShellTabItem*)m_TabCtrl.GetItemData(nItem);
        ATLASSERT(psti != nullptr);
        std::shared_ptr<CNavigatedPoint> curNp;
        if (bBack)
            psti->GoBackward(curNp);
        else
            psti->GoForward(curNp);

        m_TabCtrl.SetItemInfo(nItem, -1, curNp->GetTitle().c_str(), curNp->GetTooltip().c_str());

    }
    return TRUE;
}

BOOL CTabbarWindow::BeforeNavigate(CIDLEx& target, bool bAutoNav)
{
    TString parseName = target.GetParseName();
    m_shlFolderMap.InsertFolder(parseName, target);
    if (!m_bNavigatedByTab)
    {
        int nItem = m_TabCtrl.GetCurSel();
        if (nItem >= 0)
        {
            CShellTabItem* psti = (CShellTabItem*)m_TabCtrl.GetItemData(nItem);
            ATLASSERT(psti != nullptr);
        }
    }

    return FALSE; //返回TRUE，阻止切换目录
}

void CTabbarWindow::OnBeforeNavigate(const TString& strUrl)
{
    InitializeFirstTabOnStartup(strUrl);
}

void CTabbarWindow::OnNavigateComplete(const TString& strUrl)
{
    InitializeFirstTabOnStartup(strUrl);

    int nItem = m_TabCtrl.GetCurSel();
    if (nItem >= 0)
    {
        CShellTabItem* psti = (CShellTabItem*)m_TabCtrl.GetItemData(nItem);
        ATLASSERT(psti != nullptr);

        CIDLEx cidl;
        CIDListData IdlData;
        if (!GetCIDLDataByParseName(strUrl, cidl, IdlData))
        {
            //log here
        }

        psti->NavigatedTo(IdlData, cidl, 0, false);
        int iconIdx = GetShellObjectIcon(cidl);
        m_TabCtrl.SetItemInfo(nItem, iconIdx, psti->GetTitle().c_str(), psti->GetTooltip().c_str());
        if (m_bNavigatedByTab)
        {
            TString focusPath;
            const std::vector<CIDListData*>& items = psti->GetCurrentStatus(focusPath);
            HRESULT hr = m_ShellBrowser.SetSelectedItems(items, focusPath);
            if (hr != S_OK)
            {
            }
        }
    }
    m_bNavigatedByTab = false;
}

/*
private QTabItem CreateNewTab(IDLWrapper idlw) {
    string path = idlw.Path;
    QTabItem tab = new QTabItem(QTUtility2.MakePathDisplayText(path, false), path, tabControl1);
    tab.NavigatedTo(path, idlw.IDL, -1, false);
    tab.ToolTipText = QTUtility2.MakePathDisplayText(path, true);
    AddInsertTab(tab);
    return tab;
}
*/

int CTabbarWindow::GetToolbarIconSize()
{
    int iconSize = 0;
    if (g_bUsingLargeButton)
    {
        iconSize = g_nLargeIconSize;
    }
    else
    {
        iconSize = g_nSmallIconSize;
    }

    if (iconSize == 0)  //no preset config?
    {
        // pick icon size based on the DPI setting
        HDC hdc = ::GetDC(NULL);
        int dpi = ::GetDeviceCaps(hdc, LOGPIXELSY);
        ::ReleaseDC(NULL, hdc);
        if (dpi >= 120)
            iconSize = g_bUsingLargeButton ? 32 : 24;
        else
            iconSize = g_bUsingLargeButton ? 24 : 16;
    }
    /*
    else if (iconSize < 8) iconSize = 8;
    else if (iconSize > 128) iconSize = 128;
    */

    return iconSize;
}

BOOL CTabbarWindow::InitToolbarImageList()
{
    int iconCount = sizeof(s_StdItems) / sizeof(s_StdItems[0]);
    int iconSize = GetToolbarIconSize();

    m_imgEnabled = ::ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK | (IsLanguageRTL() ? ILC_MIRROR : 0), iconCount, 2);
    m_imgDisabled = ::ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK | (IsLanguageRTL() ? ILC_MIRROR : 0), iconCount, 2);

    if ((m_imgEnabled == NULL) || (m_imgDisabled == NULL))
        return FALSE;

    HMODULE hShell32 = ::GetModuleHandle(L"Shell32.dll");

    // create buttons
    for (int i = 0; i < iconCount; i++)
    {
        HICON hIcon = NULL;
        if(s_StdItems[i].bShellIcon)
        {
            hIcon = (HICON)::LoadImage(hShell32, MAKEINTRESOURCE(s_StdItems[i].icon), IMAGE_ICON, 
                                             iconSize, iconSize, LR_DEFAULTCOLOR);
        }
        else
        {
            hIcon = (HICON)::LoadImage(g_Instance, MAKEINTRESOURCE(s_StdItems[i].icon), IMAGE_ICON,
                iconSize, iconSize, LR_DEFAULTCOLOR);
        }

        if (hIcon)
        {
            ImageList_AddIcon(m_imgEnabled, hIcon);
            HICON hIcon2 = CreateDisabledIcon(hIcon, iconSize);
            ImageList_AddIcon(m_imgDisabled, hIcon2);
            DestroyIcon(hIcon);
            DestroyIcon(hIcon2);
        }
    }

    return TRUE;
}

void CTabbarWindow::ReleaseToolbarImageList()
{
    ImageList_Destroy(m_imgEnabled);
    m_imgEnabled = NULL;
    ImageList_Destroy(m_imgDisabled);
    m_imgDisabled = NULL;
}

LRESULT CTabbarWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LogTrace(_T("CTabbarBandWindow::OnCreate() invoked"));
    if (!InitToolbarImageList())
    {
        LogError(_T("CTabbarBandWindow call InitToolbarImageList() failed!"));
        return -1;
    }

    LogInfo(_T("CTabbarBandWindow create toolbar buttons...!"));
    if (!CreateToolbarWnd())
    {
        LogError(_T("CTabbarBandWindow call CreateToolbarWnd() failed!"));
        return -1;
    }
    
    LogInfo(_T("CTabbarBandWindow create tabctrl ...!"));
    if (!CreateTabctrlWnd())
    {
        LogError(_T("CTabbarBandWindow call CreateTabctrlWnd() failed!"));
        return -1;
    }

    return 0;
}

LRESULT CTabbarWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    DWORD_PTR dwSizeType = wParam;      // resizing flag 
    WORD nWidth = LOWORD(lParam);  // width of client area 
    WORD nHeight = HIWORD(lParam); // height of client area 

    if (!m_Toolbar.IsWindow() || !m_TabCtrl.IsWindow())
        return 0;

    RECT rcLastBtn;
    int count = (int)m_Toolbar.SendMessage(TB_BUTTONCOUNT, 0, 0);
    m_Toolbar.SendMessage(TB_GETITEMRECT, count - 1, (LPARAM)&rcLastBtn);

    int vSpace = (nHeight - rcLastBtn.bottom) / 2;
    RECT rcToolbar = { 0, vSpace , rcLastBtn.right, rcLastBtn.bottom + vSpace };
    m_Toolbar.MoveWindow(&rcToolbar, TRUE);

    RECT rcTabctrl = { rcLastBtn.right + 2, 0, nWidth, nHeight }; //流出分隔线的空间
    m_TabCtrl.MoveWindow(&rcTabctrl, TRUE);
    
    bHandled = TRUE;
    return 0;
}

void CTabbarWindow::DrawBackground(CDCHandle& dc, const CRect& rcClient)
{
    CRect rcToolbar;

    m_Toolbar.GetWindowRect(&rcToolbar);
    int leftOffset = rcClient.left + rcToolbar.Width() + 1;

    DrawThemeParentBackground(m_hWnd, dc, &rcClient);
    
    WTL::CPen pen;
    pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
    WTL::CPenHandle penOld = dc.SelectPen(pen);
    dc.MoveTo(leftOffset, rcClient.top + 3);
    dc.LineTo(leftOffset, rcClient.bottom - 3);
    dc.SelectPen(penOld);
}

LRESULT CTabbarWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    if (wParam != NULL)
    {
        CDCHandle dc((HDC)wParam);
        DrawBackground(dc, rcClient);
    }
    else
    {
        PAINTSTRUCT PaintStruct;
        HDC hDC = BeginPaint(&PaintStruct);
        CDCHandle dc(hDC);
        DrawBackground(dc, rcClient);
        EndPaint(&PaintStruct);
    }
    bHandled = TRUE;
    return 0;
}

LRESULT CTabbarWindow::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HDC hDC = (HDC)wParam;
    
    //RECT rcClient;
    //GetClientRect(&rcClient);
    //FillSolidRect(hDC, &rcClient, RGB(0,0,255));
    //DrawThemeParentBackground(m_hWnd, hDC, &rcClient);
    bHandled = TRUE;
    return 1;
}

LRESULT CTabbarWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LogTrace(_T("CTabbarBandWindow::OnDestroy() invoked"));
    ReleaseToolbarImageList();
    bHandled = FALSE;
    return 0;
}

LRESULT CTabbarWindow::OnUpdateUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // update the state of the custom buttons based on the registry settings
/*
    CRegKey regSettings;
    if (regSettings.Open(HKEY_CURRENT_USER, L"Software\\IvoSoft\\ClassicExplorer") == ERROR_SUCCESS)
    {
        for (std::vector<StdToolbarItem>::const_iterator it = m_Items.begin(); it != m_Items.end(); ++it)
        {
            if (!it->regName.empty())
            {
                DWORD val;
                if (regSettings.QueryDWORDValue(it->regName.c_str(), val) != ERROR_SUCCESS)
                    val = 0;
                m_Toolbar.SendMessage(TB_ENABLEBUTTON, it->id, (val & 1) ? 0 : 1);
                m_Toolbar.SendMessage(TB_CHECKBUTTON, it->id, (val & 2) ? 1 : 0);
            }
        }
    }
*/
    return 0;
}

// Go to the parent folder
LRESULT CTabbarWindow::OnNavigate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if (m_pBrowser)
    {
        UINT flags = (GetKeyState(VK_CONTROL) < 0 ? SBSP_NEWBROWSER : SBSP_SAMEBROWSER);
        if (wID == ID_GOUP)
            m_pBrowser->BrowseObject(NULL, flags | SBSP_PARENT);
        if (wID == ID_GOBACK)
            m_pBrowser->BrowseObject(NULL, flags | SBSP_NAVIGATEBACK);
        if (wID == ID_GOFORWARD)
            m_pBrowser->BrowseObject(NULL, flags | SBSP_NAVIGATEFORWARD);
    }

    return TRUE;
}

void CTabbarWindow::SendShellTabCommand(int command)
{
    // sends a command to the ShellTabWindowClass window
    for (CWindow parent = GetParent(); parent.m_hWnd; parent = parent.GetParent())
    {
        // find a parent window with class ShellTabWindowClass
        wchar_t name[256];
        GetClassName(parent.m_hWnd, name, _countof(name));
        if (_wcsicmp(name, L"ShellTabWindowClass") == 0)
        {
            parent.SendMessage(WM_COMMAND, command);
            break;
        }
    }
}

// Executes a cut/copy/paste/delete command
LRESULT CTabbarWindow::OnToolbarCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
#if 0
    if (wID >= ID_CUSTOM)
    {
        int idx = wID - ID_CUSTOM;
        wchar_t buf[2048];
        wcscpy_s(buf, m_Items[idx].command);
        DoEnvironmentSubst(buf, _countof(buf));
        wchar_t* pBuf = buf;
        bool bArg1 = wcsstr(buf, L"%1") != NULL;
        bool bArg2 = wcsstr(buf, L"%2") != NULL;
        wchar_t path[_MAX_PATH];
        wchar_t file[_MAX_PATH];
        path[0] = file[0] = 0;

        CComPtr<IShellView> pView;
        if (SUCCEEDED(m_pBrowser->QueryActiveShellView(&pView)))
        {
            CComPtr<IPersistFolder2> pFolder;
            LPITEMIDLIST pidl;
            CComQIPtr<IFolderView> pView2 = pView;
            if (pView2 && SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)& pFolder)) && SUCCEEDED(pFolder->GetCurFolder(&pidl)))
            {
                // get current path
                SHGetPathFromIDList(pidl, path);
                if (bArg2)
                {
                    CComPtr<IEnumIDList> pEnum;
                    int count;
                    // if only one file is selected get the file name (%2)
                    if (SUCCEEDED(pView2->ItemCount(SVGIO_SELECTION, &count)) && count == 1 && SUCCEEDED(pView2->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)& pEnum)) && pEnum)
                    {
                        PITEMID_CHILD child;
                        if (pEnum->Next(1, &child, NULL) == S_OK)
                        {
                            LPITEMIDLIST full = ILCombine(pidl, child);
                            SHGetPathFromIDList(full, file);
                            ILFree(child);
                            ILFree(full);
                        }
                    }
                }
                ILFree(pidl);
            }
        }

        if (bArg1 || bArg2)
        {
            // expand environment variables, %1, %2
            DWORD_PTR args[100] = { (DWORD_PTR)path,(DWORD_PTR)file };
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING, buf, 0, 0, (LPWSTR)& pBuf, 0, (va_list*)args);
        }

        wchar_t exe[_MAX_PATH];
        const wchar_t* params = GetToken(pBuf, exe, _countof(exe), L" ");
        ShellExecute(NULL, NULL, exe, params, path, SW_SHOWNORMAL);
        if (pBuf != buf)
            LocalFree(pBuf);
        return TRUE;
    }
#endif
    // check if the focus is on the tree side or on the list side
    CWindow focus = GetFocus();
    wchar_t name[256];
    GetClassName(focus, name, _countof(name));
    CWindow parent = focus.GetParent();
    if (_wcsicmp(name, WC_TREEVIEW) == 0)
    {
        // send these commands to the parent of the tree view
        if (wID == ID_CUT)
            parent.SendMessage(WM_COMMAND, 41025);
        if (wID == ID_COPY)
            parent.SendMessage(WM_COMMAND, 41026);
        if (wID == ID_PASTE)
            parent.SendMessage(WM_COMMAND, 41027);
        if (wID == ID_DELETE)
            parent.SendMessage(WM_COMMAND, 40995);
        //if (wID == ID_PROPERTIES)
        //    ShowTreeProperties(focus.m_hWnd);
    }
    else
    {
        GetClassName(parent, name, _countof(name));
        if (_wcsicmp(name, L"SHELLDLL_DefView") == 0)
        {
            // send these commands to the SHELLDLL_DefView window
            if (wID == ID_CUT)
            {
                parent.SendMessage(WM_COMMAND, 28696);
                focus.InvalidateRect(NULL);
            }
            if (wID == ID_COPY)
                parent.SendMessage(WM_COMMAND, 28697);
            if (wID == ID_PASTE)
                parent.SendMessage(WM_COMMAND, 28698);
            if (wID == ID_DELETE)
                parent.SendMessage(WM_COMMAND, 28689);
            if (wID == ID_PROPERTIES)
                parent.SendMessage(WM_COMMAND, 28691);
            if (wID == ID_COPYTO)
                parent.SendMessage(WM_COMMAND, 28702);
            if (wID == ID_MOVETO)
                parent.SendMessage(WM_COMMAND, 28703);
        }
    }

    if (wID == ID_UNDO)
        SendShellTabCommand(28699);
    if (wID == ID_REDO)
        SendShellTabCommand(28704);
    if (wID == ID_SELECTALL)
        SendShellTabCommand(28705);
    if (wID == ID_INVERT)
        SendShellTabCommand(28706);
    if (wID == ID_REFRESH)
        SendShellTabCommand(41504);

    return TRUE;
}

LRESULT CTabbarWindow::OnEmail(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    const IID CLSID_SendMail = { 0x9E56BE60,0xC50F,0x11CF,{0x9A,0x2C,0x00,0xA0,0xC9,0x0A,0x90,0xCE} };

    CComPtr<IShellView> pView;
    if (FAILED(m_pBrowser->QueryActiveShellView(&pView))) return TRUE;

    // check if there is anything selected
    CComQIPtr<IFolderView> pView2 = pView;
    int count;
    if (pView2 && SUCCEEDED(pView2->ItemCount(SVGIO_SELECTION, &count)) && count == 0) return TRUE;

    // get the data object
    CComPtr<IDataObject> pDataObj;
    if (FAILED(pView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)& pDataObj))) return TRUE;
    //CComQIPtr<IAsyncOperation> pAsync = pDataObj;
    CComQIPtr<IDataObjectAsyncCapability> pAsync = pDataObj;
    if (pAsync)
        pAsync->SetAsyncMode(FALSE);

    // drop into the SendMail handler
    CComPtr<IDropTarget> pDropTarget;
    if (SUCCEEDED(CoCreateInstance(CLSID_SendMail, NULL, CLSCTX_ALL, IID_IDropTarget, (void**)& pDropTarget)))
    {
        POINTL pt = { 0,0 };
        DWORD dwEffect = 0;
        pDropTarget->DragEnter(pDataObj, MK_LBUTTON, pt, &dwEffect);
        pDropTarget->Drop(pDataObj, 0, pt, &dwEffect);
    }
    return TRUE;
}

// Show the settings dialog
LRESULT CTabbarWindow::OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    //ShowSettings(m_hWnd);
    ::MessageBox(m_hWnd, _T("fdgl;gjlksdf ;drgkj;lsdf g"), _T("TabExplorer"), MB_OK);
    return TRUE;
}

LRESULT CTabbarWindow::OnNewTab(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    TString path = GetMyComputerPath();
    
    if (!AddNewTab(path))
    {
        ::MessageBox(m_hWnd, _T("add new tab fail!"), _T("TabExplorer"), MB_OK);
    }

    return TRUE;
}

LRESULT CTabbarWindow::OnTabctrlNewTabButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTCITEM* pNtItem = reinterpret_cast<NMCTCITEM*>(pnmh);
    //if(pNtItem->pt.x, pNtItem->pt.y)

    return 0;
}

LRESULT CTabbarWindow::OnTabctrlSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTC2ITEMS* pNmItem = reinterpret_cast<NMCTC2ITEMS *>(pnmh);
    
    if (pNmItem->iItem1 >= 0)
    {
        CShellTabItem* psti = (CShellTabItem*)m_TabCtrl.GetItemData(pNmItem->iItem1);
        ATLASSERT(psti != nullptr);
        std::vector<CIDListData*> selectedItems;
        HRESULT hr = m_ShellBrowser.GetSelectedItems(selectedItems, false);
        if (hr == S_OK)
        {
            CIDLEx focusItem = m_ShellBrowser.GetFocusedItem();
            TString focusPath = focusItem.GetParseName();
            psti->SetCurrentStatus(focusPath, selectedItems);
        }
    }
    CShellTabItem * pti = (CShellTabItem *)m_TabCtrl.GetItemData(pNmItem->iItem2);
    if (pti != NULL)
    {
        m_bNavigatedByTab = true;
        auto curRawIdl = pti->GetCurrentIDLData();
        CIDLEx cidl(std::get<0>(curRawIdl), std::get<1>(curRawIdl));
        m_ShellBrowser.Navigate(cidl);
    }

    return 0;
}

LRESULT CTabbarWindow::OnTabctrlDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTCITEM* pNmItem = (NMCTCITEM*)pnmh;
    
    CShellTabItem * pti = (CShellTabItem *)m_TabCtrl.GetItemData(pNmItem->iItem);
    if (pti != NULL)
    {
        delete pti;
    }

    return 0;
}

LRESULT CTabbarWindow::OnRClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMMOUSE* pInfo = (NMMOUSE*)pnmh;
    POINT pt = pInfo->pt;
    {
        RECT rc;
        
        int count = m_Toolbar.GetButtonCount();// (int)m_Toolbar.SendMessage(TB_BUTTONCOUNT);
        m_Toolbar.GetItemRect(count - 1, &rc);
        //m_Toolbar.SendMessage(TB_GETITEMRECT, count - 1, (LPARAM)& rc);
        if (pt.x > rc.right)
            return 0;
    }
    m_Toolbar.ClientToScreen(&pt);
    //ShowSettingsMenu(m_hWnd, pt.x, pt.y);
    return 1;
}

LRESULT CTabbarWindow::OnGetInfoTip(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMTBGETINFOTIP* pTip = (NMTBGETINFOTIP*)pnmh;
    const StdToolbarItem& item = s_StdItems[pTip->lParam];
    if (item.tip != NULL)
    {
        // show the tip for the standard item
        wcscpy_s(pTip->pszText, pTip->cchTextMax, item.tip);
    }
    return 0;
}

// Subclasses the rebar control on Windows 7. Makes sure the RBBS_BREAK style is properly set. Windows 7 has a bug
// that forces RBBS_BREAK for every rebar band
LRESULT CALLBACK CTabbarWindow::RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == RB_SETBANDINFO || uMsg == RB_INSERTBAND)
    {
        LogDebug(_T("CTabbarBandWindow::RebarSubclassProc() process RB_SETBANDINFO or RB_INSERTBAND for win7 bugfix"));
        REBARBANDINFO* pInfo = (REBARBANDINFO*)lParam;
        if ((pInfo->hwndChild == (HWND)dwRefData) && (pInfo->fMask & RBBIM_STYLE))
        {
            if (((CTabbarWindow*)uIdSubclass)->m_bBandNewLine)
                pInfo->fStyle |= RBBS_BREAK;
            else
                pInfo->fStyle &= ~RBBS_BREAK;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
