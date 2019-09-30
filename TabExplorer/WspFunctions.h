#pragma once

#include "ShellWrapper.h"
#include "IDListData.h"

//void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr);

BOOL GetCIDLDataByParseName(const TString& parseName, CIDLEx& cidl, CIDListData& IdlData);
