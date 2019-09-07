
#pragma once
#include "resource.h"       // main symbols

extern const UINT_PTR TIMER_NAVIGATE;

class CLeftTreeView
{
public:
    CLeftTreeView()
	{
        m_bSubclassTree = FALSE;
        m_TreeSubclassId = 0x19980862;
        m_bTreeItemModified = FALSE;
        m_hTreeWnd = NULL;
	}
	
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hTreeWnd);
    void Unintialize();
    HWND GetHwnd() const { return m_hTreeWnd; }
    BOOL IsTreeModified() { return m_bTreeItemModified; }
    void ModifyTreeItem();
protected:
	
protected:	
	CComPtr<IShellBrowser> m_spShellBrowser;
    HWND m_hTreeWnd;
    BOOL m_bSubclassTree;
    BOOL m_bTreeItemModified;
    UINT_PTR m_TreeSubclassId;

public:
    static LRESULT CALLBACK LeftTreeSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                          DWORD_PTR dwRefData );
};

