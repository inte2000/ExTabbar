// SimpleShlExt.cpp : Implementation of CSimpleShlExt

#include "stdafx.h"
#include "resource.h"
#include "SimpleExt.h"
#include "SimpleShlExt.h"

/////////////////////////////////////////////////////////////////////////////
// CSimpleShlExt
STDMETHODIMP CSimpleShlExt::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
    FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg = { TYMED_HGLOBAL };
    HDROP     hDrop;

    // Look for CF_HDROP data in the data object.
    if ( FAILED( pDataObj->GetData ( &fmt, &stg ) ))
    {
        // Nope! Return an "invalid argument" error back to Explorer.
        return E_INVALIDARG;
    }

    // Get a pointer to the actual data.
    hDrop = (HDROP) GlobalLock ( stg.hGlobal );

    // Make sure it worked.
    if ( NULL == hDrop )
        return E_INVALIDARG;

    // Sanity check - make sure there is at least one filename.
    UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
    HRESULT hr = S_OK;

    if ( 0 == uNumFiles )
    {
        GlobalUnlock ( stg.hGlobal );
        ReleaseStgMedium ( &stg );
        return E_INVALIDARG;
    }

    // Get the name of the first file and store it in our member variable m_szFile.
    if ( 0 == DragQueryFile ( hDrop, 0, m_szFile, MAX_PATH ) )
        hr = E_INVALIDARG;

    GlobalUnlock ( stg.hGlobal );
    ReleaseStgMedium ( &stg );

    return hr;
}

STDMETHODIMP CSimpleShlExt::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
    if ( uFlags & CMF_DEFAULTONLY )
        return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

    InsertMenu ( hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, _T("SimpleShlExt Test Item") );

    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 );
}

STDMETHODIMP CSimpleShlExt::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
    USES_CONVERSION;

    // Check idCmd, it must be 0 since we have only one menu item.
    if ( 0 != idCmd )
        return E_INVALIDARG;

    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer.
    if ( uFlags & GCS_HELPTEXT )
    {
        LPCTSTR szText = _T("This is the simple shell extension's help");

        if ( uFlags & GCS_UNICODE )
        {
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API.
            lstrcpynW ( (LPWSTR) pszName, T2CW(szText), cchMax );
        }
        else
        {
            // Use the ANSI string copy API to return the help string.
            lstrcpynA ( pszName, T2CA(szText), cchMax );
        }

        return S_OK;
    }

    return E_INVALIDARG;
}

void TestGetShellObject()
{
    IShellWindows *psw = NULL;  
    HRESULT hr = ::CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&psw);
    if (SUCCEEDED(hr)) 
    {
        long lCount = 0;  
        hr = psw->get_Count(&lCount);  

        //SWC_EXPLORER 指出获取explorer打开的窗口，如果没有打开任何窗口则会调用失败  
        //VISTA及以后的版本可以用SWC_DESKTOP，此时获取的是桌面窗口，即使没有其他shell窗口，该调用也不会失败.  
        VARIANT vpidl;  
        vpidl.vt = VT_UI4;  
        vpidl.ulVal = SWC_EXPLORER;  
        IDispatch *pdisp = NULL;  
        hr = psw->Item(vpidl, &pdisp);  
        if (hr == S_OK) 
        {
            IWebBrowserApp *pwba = NULL;  
            hr = pdisp->QueryInterface(IID_IWebBrowserApp, (void**)&pwba);  
 
        }
    
        VARIANT v;
        V_VT(&v) = VT_I4;
        IDispatch  *pdisp1;
        BOOL fFound = FALSE;
        for (V_I4(&v) = 0; !fFound && psw->Item(v, &pdisp1) == S_OK; V_I4(&v)++) 
        {
            IWebBrowserApp *pwba;
            if (SUCCEEDED(pdisp1->QueryInterface(IID_IWebBrowserApp, (void**)&pwba))) 
            {
            }
            pdisp1->Release();
        }
    }
}

STDMETHODIMP CSimpleShlExt::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
    // If lpVerb really points to a string, ignore this function call and bail out.
    if ( 0 != HIWORD( pCmdInfo->lpVerb ) )
        return E_INVALIDARG;

    // Get the command index - the only valid one is 0.
    switch ( LOWORD( pCmdInfo->lpVerb) )
    {
        case 0:
        {
            TCHAR szMsg [MAX_PATH + 32];

            wsprintf ( szMsg, _T("The selected file was:\n\n%s"), m_szFile );
            TestGetShellObject();
            //MessageBox ( pCmdInfo->hwnd, szMsg, _T("SimpleShlExt"), MB_ICONINFORMATION );

            return S_OK;
        }
        break;
		default:
            return E_INVALIDARG;
        break;
    }
}
#if 0
//这是改变图标的第一种方式
/*
The first IExtractIcon method called is GetIconLocation(). This function looks at the file (whose name was 
stored during IPersistFile::Load()) and returns a filename/index pair, as discussed above. 

In method 1, our extension's GetIconLocation() function gets the size of the file, and based on the size, 
returns an index between 0 and 3 inclusive. This brings up one drawback of this method - you need to keep 
track of your resource IDs and make sure they're in the right order. Our extension only has 4 icons, so this 
bookkeeping isn't difficult, but if you have many more icons, or if you add/remove some icons from your 
project, you have to be careful with your resource IDs.
*/
STDMETHODIMP CSimpleShlExt::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
{
    DWORD     dwFileSizeLo, dwFileSizeHi;
    DWORDLONG qwSize;
    HANDLE    hFile;
 
    hFile = CreateFile ( m_szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
 
    if ( INVALID_HANDLE_VALUE == hFile )
        return S_FALSE;    // tell the shell to use a default icon

    dwFileSizeLo = GetFileSize ( hFile, &dwFileSizeHi );

    CloseHandle ( hFile );

    if ( (DWORD) -1 == dwFileSizeLo  &&  GetLastError() != NO_ERROR )
        return S_FALSE;    // tell the shell to use a default icon

    qwSize = DWORDLONG(dwFileSizeHi)<<32 | dwFileSizeLo;

    TCHAR szModulePath[MAX_PATH];

    GetModuleFileName ( _Module.GetResourceInstance(), szModulePath, MAX_PATH );

    lstrcpyn ( szIconFile, szModulePath, cchMax );

    if ( 0 == qwSize )
        *piIndex = 0;
    else if ( qwSize < 4096 )
        *piIndex = 1;
    else if ( qwSize < 8192 )
        *piIndex = 2;
    else
        *piIndex = 3;

    *pwFlags = 0;
    return S_OK;
 }

STDMETHODIMP CSimpleShlExt::Extract(LPCTSTR pszFile, UINT nIconIndex,  HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize )
{
    return S_FALSE;    // Tell the shell to do the extracting itself.
}
#endif

//这是改变图标的第二种方式
/*
Method 2 involves our extension extracting the icons itself, and bypassing Explorer's icon cache. Using this 
method, IExtractIcon::Extract() is always called, and it is responsible for loading the icons and returning 
two HICONs to Explorer - one for the large icon, and one for the small icon. The advantage of this method is that 
you don't have to worry about keeping your icons' resource IDs in order. The downside is that it bypasses 
Explorer's icon cache, which conceivably might slow down file browsing a bit if you go into a directory with a ton of 
text files.

GetIconLocation() is similar to method 1, but it has a bit less work to do since it only needs to get the size 
of the file.
One thing to note is that when using method 2, returning the GIL_SIMULATEDOC flag from GetIconLocation() 
has no effect.
This method stores the file size in the CTxtIconShlExt object, and does the
icon extraction manually.  This way is more complicated, but I prefer it since
you don't have to do bookkeeping of your resource IDs.

*/
STDMETHODIMP CSimpleShlExt::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
{
    DWORD  dwFileSizeLo, dwFileSizeHi;
    HANDLE hFile;

    hFile = CreateFile ( m_szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if ( INVALID_HANDLE_VALUE == hFile )
        return S_FALSE;    // tell the shell to use a default icon

    dwFileSizeLo = GetFileSize ( hFile, &dwFileSizeHi );

    CloseHandle ( hFile );

    if ( (DWORD) -1 == dwFileSizeLo  &&  GetLastError() != NO_ERROR )
        return S_FALSE;    // tell the shell to use a default icon

    m_qwFileSize = ((DWORDLONG) dwFileSizeHi)<<32 | dwFileSizeLo;
    *pwFlags = GIL_NOTFILENAME | GIL_DONTCACHE;
    return S_OK;
}

STDMETHODIMP CSimpleShlExt::Extract(LPCTSTR pszFile, UINT nIconIndex,  HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize )
{
    UINT uIconID;
    WORD wSmallIconSize = HIWORD(nIconSize), wLargeIconSize = LOWORD(nIconSize);

    // Determine which icon to use, depending on the file size.
    if ( 0 == m_qwFileSize )
        uIconID = IDI_ZERO_BYTES;
    else if ( m_qwFileSize < 4096 )
        uIconID = IDI_UNDER_4K;
    else if ( m_qwFileSize < 8192 )
        uIconID = IDI_UNDER_8K;
    else 
        uIconID = IDI_OVER_8K;
 
    // Load the icons!
    if ( NULL != phiconLarge )
    {
        *phiconLarge = (HICON) LoadImage ( _Module.GetResourceInstance(),
                                       MAKEINTRESOURCE(uIconID), IMAGE_ICON,
                                       wLargeIconSize, wLargeIconSize,
                                       LR_DEFAULTCOLOR );
    }
 
    if ( NULL != phiconSmall )
    {
        *phiconSmall = (HICON) LoadImage ( _Module.GetResourceInstance(),
                                       MAKEINTRESOURCE(uIconID), IMAGE_ICON,
                                       wSmallIconSize, wSmallIconSize,
                                       LR_DEFAULTCOLOR );
    }

    return S_OK;
}





