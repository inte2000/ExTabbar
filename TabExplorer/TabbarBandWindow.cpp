#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "TabbarBandWindow.h"


BOOL g_bUsingLargeButton = FALSE;
int g_nSmallIconSize = 16;
int g_nLargeIconSize = 24;
BOOL g_bToolbarListMode = TRUE; //工具栏按钮上显示文本
BOOL g_bToolbarSameSize = FALSE;
BOOL g_bEnableSettings = TRUE; //显示“设置”按钮
BOOL g_bBandNewLine = FALSE;

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
    {L"GoUp", ID_GOUP, true, 46, L"Up One Level"},
    {L"Cut", ID_CUT, true, 16762, NULL},
    {L"Copy", ID_COPY, true, 243, NULL},
    {L"Paste", ID_PASTE, true, 16763, NULL},
    {L"Delete", ID_DELETE, true, 240, L"Toolbar.Delete"},
    {L"Properties", ID_PROPERTIES, true, 253, L"Toolbar.Properties"},
    {L"E-mail the selected items", ID_EMAIL, true, 265, L"Toolbar.Email"},
    {L"Settings", ID_SETTINGS, true, 210, L"Toolbar.Settings"}
};

CTabbarBandWindow::CTabbarBandWindow() 
{ 
    m_bSubclassRebar = IsWindows7(); // Windows 7
    m_bSubclassedRebar = false;
    m_imgEnabled = NULL;
    m_imgDisabled = NULL;
}

void CTabbarBandWindow::UpdateToolbar(void)
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

BOOL CTabbarBandWindow::CreateBarWnd(HWND hParent)
{
    if (Create(hParent, NULL, NULL, WS_CHILD) != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

void CTabbarBandWindow::DestroyBarWnd()
{
    if (m_bSubclassedRebar)
    {
        RemoveWindowSubclass(::GetParent(m_Toolbar.m_hWnd), RebarSubclassProc, (UINT_PTR)this);
        m_bSubclassedRebar = false;
    }

    if (IsWindow())
        DestroyWindow();

    OnDetachExplorer();
}

void CTabbarBandWindow::GetBarWndRect(RECT& rc)
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
}

void CTabbarBandWindow::SaveRebarBreakState()
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

int CTabbarBandWindow::GetToolbarIconSize()
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

BOOL CTabbarBandWindow::InitToolbarImageList()
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

void CTabbarBandWindow::ReleaseToolbarImageList()
{
    ImageList_Destroy(m_imgEnabled);
    m_imgEnabled = NULL;
    ImageList_Destroy(m_imgDisabled);
    m_imgDisabled = NULL;
}

LRESULT CTabbarBandWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (!InitToolbarImageList())
    {
        return -1;
    }

    // create buttons
    int iconCount = sizeof(s_StdItems) / sizeof(s_StdItems[0]);
    std::vector<TBBUTTON> buttons(iconCount);
    for (int i = 0; i < iconCount; i++)
    {
        TBBUTTON& button = buttons[i];

        button.idCommand = s_StdItems[i].id;
        button.dwData = i;

        if (s_StdItems[i].id == ID_SEPARATOR)
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

    // create the toolbar
    if (!g_bToolbarListMode)
        m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"", WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE, 0, 0, 10, 10, m_hWnd, (HMENU)101, g_Instance, NULL);
    else
        m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"", WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE, 0, 0, 10, 10, m_hWnd, (HMENU)101, g_Instance, NULL);

    m_Toolbar.SendMessage(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);
    m_Toolbar.SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON));
    m_Toolbar.SendMessage(TB_SETMAXTEXTROWS, 1);


    // add buttons
    HIMAGELIST old = (HIMAGELIST)m_Toolbar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)m_imgEnabled);
    if (old) ImageList_Destroy(old);
    old = (HIMAGELIST)m_Toolbar.SendMessage(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)m_imgDisabled);
    if (old) ImageList_Destroy(old);
    m_Toolbar.SendMessage(TB_ADDBUTTONS, buttons.size(), (LPARAM)& buttons[0]);
    SendMessage(WM_CLEAR);
    
    return 0;
}

LRESULT CTabbarBandWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ReleaseToolbarImageList();
    bHandled = FALSE;
    return 0;
}

LRESULT CTabbarBandWindow::OnUpdateUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CTabbarBandWindow::OnNavigate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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

void CTabbarBandWindow::SendShellTabCommand(int command)
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
LRESULT CTabbarBandWindow::OnToolbarCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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

LRESULT CTabbarBandWindow::OnEmail(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CTabbarBandWindow::OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    //ShowSettings(m_hWnd);
    ::MessageBox(m_hWnd, _T("fdgl;gjlksdf ;drgkj;lsdf g"), _T("TabExplorer"), MB_OK);
    return TRUE;
}

LRESULT CTabbarBandWindow::OnRClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMMOUSE* pInfo = (NMMOUSE*)pnmh;
    POINT pt = pInfo->pt;
    {
        RECT rc;
        int count = (int)m_Toolbar.SendMessage(TB_BUTTONCOUNT);
        m_Toolbar.SendMessage(TB_GETITEMRECT, count - 1, (LPARAM)& rc);
        if (pt.x > rc.right)
            return 0;
    }
    m_Toolbar.ClientToScreen(&pt);
    //ShowSettingsMenu(m_hWnd, pt.x, pt.y);
    return 1;
}

LRESULT CTabbarBandWindow::OnGetInfoTip(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
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
LRESULT CALLBACK CTabbarBandWindow::RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == RB_SETBANDINFO || uMsg == RB_INSERTBAND)
    {
        REBARBANDINFO* pInfo = (REBARBANDINFO*)lParam;
        if ((pInfo->hwndChild == (HWND)dwRefData) && (pInfo->fMask & RBBIM_STYLE))
        {
            if (((CTabbarBandWindow*)uIdSubclass)->m_bBandNewLine)
                pInfo->fStyle |= RBBS_BREAK;
            else
                pInfo->fStyle &= ~RBBS_BREAK;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
