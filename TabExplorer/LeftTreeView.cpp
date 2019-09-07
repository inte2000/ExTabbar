// LeftTreeView.cpp : Implementation of CLeftTreeView

#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "LeftTreeView.h"


extern BOOL g_bFixFolderScroll; //only for win 7
extern BOOL g_bFolderAutoNavigate;
extern BOOL g_bNoFadeInOutExpand;
extern BOOL g_bTreeViewAutoScroll;
extern BOOL g_bTreeViewSimpleMode;
extern int g_nTreeViewIndent;

const UINT_PTR TIMER_NAVIGATE = 'CLSH';
const UINT_PTR m_TreeParentSubclassId = 0x19950905;

static LRESULT CALLBACK SubclassLeftTreeParentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    // when the tree selection changes start a timer to navigate to the new folder in 100ms
    if (uMsg == WM_NOTIFY && ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((NMTREEVIEW*)lParam)->action == TVC_BYKEYBOARD)
        SetTimer(((NMHDR*)lParam)->hwndFrom, TIMER_NAVIGATE, 100, NULL);
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CLeftTreeView::LeftTreeSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CLeftTreeView* pThisView = (CLeftTreeView*)dwRefData;
    ATLASSERT(pThisView != nullptr);
 
    if (uMsg == TVM_ENSUREVISIBLE)
    {
        // HACK! there is a bug in Win7 Explorer and when the selected folder is expanded for the first time it sends TVM_ENSUREVISIBLE for
        // the root tree item. This causes the navigation pane to scroll up. To work around the bug we ignore TVM_ENSUREVISIBLE if it tries
        // to show the root item and it is not selected
        if (IsWindows7() && g_bFixFolderScroll)
        {
            HTREEITEM hItem = (HTREEITEM)lParam;
            if (!TreeView_GetParent(hWnd, hItem) && !(TreeView_GetItemState(hWnd, hItem, TVIS_SELECTED) & TVIS_SELECTED))
                return 0;
        }
    }

    if (uMsg == WM_TIMER && wParam == TIMER_NAVIGATE)
    {
        LogInfo(_T("CLeftTreeView subclass got TIMER_NAVIGATE timer!"));
        // time to navigate to the new folder (simulate pressing Space)
        PostMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
        KillTimer(hWnd, TIMER_NAVIGATE);
        return 0;
    }

    if (uMsg == WM_CHAR && wParam == ' ')
        return 0; // ignore the Space character (to stop the tree view from beeping)

    if (uMsg == TVM_SETEXTENDEDSTYLE && wParam == (TVS_EX_FADEINOUTEXPANDOS | TVS_EX_AUTOHSCROLL | 0x80000000) && lParam == 0)
    {
        wParam &= 0x7FFFFFFF;

        if (g_bFolderAutoNavigate)
            SetWindowSubclass(GetParent(hWnd), SubclassLeftTreeParentProc, m_TreeParentSubclassId, 0);

        if (g_bNoFadeInOutExpand)
            wParam &= ~TVS_EX_FADEINOUTEXPANDOS;

        int indent = g_nTreeViewIndent;

        if (g_bTreeViewAutoScroll)
        {
            LogInfo(_T("CLeftTreeView hack treeview TVM_SETEXTENDEDSTYLE message"));
            SetWindowTheme(hWnd, NULL, NULL);
            DWORD style = GetWindowLong(hWnd, GWL_STYLE);
            style &= ~TVS_NOHSCROLL;
            if (g_bTreeViewSimpleMode)
            {
                style |= TVS_SINGLEEXPAND | TVS_TRACKSELECT;
                style &= ~TVS_HASLINES;
            }
            else
            {
                style |= TVS_HASLINES;
                style &= ~(TVS_SINGLEEXPAND | TVS_TRACKSELECT);
                wParam |= TVS_EX_FADEINOUTEXPANDOS;
                HIMAGELIST images = TreeView_GetImageList(hWnd, TVSIL_NORMAL);
                int cx, cy;
                ImageList_GetIconSize(images, &cx, &cy);
                indent = cx + 3;
            }
            SetWindowLong(hWnd, GWL_STYLE, style);
        }
        else
        {
            wParam &= ~TVS_EX_AUTOHSCROLL;
        }

        if (indent >= 0)
            TreeView_SetIndent(hWnd, indent);

        if (wParam == 0) return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CLeftTreeView::Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hExplorerWnd)
{
    m_spShellBrowser = spShellBrowser;
    m_hTreeWnd = hExplorerWnd;
    
    ::SetWindowSubclass(m_hTreeWnd, LeftTreeSubclassProc, m_TreeSubclassId, (DWORD_PTR)this);
    ::PostMessage(m_hTreeWnd, TVM_SETEXTENDEDSTYLE, TVS_EX_FADEINOUTEXPANDOS | TVS_EX_AUTOHSCROLL | 0x80000000, 0);

    m_bSubclassTree = TRUE;
    return m_bSubclassTree;
}

void CLeftTreeView::Unintialize()
{
    if(m_bSubclassTree)
    {
        ::RemoveWindowSubclass(::GetParent(m_hTreeWnd), SubclassLeftTreeParentProc, m_TreeParentSubclassId);
        ::RemoveWindowSubclass(m_hTreeWnd, LeftTreeSubclassProc, m_TreeSubclassId);
        m_bSubclassTree = FALSE;
    }
}

void CLeftTreeView::ModifyTreeItem()
{
    ATLASSERT(!m_bTreeItemModified);

    //RemoveTreeItem(hWnd);

    m_bTreeItemModified = TRUE;
}

