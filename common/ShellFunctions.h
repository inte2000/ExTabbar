#pragma once


HRESULT SetBrowserListviewMode(CComPtr<IShellBrowser>& spShellBrowser, FOLDERVIEWMODE mode);

TString StringFromVariant(VARIANT* var);

