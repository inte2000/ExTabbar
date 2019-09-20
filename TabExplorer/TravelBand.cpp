// TravelBand.cpp : Implementation of CTravelBand

#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "TravelBand.h"


extern BOOL g_bAddUpButton;
extern int g_nUpButtonIconSize;


LRESULT CALLBACK CTravelBand::RebarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
    CTravelBand* pThisBand = (CTravelBand*)dwRefData;
    ATLASSERT(pThisBand != NULL);

	if ((uMsg == WM_NOTIFY) && (((NMHDR*)lParam)->code == NM_CUSTOMDRAW))
	{
		// custom-draw the toolbar. just draw the correct icon and nothing else
        NMTBCUSTOMDRAW* pDraw = (NMTBCUSTOMDRAW*)lParam;
        BOOL handled = FALSE;
        LRESULT hr = pThisBand->OnToolbarNotifyCustomDraw(pDraw, handled);
        if (handled)
            return hr;
	}

	if (uMsg == WM_THEMECHANGED)
	{
        LogInfo(_T("CTravelBand rebar subclass got WM_THEMECHANGED message!"));
        // the button size is reset when the theme changes. force the correct size again
        pThisBand->OnThemeChanged();
	}

	if ((uMsg == WM_NOTIFY) && (((NMHDR*)lParam)->code == TBN_GETINFOTIP))
	{
		// show the tip for the up button
        BOOL handled = FALSE;
		NMTBGETINFOTIP *pTip=(NMTBGETINFOTIP*)lParam;
        LRESULT hr = pThisBand->OnToolbarNotifyGetToolTip(pTip, handled);
        if(handled)
            return hr;
	}

	if ((uMsg == WM_NOTIFY) && (((NMHDR*)lParam)->code == NM_RCLICK))
	{
		NMMOUSE *pInfo = (NMMOUSE*)lParam;
        BOOL handled = FALSE;
        LRESULT hr = pThisBand->OnToolbarNotifyRClick(pInfo, handled);
        if (handled)
            return hr;
	}

	if ((uMsg == WM_COMMAND) && (wParam == 1))
	{
        pThisBand->OnToolbarCommand();
	}

	return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

BOOL CTravelBand::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hExplorerWnd)
{
    m_spShellBrowser = spShellBrowser;
    
    HWND hTravelBand = FindChildWndEx(hExplorerWnd, L"TravelBand");;
    LogInfo(_T("CTravelBand find hTravelBand = 0x%08x!"), hTravelBand);
    if (hTravelBand == NULL)
    {
        LogError(_T("CTravelBand can not find hTravelBand!"));
        return FALSE;
    }

    HWND toolbar = ::FindWindowEx(hTravelBand, NULL, TOOLBARCLASSNAME, NULL);
    m_hParentRebarWnd = ::GetParent(hTravelBand);
    if (m_hParentRebarWnd == NULL)
    {
        LogError(_T("CTravelBand can not find hTravelRebar window!"));
        return FALSE;
    }

    bool bWin7 = IsWindows7() ? true : false;
    if (bWin7 && g_bAddUpButton)
    {
        m_bUpToolbarAdd = AddUpButtonToTravelBand(m_hParentRebarWnd);
    }
    
    return m_bUpToolbarAdd;
}

void CTravelBand::Unintialize()
{
    if(m_bUpToolbarAdd)
    {
        RemoveUpButtonFromTravelBand();
        m_bUpToolbarAdd = FALSE;
    }
}

void CTravelBand::UpdateToolbarStatus()
{
    bool bEnableUp = true;

    if (!m_spShellBrowser || !m_bUpToolbarAdd)
        return;

    CComPtr<IShellView> pView;
    m_spShellBrowser->QueryActiveShellView(&pView);
    if (pView)
    {
        CComQIPtr<IFolderView> pView2 = pView;
        if (pView2)
        {
            CComPtr<IPersistFolder2> pFolder;
            pView2->GetFolder(IID_IPersistFolder2, (void**)&pFolder);
            if (pFolder)
            {
                LPITEMIDLIST pidl;
                pFolder->GetCurFolder(&pidl);
                if (ILIsEmpty(pidl))
                    bEnableUp = false; // only the top level has empty PIDL
                CoTaskMemFree(pidl);
            }
        }
    }

    SetUpButtonStatus(bEnableUp);
}

void CTravelBand::SetTravelButtonStatus(bool canBack, bool canForward)
{
    HWND hTravelBand = FindChildWndEx(m_hParentRebarWnd, L"TravelBand");;
    if (hTravelBand != NULL)
    {
        HWND toolbar = FindWindowEx(hTravelBand, NULL, TOOLBARCLASSNAME, NULL);
        if (toolbar != NULL)
        {
            ::SendMessage(toolbar, TB_ENABLEBUTTON, 0x100, MAKELPARAM((canBack ? 1 : 0), 0));
            ::SendMessage(toolbar, TB_ENABLEBUTTON, 0x101, MAKELPARAM((canForward ? 1 : 0), 0));
            ::SendMessage(toolbar, TB_ENABLEBUTTON, 0x102, MAKELPARAM(((canBack || canForward) ? 1 : 0), 0));
        }
    }
}

/*
ReBar
   TravelBand
       Toolbar  "导航按钮"
       toolbar   we want add here a new toolbar, it has one button: upbutton
   UpBand (win10 only)
       Toolbar  "向上一级工具栏"
*/
BOOL CTravelBand::AddUpButtonToTravelBand(HWND hParentRebarWnd)
{
    // for win7, we find the TravelBand, the rebar and the toolbar. win10 has UpBand, do the same things
    int size = g_nUpButtonIconSize;
    DWORD dwStyle = WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_CUSTOMERASE | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE;
    m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"UpButton", dwStyle, 0, 0, 10, 10, m_hParentRebarWnd, NULL, g_Instance, NULL);
    m_Toolbar.SendMessage(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);
    m_Toolbar.SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON));
    m_Toolbar.SendMessage(TB_SETMAXTEXTROWS, 1);

    HMODULE hShell32 = GetModuleHandle(L"Shell32.dll");
    std::vector<HMODULE> modules;
    LPCTSTR str = NULL;// FindSetting("UpIconNormal");  "shell32.dll,46"
    if (str != NULL)
    {
        m_IconNormal = str ? WzLoadIcon(size, str, 0, modules, hShell32) : NULL;
        str = NULL;// FindSetting("UpIconHot");
        m_IconHot = str ? WzLoadIcon(size, str, 0, modules, NULL) : NULL;
        str = NULL;// FindSetting("UpIconPressed");
        m_IconPressed = str ? WzLoadIcon(size, str, 0, modules, NULL) : NULL;
        str = NULL;// FindSetting("UpIconDisabled");
        m_IconDisabled = str ? WzLoadIcon(size, str, 0, modules, NULL) : NULL;
        if (!m_IconDisabled)
            m_IconDisabled = CreateDisabledIcon(m_IconNormal, size);
    }
    else
    {
        m_IconNormal = (HICON)LoadImage(g_Instance, MAKEINTRESOURCE(IDI_UP2NORMAL), IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
        m_IconHot = (HICON)LoadImage(g_Instance, MAKEINTRESOURCE(IDI_UP2HOT), IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
        m_IconPressed = (HICON)LoadImage(g_Instance, MAKEINTRESOURCE(IDI_UP2PRESSED), IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
        m_IconDisabled = (HICON)LoadImage(g_Instance, MAKEINTRESOURCE(IDI_UP2DISABLED), IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
    }

    for (std::vector<HMODULE>::const_iterator it = modules.begin(); it != modules.end(); ++it)
        FreeLibrary(*it);

    TBBUTTON button = { I_IMAGENONE,1,TBSTATE_ENABLED };
    m_Toolbar.SendMessage(TB_ADDBUTTONS, 1, (LPARAM)& button);
    m_Toolbar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(size, size));

    ::SetWindowSubclass(m_hParentRebarWnd, RebarSubclassProc, m_RebarSubclassId, (DWORD_PTR)this);
    REBARBANDINFO info = { sizeof(info),RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE | RBBIM_STYLE };
    SendMessage(m_hParentRebarWnd, RB_GETBANDINFO, 1, (LPARAM)& info);
    info.fStyle = RBBS_HIDETITLE | RBBS_NOGRIPPER | RBBS_FIXEDSIZE;
    info.hwndChild = m_Toolbar.m_hWnd;
    info.cxIdeal = info.cx = info.cxMinChild = size;
    info.cyMinChild = size;
    SendMessage(m_hParentRebarWnd, RB_INSERTBAND, 1, (LPARAM)& info);
    RedrawWindow(m_hParentRebarWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);

    LogInfo(_T("CTravelBand create up button toolbar success!"));
    return TRUE;
}

void CTravelBand::RemoveUpButtonFromTravelBand()
{
    LogTrace(_T("CTravelBand RemoveUpButtonFromTravelBand() invoked!"));
    if (m_hParentRebarWnd != NULL)
    {
        SendMessage(m_hParentRebarWnd, RB_DELETEBAND, 1, (LPARAM)NULL);
        RedrawWindow(m_hParentRebarWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
        ::RemoveWindowSubclass(m_hParentRebarWnd, RebarSubclassProc, m_RebarSubclassId);

        m_Toolbar.DestroyWindow();
    }
    m_Toolbar = NULL;
    if (m_IconNormal)
    {
        ::DestroyIcon(m_IconNormal);
        m_IconNormal = NULL;
    }
    if (m_IconHot)
    {
        ::DestroyIcon(m_IconHot);
        m_IconHot = NULL;
    }
    if (m_IconPressed)
    {
        ::DestroyIcon(m_IconPressed);
        m_IconPressed = NULL;
    }
    if (m_IconDisabled)
    {
        ::DestroyIcon(m_IconDisabled);
        m_IconDisabled = NULL;
    }
}

void CTravelBand::ItemCustonDrawRepaint(NMTBCUSTOMDRAW* pDraw)
{
    /*
    BOOL comp;
    if (SUCCEEDED(DwmIsCompositionEnabled(&comp)) && comp)
        FillRect(pDraw->nmcd.hdc,&pDraw->nmcd.rc,(HBRUSH)GetStockObject(WHITE_BRUSH));
    */
    DrawThemeParentBackground(pDraw->nmcd.hdr.hwndFrom, pDraw->nmcd.hdc, &pDraw->nmcd.rc);
    if (pDraw->nmcd.uItemState & CDIS_DISABLED)
    {
        if (m_IconDisabled)
            DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconDisabled, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
        else
            DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconNormal, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
    }
    else if (pDraw->nmcd.uItemState & CDIS_SELECTED)
    {
        if (m_IconPressed)
            DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconPressed, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
        else
            DrawIconEx(pDraw->nmcd.hdc, 1, 1, m_IconNormal, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
    }
    else if (pDraw->nmcd.uItemState & CDIS_HOT)
    {
        if (m_IconHot)
            DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconHot, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
        else
            DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconNormal, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
    }
    else
        DrawIconEx(pDraw->nmcd.hdc, 0, 0, m_IconNormal, 0, 0, 0, NULL, DI_NORMAL | DI_NOMIRROR);
}

void CTravelBand::OnThemeChanged()
{
    RECT rc;
    m_Toolbar.GetClientRect(&rc);
    m_Toolbar.PostMessage(TB_SETBUTTONSIZE, 0, MAKELONG(rc.right, rc.bottom));
}

void CTravelBand::OnToolbarCommand()
{
    UINT flags = (GetKeyState(VK_CONTROL) < 0 ? SBSP_NEWBROWSER : SBSP_SAMEBROWSER);
    m_spShellBrowser->BrowseObject(NULL, flags | SBSP_PARENT);
}

LRESULT CTravelBand::OnToolbarNotifyCustomDraw(NMTBCUSTOMDRAW* pDraw, BOOL& handled)
{
    handled = FALSE;

    if (pDraw->nmcd.hdr.hwndFrom == m_Toolbar.m_hWnd)
    {
        if (pDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
        {
            handled = TRUE;
            return CDRF_NOTIFYITEMDRAW;
        }
        if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
        {
            ItemCustonDrawRepaint(pDraw);
            handled = TRUE;
            return CDRF_SKIPDEFAULT;
        }
    }

    return 0;
}

LRESULT CTravelBand::OnToolbarNotifyGetToolTip(NMTBGETINFOTIP* pTip, BOOL& handled)
{
    handled = FALSE;

    if (pTip->hdr.hwndFrom == m_Toolbar.m_hWnd)
    {
        wcscpy_s(pTip->pszText, pTip->cchTextMax, L"Up One Level");
        handled = TRUE;
    }

    return 0;
}

LRESULT CTravelBand::OnToolbarNotifyRClick(NMMOUSE* pInfo, BOOL& handled)
{
    handled = FALSE;

    if (pInfo->hdr.hwndFrom == m_Toolbar.m_hWnd)
    {
        POINT pt = pInfo->pt;
        ClientToScreen(pInfo->hdr.hwndFrom, &pt);
        //ShowSettingsMenu(hWnd,pt.x,pt.y);
        handled = TRUE;
    }

    return 1;
}
