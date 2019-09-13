// TabExplorerBHO.cpp : Implementation of CTabExplorerBHO

#include "pch.h"
#include "framework.h"
#include "TabExplorerBHO.h"
#include "dllmain.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include <Ntquery.h>
#include "SystemFunctions.h"
#include "ShellFunctions.h"
#include "DebugLog.h"
#include "HookLibManager.h"


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

            GetHookMgmt().InitShellBrowserHook(m_spShellBrowser);
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

STDMETHODIMP CTabExplorerBHO::OnBeforeNavigate2(IDispatch* pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel)
{
    TString strUrl = StringFromVariant(URL);

    HRESULT hr = m_ExplorerWnd.OnBeforeNavigate(strUrl);

    LogTrace(_T("CTabExplorerBHO::OnNavigateComplete2(%s), result = 0x%08x"), strUrl.c_str(), hr);

    return S_OK;
}

STDMETHODIMP CTabExplorerBHO::OnNavigateComplete2( IDispatch *pDisp, VARIANT *URL )
{
    TString strUrl = StringFromVariant(URL);

    LogTrace(_T("CTabExplorerBHO::OnNavigateComplete2()"));

    m_ExplorerWnd.OnNavigateComplete(strUrl);

	return S_OK;
}

STDMETHODIMP CTabExplorerBHO::OnCommandStateChange(long Command, VARIANT_BOOL Enable)
{
    LogTrace(_T("CTabExplorerBHO::OnCommandStateChange(Command=%d)"), Command);
/*
    if (Command == CSC_NAVIGATEFORWARD)
    {
        SendMessage(m_BandWindow.GetToolbar(), TB_ENABLEBUTTON, ID_GOFORWARD, Enable ? 1 : 0);
    }
    if (Command == CSC_NAVIGATEBACK)
    {
        SendMessage(m_BandWindow.GetToolbar(), TB_ENABLEBUTTON, ID_GOBACK, Enable ? 1 : 0);
    }
*/
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
