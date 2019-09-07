#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "TeTabctrl.h"

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
