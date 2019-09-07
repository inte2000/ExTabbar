
#pragma once
#include "resource.h"       // main symbols

class CExplorerWindow
{
public:
	CExplorerWindow()
	{
		m_bResetStatus=true;
		m_bForceRefresh=false;
        m_bUpToolbarAdd = false;
        m_hExplorerWnd = NULL;
	}
	
	BOOL OnExplorerAttach(CComPtr<IWebBrowser2>& spWebBrowser2, CComPtr<IShellBrowser>& spShellBrowser);
    void OnExplorerDetach();
    void OnNavigateComplete();
    HWND GetHwnd() const { return m_hExplorerWnd; }

protected:
    void SetUpButtonStatus(bool bEnable);
    BOOL AddUpButtonToExplorBar();
    void RemoveUpButtonFromExplorBar();
    BOOL SubclassStatusBar();
    BOOL HookExplorer();
    void UnhookExplorer();

	enum
	{
		SPACE_SHOW=1, // show free space and selection size
		SPACE_TOTAL=2, // show total size when nothing is selected
		SPACE_WIN7=4, // running on Win7 (fix the status bar parts and show the disk free space)
	};
	
protected:	
	CWindow m_Toolbar;
    HWND m_hExplorerWnd;
    BOOL m_bUpToolbarAdd;
	HICON m_IconNormal, m_IconHot, m_IconPressed, m_IconDisabled;
	bool m_bResetStatus;
	bool m_bForceRefresh;
    CComPtr<IWebBrowser2> m_spWebBrowser2;
	CComPtr<IShellBrowser> m_spShellBrowser;

public:
	static __declspec(thread) HHOOK s_Hook;
	static LRESULT CALLBACK HookExplorer( int code, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK SubclassStatusProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                           DWORD_PTR dwRefData );
	static LRESULT CALLBACK RebarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                          DWORD_PTR dwRefData );
};

