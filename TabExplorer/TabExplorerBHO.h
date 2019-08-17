// TabExplorerBHO.h : Declaration of the CTabExplorerBHO

#pragma once
#include "resource.h"       // main symbols

#include "TabExplorer_i.h"
#include "ExplorerWindow.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CTabExplorerBHO

class ATL_NO_VTABLE CTabExplorerBHO:
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTabExplorerBHO, &CLSID_TabExplorerBHO>,
	public IObjectWithSiteImpl<CTabExplorerBHO>,
	public IDispatchImpl<ITabExplorerBHO, &IID_ITabExplorerBHO, &LIBID_TabExplorerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventImpl<1,CTabExplorerBHO,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,1,1>
{
public:
    CTabExplorerBHO()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TABEXPLORERBHO)

	BEGIN_SINK_MAP(CTabExplorerBHO)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	END_SINK_MAP()

	BEGIN_COM_MAP(CTabExplorerBHO)
		COM_INTERFACE_ENTRY(ITabExplorerBHO)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
    //STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvReturn);

	// DWebBrowserEvents2
	STDMETHOD(OnNavigateComplete)( IDispatch *pDisp, VARIANT *URL );
	STDMETHOD(OnQuit)( void );

private:
	CComPtr<IShellBrowser> m_spShellBrowser;
	CComPtr<IWebBrowser2> m_spWebBrowser2;
    CExplorerWindow  m_ExplorerWnd;
};

OBJECT_ENTRY_AUTO(__uuidof(TabExplorerBHO), CTabExplorerBHO)

