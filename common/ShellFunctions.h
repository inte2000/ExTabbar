#pragma once


BOOL IsExplorerAppWindow(CComPtr<IWebBrowserApp>& pwba);
BOOL IsExplorerAppWindow2(CComPtr<IWebBrowser2>& pwb2);
HRESULT SetBrowserListviewMode(CComPtr<IShellBrowser>& spShellBrowser, FOLDERVIEWMODE mode);

TString StringFromVariant(VARIANT* var);

int ExplorerWindowFromPoint(const POINT& pt, std::vector<HWND>& wnds);
