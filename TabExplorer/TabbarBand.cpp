// TabbarBand.cpp : Implementation of CTarbarBand

#include "pch.h"
#include "framework.h"
#include "TabbarBand.h"
#include "resource.h"
//#include "ExplorerBHO.h"
#include "SystemFunctions.h"
#include "dllmain.h"

// CTarbarBand - adds a tabbar band to Windows Explorer with

CTabbarBand::CTabbarBand( void )
{
}

// IDeskBand
STDMETHODIMP CTabbarBand::GetBandInfo( DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi )
{
    RECT rc = { 0, 0, 0, 0};
    m_BandWindow.GetBarWndRect(rc);
	if (pdbi)
	{
		if (pdbi->dwMask&DBIM_MINSIZE)
		{
			pdbi->ptMinSize.x=rc.right;
			pdbi->ptMinSize.y=rc.bottom;
		}
		if (pdbi->dwMask&DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.x=0; // ignored
			pdbi->ptMaxSize.y=-1;	// unlimited
		}
		if (pdbi->dwMask&DBIM_INTEGRAL)
		{
			pdbi->ptIntegral.x=0; // not sizeable
			pdbi->ptIntegral.y=0; // not sizeable
		}
		if (pdbi->dwMask&DBIM_ACTUAL)
		{
			pdbi->ptActual.x=rc.right;
			pdbi->ptActual.y=rc.bottom;
		}
		if (pdbi->dwMask&DBIM_TITLE)
		{
			*pdbi->wszTitle=0; // no title
		}
		if (pdbi->dwMask&DBIM_BKCOLOR)
		{
			//Use the default background color by removing this flag.
			pdbi->dwMask&=~DBIM_BKCOLOR;
		}
	}
	return S_OK;
}

// IOleWindow
STDMETHODIMP CTabbarBand::GetWindow( HWND* phwnd )
{
	if (!phwnd)
		return E_INVALIDARG;
	*phwnd = m_BandWindow.GetToolbar();
	return S_OK;
}

STDMETHODIMP CTabbarBand::ContextSensitiveHelp( BOOL fEnterMode )
{
	return S_OK;
}

// IDockingWindow
STDMETHODIMP CTabbarBand::CloseDW( unsigned long dwReserved )
{
	ShowDW(FALSE);
	return S_OK;
}

STDMETHODIMP CTabbarBand::ResizeBorderDW( const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved )
{
	// Not used by any band object.
	return E_NOTIMPL;
}

STDMETHODIMP CTabbarBand::ShowDW( BOOL fShow )
{
    // on Windows 7 get the current RBBS_BREAK state and save it in the registry to be restored later
    m_BandWindow.SaveRebarBreakState();

    ShowWindow(m_BandWindow.GetToolbar(),fShow?SW_SHOW:SW_HIDE);
	return S_OK;
}

// IObjectWithSite
STDMETHODIMP CTabbarBand::SetSite( IUnknown* pUnkSite )
{
    HRESULT hr;

    IObjectWithSiteImpl<CTabbarBand>::SetSite(pUnkSite);

    m_BandWindow.DestroyBarWnd();

	if (m_pWebBrowser && m_dwEventCookie!=0xFEFEFEFE)
		DispEventUnadvise(m_pWebBrowser,&DIID_DWebBrowserEvents2);
	
    m_pWebBrowser=NULL;

    if (m_spBandObjectSite)
        m_spBandObjectSite.Release();

	//If punkSite is not NULL, a new site is being set.
	if (pUnkSite)
	{
        CComQIPtr<IServiceProvider> pProvider = pUnkSite;
        if (pProvider)
        {
            CComPtr<IShellBrowser> pBrowser;
            pProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void**)& pBrowser);

            //Get the parent window.
            HWND hWndParent = NULL;
            CComQIPtr<IOleWindow> pOleWindow = pUnkSite;
            if (pOleWindow)
                pOleWindow->GetWindow(&hWndParent);

            if (!IsWindow(hWndParent))
                return E_FAIL;

            hr = pUnkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)& m_spBandObjectSite);
            if (!SUCCEEDED(hr))
                return hr;

            if (!m_BandWindow.CreateBarWnd(hWndParent))
                return E_FAIL;

            // listen for web browser notifications. we only care about DISPID_DOWNLOADCOMPLETE and DISPID_ONQUIT
            pProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)& m_pWebBrowser);
            if (m_pWebBrowser == NULL)
                return E_FAIL;

            if (m_dwEventCookie == 0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE when the sink is not advised
                DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);

            m_BandWindow.OnAttachExplorer(pBrowser, m_pWebBrowser);
        }
	}
	return S_OK;
}

STDMETHODIMP CTabbarBand::OnNavigateComplete( IDispatch *pDisp, VARIANT *URL )
{
	// this is called when the current folder changes. disable the Up button if this is the desktop folder
	m_BandWindow.UpdateToolbar();
	return S_OK;
}

STDMETHODIMP CTabbarBand::OnCommandStateChange( long Command, VARIANT_BOOL Enable )
{
	if (Command==CSC_NAVIGATEFORWARD)
	{
		SendMessage(m_BandWindow.GetToolbar(),TB_ENABLEBUTTON, ID_GOFORWARD,Enable?1:0);
	}
	if (Command==CSC_NAVIGATEBACK)
	{
		SendMessage(m_BandWindow.GetToolbar(),TB_ENABLEBUTTON, ID_GOBACK,Enable?1:0);
	}
	return S_OK;
}

STDMETHODIMP CTabbarBand::OnQuit( void )
{
	if (m_pWebBrowser && m_dwEventCookie!=0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE, when the sink is not advised
		return DispEventUnadvise(m_pWebBrowser,&DIID_DWebBrowserEvents2);
	
    return S_OK;
}
