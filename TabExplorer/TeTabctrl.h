#pragma once

#include "framework.h"
//#include "dllmain.h"
#include "atlgdix.h"
#include "DotNetTabCtrl.h"

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
	}

	DECLARE_WND_CLASS_EX(_T("WTL_TeTabCtrl"), CS_DBLCLKS, COLOR_WINDOW)
protected:
    
    BEGIN_MSG_MAP(CTeTabCtrl)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        //REFLECTED_NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnSelChange)
        CHAIN_MSG_MAP(CDotNetTabCtrlImpl<CTeTabCtrl>)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
    CImageList m_sysImgList;
};
