
// TabbarBand.h : Declaration of the CTabbarBand

#pragma once
#include <vector>
#include "resource.h"  
#include "TabExplorer_i.h"
#include "TabbarBandWindow.h"  

// CTabbarBand

class ATL_NO_VTABLE CTabbarBand :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CTabbarBand, & CLSID_TabbarBand>,
    public IObjectWithSiteImpl<CTabbarBand>,
    public IDeskBand,
    public IDispEventImpl<1, CTabbarBand,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,1,1>
{
public:
    CTabbarBand( void );

	DECLARE_REGISTRY_RESOURCEID(IDR_TABBARBAND)

	BEGIN_SINK_MAP(CTabbarBand)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, OnCommandStateChange)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	END_SINK_MAP()

	BEGIN_COM_MAP(CTabbarBand)
		COM_INTERFACE_ENTRY( IOleWindow )
		COM_INTERFACE_ENTRY( IObjectWithSite )
		COM_INTERFACE_ENTRY_IID( IID_IDockingWindow, IDockingWindow )
		COM_INTERFACE_ENTRY_IID( IID_IDeskBand, IDeskBand )
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

	// IDeskBand
	STDMETHOD(GetBandInfo)( DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi );

	// IObjectWithSite
	STDMETHOD(SetSite)( IUnknown* pUnkSite );

	// IOleWindow
	STDMETHOD(GetWindow)( HWND* phwnd );
	STDMETHOD(ContextSensitiveHelp)( BOOL fEnterMode );

	// IDockingWindow
	STDMETHOD(CloseDW)( unsigned long dwReserved );
	STDMETHOD(ResizeBorderDW)( const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved );
	STDMETHOD(ShowDW)( BOOL fShow );

	// DWebBrowserEvents2
	STDMETHOD(OnBeforeNavigate2)( IDispatch *pDisp, VARIANT *URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel);
	STDMETHOD(OnNavigateComplete2)( IDispatch *pDisp, VARIANT *URL );
	STDMETHOD(OnCommandStateChange)( long Command, VARIANT_BOOL Enable );
	STDMETHOD(OnQuit)( void );

protected:
    void DoFirstNavigation(bool before, const TString& path);

protected:
    CTabbarBandWindow m_BandWindow;
    CComPtr<IShellBrowser> m_spShellBrowser;
	CComPtr<IWebBrowser2> m_pWebBrowser;
    CComPtr<IInputObjectSite> m_spBandObjectSite;
    CComPtr<ITravelLogStg> m_spTravelLog;

    bool m_IsShown;

};

OBJECT_ENTRY_AUTO(__uuidof(TabbarBand), CTabbarBand)
