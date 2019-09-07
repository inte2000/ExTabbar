
#pragma once
#include "resource.h"       // main symbols

#include "TabbarWindow.h"
#include "TravelBand.h"
#include "LeftTreeView.h"
#include "StatusBar.h"
#include "ShellTabWindow.h"
#include "AddressBar.h"

class CExplorerWindow final
{
public:
    CExplorerWindow();
    ~CExplorerWindow() {}
	
	BOOL OnExplorerAttach(CComPtr<IWebBrowser2>& spWebBrowser2, CComPtr<IShellBrowser>& spShellBrowser);
    void OnExplorerDetach();
    HRESULT OnBeforeNavigate(const TString& strUrl);
    void OnNavigateComplete(const TString& strUrl);
    HWND GetHwnd() const { return m_hExplorerWnd; }
    void UpdateTabSizeAndPosition(RECT &StwRect);
    int GetTabbarHeight() const { return m_iTabbarHeight; }
protected:
    BOOL SubclassStatusBar();
    BOOL SubclassLeftTree(HWND hTreeView);
    BOOL HookExplorer();
    void UnhookExplorer();

    //void OnSize(DWORD_PTR dwSizeType, DWORD nWidth, DWORD nHeight);
    BOOL OnBrowseObject(LPCITEMIDLIST pidl, UINT wFlags);

protected:	
    CTabbarWindow m_TabbarWnd;
    CTravelBand m_TravelBand; //for win7 add up button
    CLeftTreeView m_LeftTree;
    CStatusBar m_Statusbar;
    CShellTabWindow m_ShellTabWnd;
    CAddressBar m_AddressBar;

    int   m_iTabbarHeight;

    HWND m_hExplorerWnd;
    UINT_PTR m_ExplorerSubclassId;

    CComPtr<IWebBrowser2> m_spWebBrowser2;
	CComPtr<IShellBrowser> m_spShellBrowser;

public:
    static CExplorerWindow *m_pThisExplorer;
	static __declspec(thread) HHOOK s_Hook;
	static LRESULT CALLBACK HookExplorer( int code, WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK ExplorerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

