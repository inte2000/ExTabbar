// BandObject.h : Declaration of the CBandObject

#pragma once
#include "resource.h"       // main symbols

#include <comdef.h>
#include <shlobj.h>
#include <atlconv.h>

//#include "atl_test_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CBandObject

class ATL_NO_VTABLE CBandObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBandObject, &CLSID_BandObject>,
	public IObjectWithSiteImpl<CBandObject>,
	public IDispatchImpl<IBandObject, &IID_IBandObject, &LIBID_SIMPLEEXTLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBandObject()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BANDOBJECT)


BEGIN_COM_MAP(CBandObject)
	COM_INTERFACE_ENTRY(IBandObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

public:
    //IObjectWithSite
    STDMETHODIMP SetSite(IUnknown* punkSite);
    STDMETHODIMP GetSite(REFIID riid, LPVOID* ppvReturn);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

protected:
    HWND m_hwndParent;
    CComPtr<IInputObjectSite> m_spBandObjectSite;
    CComPtr<IWebBrowser2> m_spWebBrowser2;

protected:
    void ReleaseObjects();
};

OBJECT_ENTRY_AUTO(__uuidof(BandObject), CBandObject)
