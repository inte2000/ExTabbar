// TabbarBand.cpp : Implementation of CTarbarBand

#include "pch.h"
#include "framework.h"
#include "TabbarBand.h"
#include "resource.h"
//#include "ExplorerBHO.h"
#include "SystemFunctions.h"
#include "ShellFunctions.h"
#include "dllmain.h"
#include "DebugLog.h"
#include "HookLibManager.h"
#include "ShellGuids.h"
#include "GuidEx.h"

// CTarbarBand - adds a tabbar band to Windows Explorer with

CTabbarBand::CTabbarBand( void )
{
    m_IsShown = false;
}

// IDeskBand
STDMETHODIMP CTabbarBand::GetBandInfo( DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi )
{
    RECT rc = { 0, 0, 0, 0};
    m_BandWindow.GetBarWndRect(rc);
    LogInfo(_T("CTabbarBand::GetBandInfo(left:%d, top:%d, right:%d, bottom:%d)"), rc.left, rc.top, rc.right, rc.bottom);
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
	//*phwnd = m_BandWindow.GetToolbar();
    *phwnd = m_BandWindow.m_hWnd;
    return S_OK;
}

STDMETHODIMP CTabbarBand::ContextSensitiveHelp( BOOL fEnterMode )
{
	return S_OK;
}

// IDockingWindow
STDMETHODIMP CTabbarBand::CloseDW( unsigned long dwReserved )
{
    LogTrace(_T("CTabbarBand::CloseDW()"));
    m_BandWindow.ShowBarWnd(FALSE);
    //ShowDW(FALSE);
	return S_OK;
}

STDMETHODIMP CTabbarBand::ResizeBorderDW( const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved )
{
	// Not used by any band object.
	return E_NOTIMPL;
}

STDMETHODIMP CTabbarBand::ShowDW( BOOL fShow )
{
    LogTrace(_T("CTabbarBand::ShowDW(fShow = %d)"), fShow);
    // on Windows 7 get the current RBBS_BREAK state and save it in the registry to be restored later
    m_BandWindow.SaveRebarBreakState();

    m_BandWindow.ShowBarWnd(TRUE);
    return S_OK;
}

// IObjectWithSite
STDMETHODIMP CTabbarBand::SetSite( IUnknown* pUnkSite )
{
    HRESULT hr;

    LogTrace(_T("CTabbarBand::SetSite(%p)"), pUnkSite);
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
            pProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void**)& m_spShellBrowser);

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

            GetHookMgmt().InitShellBrowserHook((IShellBrowser*)m_spShellBrowser);
            
            CGuidEx guid3(strIID_ITravelLogStg);
            CGuidEx guid4(strIID_ITravelLogStg);
            hr = pProvider->QueryService(guid3, guid4, (void**)& m_spTravelLog);

            if (m_dwEventCookie == 0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE when the sink is not advised
                DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);

            m_BandWindow.OnAttachExplorer(m_spShellBrowser, m_pWebBrowser);
        }
	}
	return S_OK;
}

STDMETHODIMP CTabbarBand::OnBeforeNavigate2(IDispatch* pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel)
{
    TString strUrl = StringFromVariant(URL);

    if (!m_IsShown)
    {
        DoFirstNavigation(true, strUrl);
    }

    return S_OK;
}

STDMETHODIMP CTabbarBand::OnNavigateComplete2( IDispatch *pDisp, VARIANT *URL )
{
    TString strUrl = StringFromVariant(URL);

    if (!m_IsShown)
    {
        DoFirstNavigation(false, strUrl);
    }


    // this is called when the current folder changes. disable the Up button if this is the desktop folder
	m_BandWindow.UpdateToolbar();
	return S_OK;
}

STDMETHODIMP CTabbarBand::OnCommandStateChange( long Command, VARIANT_BOOL Enable )
{
    LogTrace(_T("CTabbarBand::OnCommandStateChange(Command=%d)"), Command);
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
    LogTrace(_T("CTabbarBand::OnQuit()"));
    if (m_pWebBrowser && m_dwEventCookie!=0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE, when the sink is not advised
		return DispEventUnadvise(m_pWebBrowser,&DIID_DWebBrowserEvents2);
	
    return S_OK;
}

void CTabbarBand::DoFirstNavigation(bool before, const TString& path)
{
    CIDLEx cidl;
    cidl.Set(path.c_str());
    m_BandWindow.AddNewTab(path, cidl);
}
