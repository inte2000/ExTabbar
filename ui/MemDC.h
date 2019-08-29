//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule, keithr@europa.com,  Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//				 - 14/7/99 Added optional clip rect parameter [jgh]
//
//				 - 06/06/08  Added option to copy screen on construction
//
#if !defined(AFX_CMemDC_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_)
#define AFX_CMemDC_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_

#include "framework.h"

class CMemDC : public WTL::CDC 
{
private:
    WTL::CBitmapHandle m_bitmap; // Offscreen bitmap
    WTL::CBitmapHandle m_oldBitmap; // bitmap originally found in CMemDC
    WTL::CDCHandle m_PaintDC;
	CRect m_rect; // Rectangle of drawing area.
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CMemDC(HDC hDC, CRect rect = CRect(0,0,0,0), BOOL bCopyFirst = FALSE) : CDC(), m_oldBitmap(NULL), m_PaintDC(hDC)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		ATLASSERT(m_PaintDC != NULL); // If you asserted here, you passed in a NULL CDC.
			
		// Create a Memory DC
		CreateCompatibleDC(m_PaintDC);
		if ( rect == CRect(0,0,0,0) )
            m_PaintDC.GetClipBox(&m_rect);
		else
			m_rect = rect;

		m_bitmap.CreateCompatibleBitmap(m_PaintDC, m_rect.Width(), m_rect.Height());
		m_oldBitmap = SelectBitmap(m_bitmap);
		SetWindowOrg(m_rect.left, m_rect.top);
		if(bCopyFirst)
		{
			this->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                m_PaintDC, m_rect.left, m_rect.top, SRCCOPY);
		}
	}
		
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
	~CMemDC()
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		// Copy the offscreen bitmap onto the screen.
        m_PaintDC.BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
            m_hDC, m_rect.left, m_rect.top, SRCCOPY);
		//Swap back the original bitmap.
        SelectBitmap(m_oldBitmap);
        DeleteObject(m_bitmap);
	}
		
	// Allow usage as a pointer
	CMemDC* operator->() {return this;}
		
	// Allow usage as a pointer
	operator CMemDC*() {return this;}
};

#endif
// End CMemDC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

