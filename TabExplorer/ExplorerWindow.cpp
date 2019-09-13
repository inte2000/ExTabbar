// ExplorerWindow.cpp : Implementation of CExplorerWindow

#include "pch.h"
#include "framework.h"
#include "TabExplorerBHO.h"
#include "dllmain.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "PrivateMessage.h"
#include "ShellWrapper.h"
#include "ExplorerWindow.h"


//global options
BOOL g_bHideRapidAccess = TRUE; //隐藏快速访问节点
BOOL g_bAddUpButton = TRUE; //给浏览栏增加一个UP按钮（win7和win8需要，win10已经恢复了UP按钮）
BOOL g_bFixFolderScroll = FALSE; //only for win 7
BOOL g_bFolderAutoNavigate = TRUE;
/*vista只有的版本中，explorer的树控件增加了一种自动隐藏节点+/-标记的特性，当explorer
失去焦点时，这些三角形符号会慢慢消失 */
BOOL g_bNoFadeInOutExpand = FALSE; 
BOOL g_bTreeViewAutoScroll = FALSE;
BOOL g_bTreeViewSimpleMode = FALSE;
int g_nTreeViewIndent = 0;
int g_nUpButtonIconSize = 24;
BOOL g_bForceRefreshWin7 = TRUE;
BOOL g_bShowFreespaceInStatusBar = TRUE;
BOOL g_bNoWindowResizing = FALSE;

extern BOOL g_bUsingLargeButton;
extern int g_nSmallIconSize;
extern int g_nLargeIconSize;

CExplorerWindow * CExplorerWindow::m_pThisExplorer = nullptr;
__declspec(thread) HHOOK CExplorerWindow::s_Hook; // one hook per thread

LRESULT CALLBACK CExplorerWindow::HookExplorer( int nCode, WPARAM wParam, LPARAM lParam )
{
	if (nCode==HCBT_CREATEWND)
	{
		HWND hWnd = (HWND)wParam;
		CBT_CREATEWND *create=(CBT_CREATEWND*)lParam;
		if (create->lpcs->lpszClass>(LPTSTR)0xFFFF && _wcsicmp(create->lpcs->lpszClass,WC_TREEVIEW)==0)
		{
            LogInfo(_T("ExplorerWindow treeview windows create hooked!"));
            ATLASSERT(m_pThisExplorer != nullptr);
            m_pThisExplorer->SubclassLeftTree(hWnd);
			UnhookWindowsHookEx(s_Hook);
			s_Hook=NULL;
			return 0;
		}
	}
	return CallNextHookEx(NULL,nCode,wParam,lParam);
}

LRESULT CALLBACK CExplorerWindow::ExplorerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CExplorerWindow* pThisExplorer = (CExplorerWindow*)dwRefData;
    ATLASSERT(pThisExplorer != nullptr);

    if (uMsg == WM_BROWSEOBJECT)
    {
        UINT wFlags = (UINT)wParam;
        LPCITEMIDLIST pidl = (LPCITEMIDLIST)lParam;
        if (pThisExplorer->OnBrowseObject(pidl, wFlags))
            return 1;
        else
            return 0;
    }
#if 0
    if (uMsg == WM_SIZE)
    {
        LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        DWORD_PTR dwSizeType = wParam;      // resizing flag 
        WORD nWidth = LOWORD(lParam);  // width of client area 
        WORD nHeight = HIWORD(lParam); // height of client area 
        //pThisExplorer->OnSize(dwSizeType, nWidth, nHeight);
        return lr;
    }
#endif
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

CExplorerWindow::CExplorerWindow()
{
    m_hExplorerWnd = NULL;
    m_ExplorerSubclassId = 0x20190803;
    m_pThisExplorer = this;
    m_iTabbarHeight = g_bUsingLargeButton ? g_nLargeIconSize + 8 : g_nSmallIconSize + 8;
}


//class name: UIRibbonCommandBarDock, caption: UIRibbonDockTop
//class name: ShellTabWindowClass, caption: 此电脑


BOOL CExplorerWindow::OnExplorerAttach(CComPtr<IWebBrowser2>& spWebBrowser2, CComPtr<IShellBrowser>& spShellBrowser)
{
    BOOL bRtn = FALSE;

    LogTrace(_T("ExplorerWindow OnExplorerAttach() invoked!"));

    m_spWebBrowser2 = spWebBrowser2;
    m_spShellBrowser = spShellBrowser;
    m_hExplorerWnd = NULL;

    if (!m_spWebBrowser2 || (m_spWebBrowser2->get_HWND((LONG_PTR*)& m_hExplorerWnd) != S_OK))
    {
        return FALSE;
    }

    if (!HookExplorer())
        return FALSE;

    HWND hWorkerWnd = FindChildWndEx(m_hExplorerWnd, _T("WorkerW"), nullptr);
    HWND hShellTabWnd = FindChildWndEx(m_hExplorerWnd, _T("ShellTabWindowClass"), nullptr);
    if ((hShellTabWnd == NULL) || (hWorkerWnd == NULL))
    {
        LogError(_T("ExplorerWindow find lower window faild!"));
        return FALSE;
    }
    if (!m_AddressBar.Initialize(m_spShellBrowser, hWorkerWnd, this))
    {
        LogError(_T("AddressBar subclass shell tab windows faild!"));
        return FALSE;
    }
    if (!m_TabbarWnd.Initialize(m_spShellBrowser, m_hExplorerWnd))
    {
        LogError(_T("ExplorerWindow create tabbar window faild!"));
        return FALSE;
    }
    if (!m_ShellTabWnd.Initialize(m_spShellBrowser, hShellTabWnd, this))
    {
        LogError(_T("ExplorerWindow subclass shell tab windows faild!"));
        return FALSE;
    }

    if (!::SetWindowSubclass(m_hExplorerWnd, ExplorerSubclassProc, m_ExplorerSubclassId, (DWORD_PTR)this))
    {
        LogError(_T("ExplorerWindow subclass explorer main windows faild!"));
        return FALSE;
    }

    if (!m_TravelBand.Initialize(m_spShellBrowser, m_hExplorerWnd))
    {
        LogError(_T("ExplorerWindow TravelBand::Initialize() faild!"));
    }

    if (g_bShowFreespaceInStatusBar)
    {
        SubclassStatusBar();
    }

    if (g_bNoWindowResizing)
    {
        LONG_PTR dwStyle = ::GetWindowLongPtr(m_hExplorerWnd, GWL_STYLE);
        dwStyle &= ~WS_SIZEBOX;
        ::SetWindowLongPtr(m_hExplorerWnd, GWL_STYLE, dwStyle);
    }

    m_AddressBar.SetManaging(true);
    m_ShellTabWnd.SetManaging(true);

    return TRUE;
}

void CExplorerWindow::OnExplorerDetach()
{
    LogTrace(_T("ExplorerWindow OnExplorerAttach() invoked!"));

    UnhookExplorer();
    m_TravelBand.Unintialize();
    m_LeftTree.Unintialize();
    m_Statusbar.Unintialize();
    m_TabbarWnd.Unintialize();
    m_AddressBar.Unintialize();
    m_ShellTabWnd.Unintialize();
    ::RemoveWindowSubclass(m_hExplorerWnd, ExplorerSubclassProc, m_ExplorerSubclassId);

    m_spShellBrowser = NULL;
    m_spWebBrowser2 = NULL;
    m_hExplorerWnd = NULL;
}

HRESULT CExplorerWindow::OnBeforeNavigate(const TString& strUrl)
{
    if (!m_LeftTree.IsTreeModified())
        m_LeftTree.ModifyTreeItem();

    m_TabbarWnd.OnBeforeNavigate(strUrl);
    return S_OK;
}

void CExplorerWindow::OnNavigateComplete(const TString& strUrl)
{
    // this is called when the current folder changes. disable the Up button if this is the desktop folder
    if (!m_LeftTree.IsTreeModified())
        m_LeftTree.ModifyTreeItem();

    m_TravelBand.UpdateToolbarStatus();
    m_TabbarWnd.OnNavigateComplete(strUrl);
}

void CExplorerWindow::UpdateTabSizeAndPosition(RECT& StwRect)
{
    if ((StwRect.bottom - StwRect.top) <= 0)
        return;

    CWindow ewnd(m_hExplorerWnd);

    StwRect.top += m_iTabbarHeight;

    RECT rcWorker;
    m_AddressBar.GetWindowRect(&rcWorker);
    ewnd.ScreenToClient(&rcWorker);

    int workerHeight = rcWorker.bottom - rcWorker.top;
    rcWorker.bottom = StwRect.top;
    rcWorker.top = rcWorker.bottom - workerHeight; 

    RECT rcLocalTab = StwRect;  //tabbar上移
    rcLocalTab.bottom = rcWorker.top;
    rcLocalTab.top = rcLocalTab.bottom - m_iTabbarHeight;

    //m_AddressBar.MovePosition(&rcWorker);
    m_TabbarWnd.MovePosition(rcLocalTab);
}

BOOL CExplorerWindow::SubclassStatusBar()
{
    HWND hStatusbar = NULL;
    LogTrace(_T("ExplorerWindow Subclass StatusBar..."));
    if (m_spShellBrowser && SUCCEEDED(m_spShellBrowser->GetControlWindow(FCW_STATUS, &hStatusbar)))
    {
        return m_Statusbar.Initialize(m_spShellBrowser, hStatusbar);
    }

    return FALSE;
}

BOOL CExplorerWindow::SubclassLeftTree(HWND hTreeView)
{
    LogTrace(_T("ExplorerWindow Subclass LeftTreeView..."));
    return m_LeftTree.Initialize(m_spShellBrowser, hTreeView);
}

BOOL CExplorerWindow::HookExplorer()
{
    // hook
    if (!s_Hook)
    {
        LogTrace(_T("ExplorerWindow HookExplorer() set explorer WH_CBT hook!"));
        s_Hook = SetWindowsHookEx(WH_CBT, HookExplorer, NULL, GetCurrentThreadId());
    }

    return (s_Hook != NULL);
}

void CExplorerWindow::UnhookExplorer()
{
    // unhook
    if (s_Hook)
    {
        LogTrace(_T("ExplorerWindow UnhookExplorer() remove explorer WH_CBT hook!"));
        UnhookWindowsHookEx(s_Hook);
        s_Hook = NULL;
    }
}

/*
void CExplorerWindow::OnSize(DWORD_PTR dwSizeType, DWORD nWidth, DWORD nHeight)
{
}
*/

BOOL CExplorerWindow::OnBrowseObject(LPCITEMIDLIST pidl, UINT wFlags)
{
    if ((wFlags & SBSP_NAVIGATEBACK) != 0)
    {
        return m_TabbarWnd.NavigateCurrentTab(true);
    }
    else if ((wFlags & SBSP_NAVIGATEFORWARD) != 0)
    {
        return m_TabbarWnd.NavigateCurrentTab(false);
    }
    else
    {
        CIDLEx cidl;
        bool autonav = (wFlags & 0x100) != 0; //SBSP_AUTONAVIGATE
        if (pidl != NULL)
        {
            cidl.Attach((LPITEMIDLIST)pidl, false);
        }

        return m_TabbarWnd.BeforeNavigate(cidl, autonav);
    }
}
