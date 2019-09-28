#include "pch.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "DataObject.h"
#include "DragDropData.h"
#include "ShellTabItem.h"
#include "ShellBrowserEx.h"
#include "ExplorerWindow.h" //
#include "TeTabctrl.h"

LRESULT CTeTabCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LogTrace(_T("CTeTabCtrl::OnCreate() invoked"));

    HIMAGELIST hSysImgList = GetSystemImageList(FALSE);
    if (hSysImgList == NULL)
    {
        LogError(_T("CTeTabCtrl::OnCreate() get system imagelist failed!"));
        return 0;
    }

    m_sysImgList.Attach(hSysImgList);
    CImageList oldImgList = SetImageList(hSysImgList);
    if(!oldImgList.IsNull())
        oldImgList.Destroy();

    bHandled = FALSE;
    return 0;
}

LRESULT CTeTabCtrl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HIMAGELIST hSysImgList = m_sysImgList.Detach();
    if (hSysImgList != NULL)
        ::ImageList_Destroy(hSysImgList);
    
    bHandled = FALSE;
    return 0;
}

/*
LRESULT CTeTabCtrl::OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTC2ITEMS* pNmItem = (NMCTC2ITEMS*)pnmh;
    return 0;
}
*/

LRESULT CTeTabCtrl::OnCloseButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    ATLASSERT(m_pExplorerWnd != nullptr);

    NMCTCITEM* pNtItem = reinterpret_cast<NMCTCITEM*>(pnmh);

    if (pNtItem->iItem >= 0)
    {
        int nItemCount = GetItemCount();
        if (nItemCount == 1) //last tab￡?should give an alert to close explorer window?
        {
        }
        InternalRemoveItem(pNtItem->iItem);
        if (GetItemCount() <= 0)
            m_pExplorerWnd->CloseExplorerWindow();
    }

    return 0;
}

