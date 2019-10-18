#include "pch.h"
#include "ShellWrapper.h"
#include "ShellFunctions.h"
#include "SystemFunctions.h"

BOOL IsExplorerAppWindow(CComPtr<IWebBrowserApp>& pwba)
{
    HWND hWnd = NULL;
    HRESULT hr = pwba->get_HWND((LONG_PTR*)& hWnd);
    if (hr == S_OK)
    {
        return IsExplorerWindow(hWnd);
    }

    return FALSE;
}

BOOL IsExplorerAppWindow2(CComPtr<IWebBrowser2>& pwb2)
{
    HWND hWnd = NULL;
    HRESULT hr = pwb2->get_HWND((LONG_PTR*)& hWnd);
    if (hr == S_OK)
    {
        return IsExplorerWindow(hWnd);
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

int ExplorerWindowFromPoint(const POINT& pt, std::vector<HWND>& wnds)
{
    CComPtr<IShellWindows> psw;
    HRESULT hr = psw.CoCreateInstance(CLSID_ShellWindows);
    if (SUCCEEDED(hr))
    {
        VARIANT v;
        V_VT(&v) = VT_I4;
        CComPtr<IDispatch> pdisp;
        for (V_I4(&v) = 0; psw->Item(v, &pdisp) == S_OK; V_I4(&v)++)
        {
            CComPtr<IWebBrowserApp> pwba;
            hr = pdisp->QueryInterface(__uuidof(IWebBrowserApp), (void**)& pwba);
            if (hr == S_OK)
            {
                HWND hWnd = NULL;
                hr = pwba->get_HWND((LONG_PTR*)& hWnd);
                if (hr == S_OK)
                {
                    if (IsExplorerWindow(hWnd) && IsPointInWindow(hWnd, pt))
                    {
                        wnds.push_back(hWnd);
                    }
                }
            }

            pdisp.Release();
        }
    }

    return static_cast<int>(wnds.size());
}
