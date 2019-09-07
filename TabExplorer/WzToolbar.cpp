#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "WzToolbar.h"

LRESULT CWzToolbar::OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMTBCUSTOMDRAW* pDraw = (NMTBCUSTOMDRAW*)pnmh;
    BOOL handled = TRUE;

    LRESULT hr = OnToolbarNotifyCustomDraw(pDraw, handled);
    if (handled)
        return hr;

    return 0;
}
LRESULT CWzToolbar::OnToolbarNotifyCustomDraw(NMTBCUSTOMDRAW* pDraw, BOOL& handled)
{
    handled = FALSE;

    if (pDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        //ATLTRACE(_T("dwDrawStage = 0x%08x, dwItemSpec = %d, uItemState=%d\n"), pDraw->nmcd.dwDrawStage, pDraw->nmcd.dwItemSpec, pDraw->nmcd.uItemState);
        handled = TRUE;
        return CDRF_NOTIFYITEMDRAW;
    }
    if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        //ATLTRACE(_T("dwDrawStage = 0x%08x, dwItemSpec = %d, uItemState=%d\n"), pDraw->nmcd.dwDrawStage, pDraw->nmcd.dwItemSpec, pDraw->nmcd.uItemState);
        //DrawThemeBackground(pDraw->nmcd.hdc, BP_PUSHBUTTON, PBS_NORMAL, &pDraw->nmcd.rc);
        //DrawThemeEdge(pDraw->nmcd.hdc, BP_PUSHBUTTON, PBS_NORMAL, &pDraw->nmcd.rc, EDGE_RAISED, BF_FLAT);
        CDCHandle dc(pDraw->nmcd.hdc);
        dc.FillSolidRect(&pDraw->nmcd.rc, GetSysColor(COLOR_WINDOW));
        handled = TRUE;
        return CDRF_DODEFAULT;
    }

    return 0;
}
