// BandObject.cpp : Implementation of CBandObject

#include "stdafx.h"
#include "SimpleExt.h"
#include "BandObject.h"


// CBandObject
STDMETHODIMP CBandObject::SetSite(IUnknown* punkSite)
{
    HRESULT hr = S_OK;
    ReleaseObjects();
    
    if (punkSite != nullptr)
    {
        CComQIPtr<IServiceProvider> psp = punkSite;
        if (!psp)
            return E_FAIL;

        hr = psp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<LPVOID*>(&m_spWebBrowser2));
        if (!SUCCEEDED(hr))
            return hr;

        hr = punkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_spBandObjectSite);
        if (!SUCCEEDED(hr))
            return hr;

        m_hwndParent = NULL;
        CComQIPtr<IOleWindow> spOleWin = m_spBandObjectSite;
        if (spOleWin)
        {
            spOleWin->GetWindow(&m_hwndParent);
        }

        if (!m_hwndParent)
            return E_FAIL;

        return S_OK;
    }

    return hr;
}

STDMETHODIMP CBandObject::GetSite(REFIID riid, LPVOID* ppvReturn)
{
    *ppvReturn = nullptr;

    if (m_spBandObjectSite)
    {
        return m_spBandObjectSite->QueryInterface(riid, ppvReturn);
    }

    return E_FAIL;
}

void CBandObject::ReleaseObjects()
{
    if (m_spBandObjectSite)
    {
        m_spBandObjectSite.Release();
        m_spBandObjectSite = nullptr;
    }
    if (m_spWebBrowser2)
    {
        m_spWebBrowser2.Release();
        m_spWebBrowser2 = nullptr;
    }
}
