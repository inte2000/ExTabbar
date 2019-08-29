// TabExplorerBHO.cpp : Implementation of CTabExplorerBHO

#include "pch.h"
#include "framework.h"
#include "TabExplorerBHO.h"
#include "dllmain.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include <Ntquery.h>
#include "SystemFunctions.h"
#include "DebugLog.h"


// CTabExplorerBHO - a browser helper object that implements Alt+Enter for the folder tree

HRESULT STDMETHODCALLTYPE CTabExplorerBHO::SetSite( IUnknown *pUnkSite )
{
    //HRESULT hr;
    
    LogTrace(_T("TabExplorerBHO SetSite(%p) invoked"), pUnkSite);

    IObjectWithSiteImpl<CTabExplorerBHO>::SetSite(pUnkSite);
	if (pUnkSite)
	{
		CComQIPtr<IServiceProvider> pProvider = pUnkSite;
		if (pProvider)
		{
            pProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)& m_spWebBrowser2);
            pProvider->QueryService(SID_SShellBrowser,IID_IShellBrowser,(void**)& m_spShellBrowser);

            if (!m_ExplorerWnd.OnExplorerAttach(m_spWebBrowser2, m_spShellBrowser))
            {
                return E_FAIL;
            }

            // listen for web browser notifications. we only care about DISPID_DOWNLOADCOMPLETE and DISPID_ONQUIT
            if (m_spWebBrowser2)
            {
                if (m_dwEventCookie == 0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE when the sink is not advised
                    DispEventAdvise(m_spWebBrowser2, &DIID_DWebBrowserEvents2);
            }
		}
	}
	else
	{
		if (m_spWebBrowser2 && m_dwEventCookie!=0xFEFEFEFE)
			DispEventUnadvise(m_spWebBrowser2,&DIID_DWebBrowserEvents2);

        m_ExplorerWnd.OnExplorerDetach();
        m_spShellBrowser = NULL;
        m_spWebBrowser2 = NULL;
	}
	return S_OK;
}
/*
STDMETHODIMP CTabExplorerBHO::GetSite(REFIID riid, LPVOID* ppvReturn)
{
    *ppvReturn = nullptr;

    if (m_spBandObjectSite)
    {
        return m_spBandObjectSite->QueryInterface(riid, ppvReturn);
    }

    return E_FAIL;
}
*/
STDMETHODIMP CTabExplorerBHO::OnNavigateComplete( IDispatch *pDisp, VARIANT *URL )
{
    m_ExplorerWnd.OnNavigateComplete();

	return S_OK;
}

STDMETHODIMP CTabExplorerBHO::OnQuit( void )
{
    HRESULT hr = S_OK;

    LogTrace(_T("TabExplorerBHO OnQuit() invoked"));
    if (m_spWebBrowser2 && m_dwEventCookie != 0xFEFEFEFE) // ATL's event cookie is 0xFEFEFEFE, when the sink is not advised
    {
        hr = DispEventUnadvise(m_spWebBrowser2, &DIID_DWebBrowserEvents2);
    }
	
    m_ExplorerWnd.OnExplorerDetach();
    return hr;
}
