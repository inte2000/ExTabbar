// TabExplorerBHO.cpp : Implementation of CTabExplorerBHO

#include "pch.h"
#include "framework.h"
#include "TabExplorerBHO.h"
#include "dllmain.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include <Ntquery.h>
#include "SystemFunctions.h"
#include "DebugLog.h"

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

__declspec(thread) HHOOK CExplorerWindow::s_Hook; // one hook per thread

struct FindChild
{
	const wchar_t *className;
	HWND hWnd;
};

static BOOL CALLBACK EnumChildProc( HWND hwnd, LPARAM lParam )
{
	FindChild &find=*(FindChild*)lParam;
	wchar_t name[256];
	GetClassName(hwnd,name,_countof(name));
	if (_wcsicmp(name,find.className)!=0) return TRUE;
	find.hWnd=hwnd;
	return FALSE;
}

static HWND FindChildWindow( HWND hwnd, const wchar_t *className )
{
	FindChild find={className};
	EnumChildWindows(hwnd,EnumChildProc,(LPARAM)&find);
	return find.hWnd;
}

const UINT_PTR TIMER_NAVIGATE='CLSH';

static LRESULT CALLBACK SubclassParentProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
	// when the tree selection changes start a timer to navigate to the new folder in 100ms
	if (uMsg==WM_NOTIFY && ((NMHDR*)lParam)->code==TVN_SELCHANGED && ((NMTREEVIEW*)lParam)->action==TVC_BYKEYBOARD)
		SetTimer(((NMHDR*)lParam)->hwndFrom,TIMER_NAVIGATE,100,NULL);
	return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

static LRESULT CALLBACK SubclassTreeProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
	if (uMsg==TVM_ENSUREVISIBLE && (dwRefData&1))
	{
		// HACK! there is a bug in Win7 Explorer and when the selected folder is expanded for the first time it sends TVM_ENSUREVISIBLE for
		// the root tree item. This causes the navigation pane to scroll up. To work around the bug we ignore TVM_ENSUREVISIBLE if it tries
		// to show the root item and it is not selected
		HTREEITEM hItem=(HTREEITEM)lParam;
		if (!TreeView_GetParent(hWnd,hItem) && !(TreeView_GetItemState(hWnd,hItem,TVIS_SELECTED)&TVIS_SELECTED))
			return 0;
	}
    
	if (uMsg==WM_TIMER && wParam==TIMER_NAVIGATE)
	{
		// time to navigate to the new folder (simulate pressing Space)
		PostMessage(hWnd,WM_KEYDOWN,VK_SPACE,0);
		KillTimer(hWnd,TIMER_NAVIGATE);
		return 0;
	}

    if (uMsg == WM_PAINT)
    {
        //RemoveTreeItem(hWnd);
    }

	if (uMsg==WM_CHAR && wParam==' ')
		return 0; // ignore the Space character (to stop the tree view from beeping)

	if (uMsg==TVM_SETEXTENDEDSTYLE && wParam==(TVS_EX_FADEINOUTEXPANDOS|TVS_EX_AUTOHSCROLL|0x80000000) && lParam==0)
	{
		wParam&=0x7FFFFFFF;

		if (g_bFolderAutoNavigate)
			SetWindowSubclass(GetParent(hWnd),SubclassParentProc,'CLSH',0);

		if (g_bNoFadeInOutExpand)
			wParam&=~TVS_EX_FADEINOUTEXPANDOS;

		int indent = g_nTreeViewIndent;

		if (g_bTreeViewAutoScroll)
		{
            LogInfo(_T("ExplorerWindow hack treeview TVM_SETEXTENDEDSTYLE message"));
            SetWindowTheme(hWnd,NULL,NULL);
			DWORD style=GetWindowLong(hWnd,GWL_STYLE);
			style&=~TVS_NOHSCROLL;
			if (g_bTreeViewSimpleMode)
			{
				style|=TVS_SINGLEEXPAND|TVS_TRACKSELECT;
				style&=~TVS_HASLINES;
			}
			else
			{
				style|=TVS_HASLINES;
				style&=~(TVS_SINGLEEXPAND|TVS_TRACKSELECT);
				wParam|=TVS_EX_FADEINOUTEXPANDOS;
				HIMAGELIST images=TreeView_GetImageList(hWnd,TVSIL_NORMAL);
				int cx, cy;
				ImageList_GetIconSize(images,&cx,&cy);
				indent=cx+3;
			}
			SetWindowLong(hWnd,GWL_STYLE,style);
		}
		else
		{
			wParam&=~TVS_EX_AUTOHSCROLL;
		}

		if (indent >=0 )
			TreeView_SetIndent(hWnd,indent);

		if (wParam==0) return 0;
	}
	return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK CExplorerWindow::SubclassStatusProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
	wchar_t buf[1024];
	if (uMsg==WM_PAINT && ((CExplorerWindow*)uIdSubclass)->m_bForceRefresh)
	{
		// sometimes Explorer doesn't fully initialize the status bar on Windows 7 and leaves it with 1 part
		// in such case force the view to refresh after the status bar is fully visible
		((CExplorerWindow*)uIdSubclass)->m_bForceRefresh=false;
		if (SendMessage(hWnd,SB_GETPARTS,0,0)<=1)
			PostMessage(GetParent(hWnd),WM_COMMAND,41504,0); // Refresh command
	}
	if (uMsg==SB_SETTEXT && LOWORD(wParam)==0)
	{
		// when the text of the first part is changing

		// recalculate the selection size on a timer. this way if the status text is changed frequently
		// the recalculation will not happen every time
		SetTimer(hWnd,uIdSubclass,10,NULL);

		if (dwRefData&SPACE_WIN7)
		{
            if (((CExplorerWindow*)uIdSubclass)->m_bResetStatus && SendMessage(hWnd,SB_GETPARTS,0,0)<=1)
			{
                LogInfo(_T("ExplorerWindow hack statusbar space for win 7"));
                // HACK! there is a bug in Win7 and when the Explorer window is created it doesn't correctly
				// initialize the status bar to have 3 parts. as soon as the user resizes the window the
				// 3 parts appear. so here we resize the parent of the status bar to create the 3 parts.
				HWND parent=GetParent(hWnd);
				RECT rc;
				GetWindowRect(parent,&rc);
				SetWindowPos(parent,NULL,0,0,rc.right-rc.left+1,rc.bottom-rc.top,SWP_NOZORDER|SWP_NOMOVE);
				SetWindowPos(parent,NULL,0,0,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER|SWP_NOMOVE);
				// the first time the status text is set it is too early. so we do this until we get at lest 2 parts
				if (SendMessage(hWnd,SB_GETPARTS,0,0)>1)
					((CExplorerWindow*)uIdSubclass)->m_bResetStatus=false;
			}

            LogInfo(_T("ExplorerWindow hack statusbar calculate free space"));
			// find the current folder and show the free space of the drive containing the current folder
			// also works for network locations
			IShellBrowser *pBrowser=((CExplorerWindow*)uIdSubclass)->m_spShellBrowser;
			CComPtr<IShellView> pView;
			if (pBrowser && SUCCEEDED(pBrowser->QueryActiveShellView(&pView)))
			{
				CComQIPtr<IFolderView> pView2=pView;
				CComPtr<IPersistFolder2> pFolder;
				if (pView2 && SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2,(void**)&pFolder)))
				{
					LPITEMIDLIST pidl;
					if (SUCCEEDED(pFolder->GetCurFolder(&pidl)))
					{
						if (SHGetPathFromIDList(pidl,buf))
						{
							ULARGE_INTEGER size;
							if (GetDiskFreeSpaceEx(buf,NULL,NULL,&size))
							{
								const wchar_t *text=(wchar_t*)lParam;
								wchar_t str[100];
								StrFormatByteSize64(size.QuadPart,str,_countof(str));
								swprintf_s(buf,_T("%s (Disk free space: %s)"),text,str);
								lParam=(LPARAM)buf;
							}
						}
						ILFree(pidl);
					}
				}
			}
		}
	}
	if (uMsg==SB_SETTEXT && LOWORD(wParam)==1)
	{
		return 0;
	}

	if (uMsg==WM_TIMER && wParam==uIdSubclass)
	{
		// recalculate the total size of the selected files and show it in part 2 of the status bar
		KillTimer(hWnd,wParam);
		IShellBrowser *pBrowser=((CExplorerWindow*)uIdSubclass)->m_spShellBrowser;
		__int64 size=-1;
		CComPtr<IShellView> pView;
		if (pBrowser && SUCCEEDED(pBrowser->QueryActiveShellView(&pView)))
		{
			CComQIPtr<IFolderView> pView2=pView;
			CComPtr<IPersistFolder2> pFolder;
			LPITEMIDLIST pidl;
			if (pView2 && SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2,(void**)&pFolder)) && SUCCEEDED(pFolder->GetCurFolder(&pidl)))
			{
				CComQIPtr<IShellFolder2> pFolder2=pFolder;
				UINT type=SVGIO_SELECTION;
				int count;
				if ((dwRefData&SPACE_TOTAL) && (FAILED(pView2->ItemCount(SVGIO_SELECTION,&count)) || count==0))
					type=SVGIO_ALLVIEW;
				CComPtr<IEnumIDList> pEnum;
				if (SUCCEEDED(pView2->Items(type,IID_IEnumIDList,(void**)&pEnum)) && pEnum)
				{
					PITEMID_CHILD child;
					SHCOLUMNID column={PSGUID_STORAGE,PID_STG_SIZE};
					while (pEnum->Next(1,&child,NULL)==S_OK)
					{
						CComVariant var;
						if (SUCCEEDED(pFolder2->GetDetailsEx(child,&column,&var)) && var.vt==VT_UI8)
						{
							if (size<0)
								size=var.ullVal;
							else
								size+=var.ullVal;
						}
						ILFree(child);
					}
				}
				ILFree(pidl);
			}
		}
		if (size>=0)
		{
			// format the file size as KB, MB, etc
			StrFormatByteSize64(size,buf,_countof(buf));
		}
		else
			buf[0]=0;
		DefSubclassProc(hWnd,SB_SETTEXT,1,(LPARAM)buf);
	}
	return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK CExplorerWindow::HookExplorer( int nCode, WPARAM wParam, LPARAM lParam )
{
	if (nCode==HCBT_CREATEWND)
	{
		HWND hWnd=(HWND)wParam;
		CBT_CREATEWND *create=(CBT_CREATEWND*)lParam;
		if (create->lpcs->lpszClass>(LPTSTR)0xFFFF && _wcsicmp(create->lpcs->lpszClass,WC_TREEVIEW)==0)
		{
            LogInfo(_T("ExplorerWindow treeview windows create hooked!"));
            DWORD_PTR settings=0;
            
            if (IsWindows7() && g_bFixFolderScroll)
				settings |= 1;

			SetWindowSubclass(hWnd,SubclassTreeProc,'CLSH',settings);
			PostMessage(hWnd,TVM_SETEXTENDEDSTYLE,TVS_EX_FADEINOUTEXPANDOS|TVS_EX_AUTOHSCROLL|0x80000000,0);
			UnhookWindowsHookEx(s_Hook);
			s_Hook=NULL;
			return 0;
		}
	}
	return CallNextHookEx(NULL,nCode,wParam,lParam);
}

LRESULT CALLBACK CExplorerWindow::RebarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
	if (uMsg==WM_NOTIFY && ((NMHDR*)lParam)->hwndFrom==(HWND)dwRefData && ((NMHDR*)lParam)->code==NM_CUSTOMDRAW)
	{
		// custom-draw the toolbar. just draw the correct icon and nothing else
		NMTBCUSTOMDRAW *pDraw=(NMTBCUSTOMDRAW*)lParam;
		if (pDraw->nmcd.dwDrawStage==CDDS_PREPAINT)
			return CDRF_NOTIFYITEMDRAW;
		if (pDraw->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
		{
            CExplorerWindow*pThis=(CExplorerWindow*)uIdSubclass;
            /*
			BOOL comp;
			if (SUCCEEDED(DwmIsCompositionEnabled(&comp)) && comp)
				FillRect(pDraw->nmcd.hdc,&pDraw->nmcd.rc,(HBRUSH)GetStockObject(WHITE_BRUSH));
            */
            DrawThemeParentBackground(pDraw->nmcd.hdr.hwndFrom, pDraw->nmcd.hdc, &pDraw->nmcd.rc);
			if (pDraw->nmcd.uItemState&CDIS_DISABLED)
			{
				if (pThis->m_IconDisabled)
					DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconDisabled,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
				else
					DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconNormal,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
			}
			else if (pDraw->nmcd.uItemState&CDIS_SELECTED)
			{
				if (pThis->m_IconPressed)
					DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconPressed,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
				else
					DrawIconEx(pDraw->nmcd.hdc,1,1,pThis->m_IconNormal,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
			}
			else if (pDraw->nmcd.uItemState&CDIS_HOT)
			{
				if (pThis->m_IconHot)
					DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconHot,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
				else
					DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconNormal,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
			}
			else
				DrawIconEx(pDraw->nmcd.hdc,0,0,pThis->m_IconNormal,0,0,0,NULL,DI_NORMAL|DI_NOMIRROR);
			return CDRF_SKIPDEFAULT;
		}
	}

	if (uMsg==WM_THEMECHANGED)
	{
        LogInfo(_T("ExplorerWindow rebar subclass got WM_THEMECHANGED message!"));
        // the button size is reset when the theme changes. force the correct size again
		HWND toolbar=(HWND)dwRefData;
		RECT rc;
		GetClientRect(toolbar,&rc);
		PostMessage(toolbar,TB_SETBUTTONSIZE,0,MAKELONG(rc.right,rc.bottom));
	}

	if (uMsg==WM_NOTIFY && ((NMHDR*)lParam)->hwndFrom==(HWND)dwRefData && ((NMHDR*)lParam)->code==TBN_GETINFOTIP)
	{
		// show the tip for the up button
		NMTBGETINFOTIP *pTip=(NMTBGETINFOTIP*)lParam;
		wcscpy_s(pTip->pszText,pTip->cchTextMax,L"Up One Level");
		return 0;
	}

	if (uMsg==WM_NOTIFY && ((NMHDR*)lParam)->hwndFrom==(HWND)dwRefData && ((NMHDR*)lParam)->code==NM_RCLICK)
	{
		NMMOUSE *pInfo=(NMMOUSE*)lParam;
		POINT pt=pInfo->pt;
		ClientToScreen(pInfo->hdr.hwndFrom,&pt);
		//ShowSettingsMenu(hWnd,pt.x,pt.y);
		return TRUE;
	}

	if (uMsg==WM_COMMAND && wParam==1)
	{
		UINT flags=(GetKeyState(VK_CONTROL)<0?SBSP_NEWBROWSER:SBSP_SAMEBROWSER);
		((CExplorerWindow*)uIdSubclass)->m_spShellBrowser->BrowseObject(NULL,flags|SBSP_PARENT);
	}
	return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

BOOL CExplorerWindow::OnExplorerAttach(CComPtr<IWebBrowser2>& spWebBrowser2, CComPtr<IShellBrowser>& spShellBrowser)
{
    BOOL bRtn = FALSE;

    m_spWebBrowser2 = spWebBrowser2;
    m_spShellBrowser = spShellBrowser;

    if (!HookExplorer())
        return FALSE;

    bool bWin7 = IsWindows7() ? true : false;
    if (bWin7 && g_bAddUpButton)
    {
        if(!AddUpButtonToExplorBar())
        {
            UnhookExplorer();
            return FALSE;
        }
        m_bUpToolbarAdd = true;
    }

    if (g_bShowFreespaceInStatusBar)
    {
        SubclassStatusBar();
    }

    return TRUE;
}

void CExplorerWindow::OnExplorerDetach()
{
    UnhookExplorer();
    RemoveUpButtonFromExplorBar();

    m_spShellBrowser = NULL;
    m_spWebBrowser2 = NULL;
    m_hExplorerWnd = NULL;
}

void CExplorerWindow::OnNavigateComplete()
{
    // this is called when the current folder changes. disable the Up button if this is the desktop folder
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

void CExplorerWindow::SetUpButtonStatus(bool bEnable)
{
    m_Toolbar.SendMessage(TB_ENABLEBUTTON, 1, bEnable ? 1 : 0);
}
/*
ReBar
   TravelBand
       Toolbar  "导航按钮"
       toolbar   we want add here a new toolbar, it has one button: upbutton
   UpBand (win10 only)
       Toolbar  "向上一级工具栏"
*/
BOOL CExplorerWindow::AddUpButtonToExplorBar()
{
    m_hExplorerWnd = NULL;

    LogTrace(_T("ExplorerWindow AddUpButtonToExplorBar() invoked!"));
    if (m_spWebBrowser2 && (m_spWebBrowser2->get_HWND((LONG_PTR*)&m_hExplorerWnd) == S_OK))
    {
        // for win7, we find the TravelBand, the rebar and the toolbar. win10 has UpBand, do the same things
        HWND hTravelBand = hTravelBand = FindChildWindow(m_hExplorerWnd, L"TravelBand");;
        LogInfo(_T("ExplorerWindow find hTravelBand = 0x%08x!"), hTravelBand);
        if (hTravelBand)
        {
            HWND toolbar = FindWindowEx(hTravelBand, NULL, TOOLBARCLASSNAME, NULL);
            RECT rc;
            GetClientRect(toolbar, &rc);
            HWND rebar = GetParent(hTravelBand);
            int size = g_nUpButtonIconSize;
            m_Toolbar = CreateWindow(TOOLBARCLASSNAME, L"UpButton", WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_CUSTOMERASE | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE, 0, 0, 10, 10, rebar, NULL, g_Instance, NULL);
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

            SetWindowSubclass(rebar, RebarSubclassProc, (UINT_PTR)this, (DWORD_PTR)m_Toolbar.m_hWnd);
            REBARBANDINFO info = { sizeof(info),RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE | RBBIM_STYLE };
            SendMessage(rebar, RB_GETBANDINFO, 1, (LPARAM)& info);
            info.fStyle = RBBS_HIDETITLE | RBBS_NOGRIPPER | RBBS_FIXEDSIZE;
            info.hwndChild = m_Toolbar.m_hWnd;
            info.cxIdeal = info.cx = info.cxMinChild = size;
            info.cyMinChild = size;
            SendMessage(rebar, RB_INSERTBAND, 1, (LPARAM)& info);
            RedrawWindow(rebar, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);

            LogInfo(_T("ExplorerWindow create up button toolbar success!"));
            return TRUE;
        }
    }

    return FALSE;
}

void CExplorerWindow::RemoveUpButtonFromExplorBar()
{
    LogTrace(_T("ExplorerWindow RemoveUpButtonFromExplorBar() invoked!"));
    if (m_hExplorerWnd != NULL)
    {
        HWND hTravelBand = hTravelBand = FindChildWindow(m_hExplorerWnd, L"TravelBand");;
        if (hTravelBand)
        {
            HWND rebar = GetParent(hTravelBand);

            SendMessage(rebar, RB_DELETEBAND, 1, (LPARAM)NULL);
            RedrawWindow(rebar, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);

            m_Toolbar.DestroyWindow();
        }
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

BOOL CExplorerWindow::SubclassStatusBar()
{
    HWND hStatusbar;
    LogTrace(_T("ExplorerWindow SubclassStatusBar() invoked!"));
    if (m_spShellBrowser && SUCCEEDED(m_spShellBrowser->GetControlWindow(FCW_STATUS, &hStatusbar)))
    {
        bool bWin7 = IsWindows7() ? true : false;
        DWORD FreeSpace = SPACE_SHOW | SPACE_TOTAL; // always show total
        if (bWin7)
            FreeSpace |= SPACE_WIN7;
        
        LogInfo(_T("ExplorerWindow SubclassStatusBar() subclass statusbar with FreeSpace=%x!"), FreeSpace);
        SetWindowSubclass(hStatusbar, SubclassStatusProc, (UINT_PTR)this, FreeSpace);
        m_bForceRefresh = (bWin7 && g_bForceRefreshWin7);

        return TRUE;
    }

    return FALSE;
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

