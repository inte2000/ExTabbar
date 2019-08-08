// SimpleShlExt.h : Declaration of the CSimpleShlExt

#ifndef __SIMPLESHLEXT_H_
#define __SIMPLESHLEXT_H_

#include <comdef.h>
#include <shlobj.h>
#include <atlconv.h>
/////////////////////////////////////////////////////////////////////////////
// CSimpleShlExt

class ATL_NO_VTABLE CSimpleShlExt : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSimpleShlExt, &CLSID_SimpleShlExt>,
    public IShellExtInit,
    public IContextMenu,
    public IPersistFile,
    public IExtractIcon
{
public:
    CSimpleShlExt() { }

    DECLARE_REGISTRY_RESOURCEID(IDR_SIMPLESHLEXT)

    BEGIN_COM_MAP(CSimpleShlExt)
        COM_INTERFACE_ENTRY(IShellExtInit)
        COM_INTERFACE_ENTRY(IContextMenu)
        COM_INTERFACE_ENTRY(IPersistFile)
        COM_INTERFACE_ENTRY(IExtractIcon)
    END_COM_MAP()

public:
    // IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);
    
    // IPersistFile
    STDMETHOD(GetClassID)( CLSID* )       { return E_NOTIMPL; }
    STDMETHOD(IsDirty)()                  { return E_NOTIMPL; }
    STDMETHOD(Save)( LPCOLESTR, BOOL )    { return E_NOTIMPL; }
    STDMETHOD(SaveCompleted)( LPCOLESTR ) { return E_NOTIMPL; }
    STDMETHOD(GetCurFile)( LPOLESTR* )    { return E_NOTIMPL; }
    STDMETHOD(Load)( LPCOLESTR wszFile, DWORD /*dwMode*/ )
    { 
        USES_CONVERSION;
        lstrcpyn(m_szFile, OLE2CT(wszFile), MAX_PATH);
        //MessageBox ( NULL, m_szFile, _T("Load"), MB_ICONINFORMATION );

        return S_OK;
    }

    // IExtractIcon
    STDMETHODIMP GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax,
                               int* piIndex, UINT* pwFlags);
    STDMETHODIMP Extract(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge,
                       HICON* phiconSmall, UINT nIconSize);
protected:
    TCHAR m_szFile [MAX_PATH];
    DWORDLONG m_qwFileSize; 
};

#endif //__SIMPLESHLEXT_H_
