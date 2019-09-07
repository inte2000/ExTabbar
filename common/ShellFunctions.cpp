#include "pch.h"
#include "ShellWrapper.h"
#include "ShellFunctions.h"

BOOL IsExplorerAppWindow(CComPtr<IWebBrowser2>& pwb2)
{
    HWND hWnd = NULL;
    HRESULT hr = pwb2->get_HWND((LONG_PTR*)& hWnd);
    if (hr == S_OK)
    {
        TCHAR szClass[256] = { 0 };
        ::GetClassName(hWnd, szClass, 256); //xp is "ExploreWClass", win7 and win10 is "CabinetWClass"
        if ((lstrcmp(szClass, _T("ExploreWClass")) == 0) || (lstrcmp(szClass, _T("CabinetWClass")) == 0))
        {
            return TRUE;
        }
    }

    return FALSE;
}
//,
HRESULT SetBrowserListviewMode(CComPtr<IShellBrowser>& spShellBrowser, FOLDERVIEWMODE mode)
{
    CComPtr<IShellView> pShellView;
    HRESULT hr = spShellBrowser->QueryActiveShellView(&pShellView);
    UINT err = ::GetLastError();
    if (hr == S_OK)
    {
        CComPtr<IFolderView> pFolderView;
        hr = pShellView->QueryInterface(IID_IFolderView, (void**)& pFolderView);
        if (hr == S_OK)
        {
            hr = pFolderView->SetCurrentViewMode(mode);
        }
    }

    return hr;
}

TString StringFromVariant(VARIANT* var)
{
    TString strRtn;
    if (var->vt == VT_BSTR)
    {
#ifdef UNICODE
        strRtn = var->bstrVal;
#else
        UINT len = ::SysStringLen(var->bstrVal);
        int _convert = ::WideCharToMultiByte(CP_ACP, 0, var->bstrVal, len, NULL, 0, NULL, NULL);
        char* chBuf = new char[_convert];
        if (chBuf != nullptr)
        {
            ::WideCharToMultiByte(CP_ACP, 0, var->bstrVal, len, chBuf, _convert, NULL, NULL);
            strRtn = chBuf;
            delete[] chBuf;
        }
#endif
    }

    return std::move(strRtn);
}

