
#pragma once
#include "resource.h"       // main symbols

class CExplorerWindow;

class CAddressBar final
{
public:
    CAddressBar ();
	
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hAddressBarWnd, CExplorerWindow *pExeplorerWnd);
    void Unintialize();
    HWND GetHwnd() const { return m_hAddressBarWnd; }
    bool SetManaging(bool bManaging);
    void GetWindowRect(LPRECT pRect) { ATLASSERT(m_hAddressBarWnd != NULL);  ::GetWindowRect(m_hAddressBarWnd, pRect); }
    void MovePosition(LPRECT pRect);
protected:
    void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    void ForceRedraw();
    void ReclameTabSpace();
    void UpdateTabSizeAndPosition(RECT& WndRect);

protected:	
    CComPtr<IShellBrowser> m_spShellBrowser;
    HWND m_hAddressBarWnd;
    BOOL m_bSubclassed;
    UINT_PTR m_SubclassId;
    CExplorerWindow* m_pExplorerWnd;

    bool  m_bManaging;

public:
    static LRESULT CALLBACK AddressBarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                          DWORD_PTR dwRefData );
};

