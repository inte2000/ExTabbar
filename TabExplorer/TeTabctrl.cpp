#include "pch.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "DataObject.h"
#include "DragDropData.h"
#include "ShellTabItem.h"
#include "ShellBrowserEx.h"
#include "ExplorerWindow.h" //
#include "WspFunctions.h"
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

BOOL CTeTabCtrl::InternalRemoveItem(int nItem)
{
    int nItemCount = GetItemCount();
    if ((nItem < 0) || (nItem >= nItemCount))
        return FALSE;

    if (m_iDragItem == nItem)
        m_iDragItem = -1;

    if (m_iDragItemOriginal == nItem)
        m_iDragItemOriginal = -1;

    int nCurSel = GetCurSel();
    if (nCurSel == nItem)
    {
        if (nItem == (nItemCount - 1))
            SetCurSel(nItem - 1);
        else
            SetCurSel(nItem + 1);
    }
    
    return DeleteItem(nItem);
}

int CTeTabCtrl::InternalInsertItem(int nItem, LPCTSTR sText, int nImage, LPCTSTR sToolTip, ULONG_PTR data, bool bSelectItem)
{
    int index = InsertItem(nItem, sText, nImage, sToolTip, false);
    if (index >= 0)
    {
        SetItemData(index, data);
        if(bSelectItem)
            SetCurSel(index);
    }

    return index;
}


/*
int InsertItem(int nItem, LPCTSTR sText = NULL, int nImage = -1, LPCTSTR sToolTip = NULL, bool bSelectItem = false)
SetItemDrag();
*/

