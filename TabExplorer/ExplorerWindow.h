
#pragma once
#include "resource.h"       // main symbols

#include "TabbarWindow.h"
#include "TravelBand.h"
#include "LeftTreeView.h"
#include "StatusBar.h"
#include "ShellTabWindow.h"
#include "AddressBar.h"
#include "TravelLogMgmt.h"

class CExplorerWindow final
{
public:
    CExplorerWindow();
    ~CExplorerWindow() {}
	
    void CloseExplorerWindow();
	BOOL OnExplorerAttach(CComPtr<IWebBrowser2>& spWebBrowser2, CComPtr<IShellBrowser>& spShellBrowser, CComPtr<ITravelLogStg>& spTravelLogStg);
    void OnExplorerDetach();
    HRESULT OnBeforeNavigate(const TString& strUrl);
    void OnNavigateComplete(const TString& strUrl);
    HWND GetHwnd() const { return m_hExplorerWnd; }
    void Show(BOOL bShow) { ATLASSERT(m_hExplorerWnd != NULL); ::ShowWindow(m_hExplorerWnd, bShow ? SW_SHOW : SW_HIDE); }
    void UpdateTabSizeAndPosition(RECT &StwRect);
    int GetTabbarHeight() const { return m_iTabbarHeight; }
    bool SetTravelBandLogEntries(const std::vector<CNavigatedPoint>& logs, const CNavigatedPoint& curItem, bool bTravelToCurrent);
    void GetTravelBandLogEntries(std::vector<CNavigatedPoint>& logs, int maxWanted, CNavigatedPoint& curItem) const;
    const CShellTabWindow& GetShellTabWindow() const { ATLASSERT(m_hExplorerWnd != NULL); return m_ShellTabWnd; }
    CShellTabWindow& GetShellTabWindow() { ATLASSERT(m_hExplorerWnd != NULL); return m_ShellTabWnd; }
protected:
    BOOL SubclassStatusBar();
    BOOL SubclassLeftTree(HWND hTreeView);
    BOOL InstallExplorerHook();
    void UninstallExplorerHook();

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
    CTravelLogMgmt m_TravelLogMgmt;

    HHOOK m_Hook;
public:
    static CExplorerWindow *m_pThisExplorer;
	static LRESULT CALLBACK HookExplorerProc( int code, WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK ExplorerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

