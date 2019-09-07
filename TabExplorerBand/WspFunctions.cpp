#include "pch.h"
#include "framework.h"
#include "WspFunctions.h"

void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
    ::SetBkColor(hDC, clr);
    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
} 
