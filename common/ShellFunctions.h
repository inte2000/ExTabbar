#pragma once


BOOL IsExplorerAppWindow(CComPtr<IWebBrowser2>& pwb2);
HRESULT SetBrowserListviewMode(CComPtr<IShellBrowser>& spShellBrowser, FOLDERVIEWMODE mode);

TString StringFromVariant(VARIANT* var);
