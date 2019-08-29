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

public:

	DECLARE_WND_CLASS_EX(_T("WTL_TeTabCtrl"), CS_DBLCLKS, COLOR_WINDOW)

	//We have nothing special to add.
	//BEGIN_MSG_MAP(thisClass)
	//	CHAIN_MSG_MAP(baseClass)
	//END_MSG_MAP()
};
