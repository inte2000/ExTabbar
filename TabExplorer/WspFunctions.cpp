#include "pch.h"
#include "ShellFoldersMap.h"
#include "SystemFunctions.h"
#include "WspFunctions.h"

/*
void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
    ::SetBkColor(hDC, clr);
    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}
*/

BOOL GetCIDLDataByParseName(const TString& parseName, CIDLEx& cidl, CIDListData& IdlData)
{
    if (IsNamespacePath(parseName))
    {
        if (!TeGetFolderMap().FindFolder(parseName, IdlData))
        {
            //log here
            return FALSE;
        }

        auto rawIdl = IdlData.GetIDLData();
        cidl.CreateByIdListData(std::get<0>(rawIdl), std::get<1>(rawIdl));
    }
    else
    {
        cidl = CIDLEx::CIDLFromFullPath(parseName);
        if (cidl.IsEmpty())
            return FALSE;
        IdlData = CIDListData(&cidl, parseName);
    }

    return TRUE;
}
