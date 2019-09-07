#pragma once

#include "framework.h"
#include "atlgdix.h"
//#include "wtlaero.h"

//class CWzToolbar : public CToolBarCtrlT<CWindow>
class CWzToolbar : public CWindowImpl<CWzToolbar, CToolBarCtrl >,
                   public WTL::CThemeImpl<CWzToolbar>
{
public:
    CWzToolbar()
    {
        SetThemeClassList(_T("globals")); 
    }

protected:
    BEGIN_MSG_MAP(CWzToolbar)
        CHAIN_MSG_MAP(WTL::CThemeImpl<CWzToolbar>)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    
    LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnToolbarNotifyCustomDraw(NMTBCUSTOMDRAW* pDraw, BOOL& handled);

};
