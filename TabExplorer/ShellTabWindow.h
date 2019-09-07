
#pragma once
#include "resource.h"       // main symbols

class CExplorerWindow;

class CShellTabWindow final
{
public:
    CShellTabWindow();
	
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hShellTabWnd, CExplorerWindow *pExeplorerWnd);
    void Unintialize();
    HWND GetHwnd() const { return m_hShellTabWnd; }
    bool SetManaging(bool bManaging);

protected:
    void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    void ForceRedraw();
    void ReclameTabSpace();
    void UpdateTabSizeAndPosition(RECT& WndRect);

protected:	
	CComPtr<IShellBrowser> m_spShellBrowser;
    HWND m_hShellTabWnd;
    BOOL m_bSubclassed;
    UINT_PTR m_SubclassId;
    CExplorerWindow* m_pExplorerWnd;

    bool  m_bManaging;
    RECT  m_LastParentPos;

public:
    static LRESULT CALLBACK ShellTabWndSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                          DWORD_PTR dwRefData );
};

