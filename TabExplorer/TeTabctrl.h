#pragma once

#include "framework.h"
//#include "dllmain.h"
#include "atlgdix.h"
#include "DotNetTabCtrl.h"
#include "DragDropTarget.h"
#include "DragDropSource.h"

class CExplorerWindow;
class CShellBrowserEx;

//CDotNetTabCtrlImpl
class CTeTabCtrl : public CDotNetTabCtrlImpl<CTeTabCtrl>
{
protected:
	typedef CTeTabCtrl thisClass;
	typedef CDotNetButtonTabCtrlImpl<CTeTabCtrl> baseClass;

// Constructors:
public:
	CTeTabCtrl()
	{
        m_pShellBrowser = nullptr;
        m_pExplorerWnd = nullptr;
    }

	DECLARE_WND_CLASS_EX(_T("WTL_TeTabCtrl"), CS_DBLCLKS, COLOR_WINDOW)

    void SetShellObject(CExplorerWindow* pExplorerWnd, CShellBrowserEx* pShellBrowser)
    {
        m_pShellBrowser = pShellBrowser;
        m_pExplorerWnd = pExplorerWnd;
    }

protected:
    
    BEGIN_MSG_MAP(CTeTabCtrl)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        //REFLECTED_NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnSelChange)
        CHAIN_MSG_MAP(CDotNetTabCtrlImpl<CTeTabCtrl>)
        REFLECTED_NOTIFY_CODE_HANDLER(CTCN_CLOSE, OnCloseButton)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //LRESULT OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnCloseButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    BOOL InternalRemoveItem(int nItem);
    int InternalInsertItem(int nItem, LPCTSTR sText, int nImage, LPCTSTR sToolTip, ULONG_PTR data, bool bSelectItem = false);

protected:
    CImageList m_sysImgList;
    CShellBrowserEx* m_pShellBrowser;
    //for parent explorer window
    CExplorerWindow* m_pExplorerWnd;
};
