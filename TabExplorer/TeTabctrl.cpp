#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
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

LRESULT CTeTabCtrl::OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTC2ITEMS* pNmItem = (NMCTC2ITEMS*)pnmh;
/*
    CShellTabItem *pti = GetItemData(pNmItem->iItem2);
    if (pti != NULL)
    {
        pti->
    }
*/
    return 0;
}
