#include "pch.h"
//#include <strsafe.h>
#include "SystemFunctions.h"
#include "DataObject.h"

#define  MAX_DATASTORAGE_SIZE    64

HGLOBAL GlobalClone(HGLOBAL hglobIn)
{
    HGLOBAL hglobOut = NULL;

    LPVOID pvIn = GlobalLock(hglobIn);
    if (pvIn) 
    {
        SIZE_T cb = GlobalSize(hglobIn);
        HGLOBAL hglobOut = GlobalAlloc(GMEM_FIXED, cb);
        if (hglobOut) 
        {
            CopyMemory(hglobOut, pvIn, cb);
        }
        GlobalUnlock(hglobIn);
    }

    return hglobOut;
}

IUnknown* GetCanonicalIUnknown(IUnknown* punk)
{
    IUnknown* punkCanonical = nullptr;
    if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown, (LPVOID*)& punkCanonical)))
    {
        punkCanonical->Release();
    }
    else
    {
        punkCanonical = punk;
    }

    return punkCanonical;
}

CDataObject::CDataObject()
{
    m_lRefCount = 1;
    m_dsCount = 0;
    m_dataStorage = (PDATASTORAGETYPE)::CoTaskMemAlloc(sizeof(DATASTORAGETYPE) * MAX_DATASTORAGE_SIZE);
}

CDataObject::CDataObject(FORMATETC* fmt, STGMEDIUM* stgmed, int count) : CDataObject()
{
    int dc = count > MAX_DATASTORAGE_SIZE ? MAX_DATASTORAGE_SIZE : count;
    for (int i = 0; i < dc; i++)
    {
        InternalAddData(&fmt[i], &stgmed[i], TRUE);
    }
}

CDataObject::~CDataObject()
{
    for (UINT i = 0; i < m_dsCount; ++i)
    { 
        if(m_dataStorage[i].formatEtc.ptd != nullptr)
            ::CoTaskMemFree(m_dataStorage[i].formatEtc.ptd);

        ::ReleaseStgMedium(&m_dataStorage[i].stgMedium);
    }
    ::CoTaskMemFree(m_dataStorage);
    m_dataStorage = nullptr;
    m_dsCount = 0;
}

__inline CLIPFORMAT GetClipboardFormat(CLIPFORMAT* pcf, PCWSTR pszForamt)
{
    if (*pcf == 0)
    {
        *pcf = (CLIPFORMAT)::RegisterClipboardFormat(pszForamt);
    }
    return *pcf;
}

BOOL CDataObject::SetDropTip(DROPIMAGETYPE type, LPCTSTR pszMsg, LPCTSTR pszInsert)
{
    DROPDESCRIPTION dd = { type };

    WStrFromTString(dd.szMessage, ARRAYSIZE(dd.szMessage), pszMsg);
    WStrFromTString(dd.szInsert, ARRAYSIZE(dd.szInsert), pszInsert ? pszInsert : L"");

    static CLIPFORMAT s_cfDropDescription = 0;
    return SetBlob(GetClipboardFormat(&s_cfDropDescription, CFSTR_DROPDESCRIPTION), &dd, sizeof(dd));
}

// IUnknown interface
HRESULT STDMETHODCALLTYPE CDataObject::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IDataObject || iid == IID_IUnknown)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE CDataObject::AddRef(void)
{
    InterlockedIncrement(&m_lRefCount);
    return m_lRefCount;
}

ULONG STDMETHODCALLTYPE CDataObject::Release(void)
{
    ULONG ulRefCount = InterlockedDecrement(&m_lRefCount);
    if (ulRefCount == 0)
    {
        delete this;
    }

    return ulRefCount;
}

//CF_TEXT
// IDataObject interface
HRESULT STDMETHODCALLTYPE CDataObject::GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    if ((pFormatEtc == nullptr) || (pMedium == nullptr))
    {
        return E_INVALIDARG;
    }

    PDATASTORAGETYPE pde = nullptr;
    //ATLTRACE(_T("CDataObject::GetData(cfFormat=%d, tymed=%d)\n"), pFormatEtc->cfFormat, pFormatEtc->tymed);
    HRESULT hr = FindFormatEtc(pFormatEtc, &pde, FALSE);
    if (SUCCEEDED(hr))
    {
        hr = AddRefStgMedium(&pde->stgMedium, pMedium, FALSE);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CDataObject::GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    UNREFERENCED_PARAMETER(pFormatEtc);
    UNREFERENCED_PARAMETER(pMedium);

    return DV_E_FORMATETC;// E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObject::QueryGetData(FORMATETC* pFormatEtc)
{
    if (pFormatEtc == nullptr) 
    { 
        return E_INVALIDARG; 
    }

    PDATASTORAGETYPE pde = nullptr;
    return FindFormatEtc(pFormatEtc, &pde, FALSE);
}

HRESULT STDMETHODCALLTYPE CDataObject::GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut)
{
    //*pFormatEtcOut = *pFormatEct;
    //pFormatEtcOut->ptd = nullptr;
    return DATA_S_SAMEFORMATETC;
}

HRESULT STDMETHODCALLTYPE CDataObject::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
    if ((pFormatEtc == nullptr) || (pMedium == nullptr))
    { 
        return E_INVALIDARG; 
    }     
    //if (!fRelease) 
    //    return E_NOTIMPL;

   //ATLTRACE(_T("CDataObject::SetData(cfFormat=%d, tymed=%d, fRelease=%d, stgTymed=%d)\n"), 
                //pFormatEtc->cfFormat, pFormatEtc->tymed, fRelease, pMedium->tymed);
    PDATASTORAGETYPE pde = nullptr;
    HRESULT hr = FindFormatEtc(pFormatEtc, &pde, TRUE);
    if (SUCCEEDED(hr))
    {
        if (pde->stgMedium.tymed)
        {
            ::ReleaseStgMedium(&pde->stgMedium);
            ::ZeroMemory(&pde->stgMedium, sizeof(STGMEDIUM));
        }

        if (fRelease)
        {
            pde->stgMedium = *pMedium;
            hr = S_OK;
        }
        else
        {
            hr = AddRefStgMedium(pMedium, &pde->stgMedium, TRUE);
        }
        pde->formatEtc.tymed = pde->stgMedium.tymed;

        //Subtlety! Break circular reference loop
        if (GetCanonicalIUnknown(pde->stgMedium.pUnkForRelease) 
            == GetCanonicalIUnknown(static_cast<IDataObject *>(this)))
        {
            pde->stgMedium.pUnkForRelease->Release();
            pde->stgMedium.pUnkForRelease = nullptr;
        }
    }

    ATLTRACE(_T("DataObject::SetData(obj=%p) add  cfFormat=%d, stgMedium=0x%x (tymed=%d)\n"), this, pFormatEtc->cfFormat, pMedium->hGlobal, pMedium->tymed);
    return hr;
}

//派生类需要重写这个接口
HRESULT STDMETHODCALLTYPE CDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc)
{
    if (ppEnumFormatEtc == nullptr)
    { 
        return E_INVALIDARG; 
    }    
    
    //ATLTRACE(_T("CDataObject::EnumFormatEtc(dwDirection=%d)\n"),   dwDirection);

    *ppEnumFormatEtc = NULL;
    HRESULT hr = E_NOTIMPL;  

    if (DATADIR_GET == dwDirection) 
    { 
        //for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however        
        //to support all Windows platforms we need to implement IEnumFormatEtc ourselves.   

        FORMATETC* ptcs = new (std::nothrow) FORMATETC[m_dsCount];
        if (ptcs == nullptr)
            return E_OUTOFMEMORY;

        for (UINT i = 0; i < m_dsCount; i++)
            ptcs[i] = m_dataStorage[i].formatEtc;

        hr = SHCreateStdEnumFmtEtc(m_dsCount, ptcs, ppEnumFormatEtc);

        delete[] ptcs;
/*
        just support CF_UNICODETEXT， for test only
        FORMATETC rgfmtetc[] = 
        { 
            { CF_UNICODETEXT, NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL }, 
        };        
        hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppEnumFormatEtc); 
*/
    }
    
    return hr;
}

HRESULT STDMETHODCALLTYPE CDataObject::DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
    UNREFERENCED_PARAMETER(pFormatEtc);
    UNREFERENCED_PARAMETER(advf);    
    UNREFERENCED_PARAMETER(pAdvSink);
    UNREFERENCED_PARAMETER(pdwConnection);   

    return  OLE_E_ADVISENOTSUPPORTED; //E_NOTIMPL
}

HRESULT STDMETHODCALLTYPE CDataObject::DUnadvise(DWORD dwConnection)
{
    UNREFERENCED_PARAMETER(dwConnection);
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT STDMETHODCALLTYPE CDataObject::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise)
{
    UNREFERENCED_PARAMETER(ppEnumAdvise);
    return OLE_E_ADVISENOTSUPPORTED;
}

//protected
HRESULT CDataObject::InternalAddData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
    if (m_dsCount >= MAX_DATASTORAGE_SIZE)
        return E_FAIL;

    PDATASTORAGETYPE rgde = &m_dataStorage[m_dsCount];
    ::CopyMemory(&rgde->formatEtc, pFormatEtc, sizeof(FORMATETC));
    if (fRelease)
    {
        ::CopyMemory(&rgde->stgMedium, pMedium, sizeof(STGMEDIUM));
    }
    else
    {
        ::ZeroMemory(&rgde->stgMedium, sizeof(STGMEDIUM));
        CopyStgMedium(&rgde->stgMedium, pMedium);
    }
    m_dsCount++;

    return S_OK;
}

HRESULT CDataObject::FindFormatEtc(FORMATETC* pFormatEtc, PDATASTORAGETYPE* ppde, BOOL bAdd)
{
    *ppde = nullptr;

    //compare two DVTARGETDEVICE structure is hard, so we don't even try
    if (pFormatEtc->ptd != nullptr)
        return DV_E_DVTARGETDEVICE;

    // check each of ourformats in turn to see if one matches
    for(UINT i = 0; i < m_dsCount; i++)
    {        
        if((m_dataStorage[i].formatEtc.cfFormat == pFormatEtc->cfFormat)
            && (m_dataStorage[i].formatEtc.dwAspect == pFormatEtc->dwAspect)
            && (m_dataStorage[i].formatEtc.lindex == pFormatEtc->lindex))
        {      
            if (bAdd || (m_dataStorage[i].formatEtc.tymed & pFormatEtc->tymed))
            {
                *ppde = &m_dataStorage[i];
                return S_OK;
            }
            else
            {
                return DV_E_TYMED;
            }
        }    
    }

    if (!bAdd)
        return DV_E_FORMATETC;

    if (m_dsCount >= MAX_DATASTORAGE_SIZE)
        return E_OUTOFMEMORY;

    PDATASTORAGETYPE rgde = &m_dataStorage[m_dsCount];
    rgde->formatEtc = *pFormatEtc;
    ::ZeroMemory(&rgde->stgMedium, sizeof(STGMEDIUM));
    *ppde = rgde;
    m_dsCount++;

    return S_OK;
}

HRESULT CDataObject::SetBlob(CLIPFORMAT cf, const void* pvBlob, UINT cbBlob)
{
    void* pv = ::GlobalAlloc(GPTR, cbBlob);    
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;    
    if (SUCCEEDED(hr)) 
    {
        CopyMemory(pv, pvBlob, cbBlob);        
        FORMATETC fmte = { cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        // The STGMEDIUM structure is used to define how to handle a global memory transfer. 
        // This structure includes a flag, tymed, which indicates the medium 
        // to be used, and a union comprising pointers and a handle for getting whichever
        // medium is specified in tymed.       
        STGMEDIUM medium = {};        
        medium.tymed = TYMED_HGLOBAL;        
        medium.hGlobal = pv;        
        hr = this->SetData(&fmte, &medium, TRUE);        
        if (FAILED(hr))        
        {            
            ::GlobalFree(pv);        
        }   
    }    
    return hr;
}

HRESULT CDataObject::AddRefStgMedium(STGMEDIUM* pstgmIn, STGMEDIUM* pstgmOut, BOOL fCopyIn)
{
    HRESULT hres = S_OK;
    STGMEDIUM stgmOut = *pstgmIn;

    if (pstgmIn->pUnkForRelease == NULL && !(pstgmIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE))) 
    {
        if (fCopyIn) 
        {
            /* Object needs to be cloned */
            if (pstgmIn->tymed == TYMED_HGLOBAL) 
            {
                stgmOut.hGlobal = GlobalClone(pstgmIn->hGlobal);
                if (!stgmOut.hGlobal) 
                {
                    hres = E_OUTOFMEMORY;
                }
            }
            else 
            {
                hres = DV_E_TYMED;      /* Don't know how to clone GDI objects */
            }
        }
        else 
        {
            stgmOut.pUnkForRelease = static_cast<IDataObject*>(this);
        }
    }

    if (SUCCEEDED(hres)) {
        switch (stgmOut.tymed) {
        case TYMED_ISTREAM:
            stgmOut.pstm->AddRef();
            break;
        case TYMED_ISTORAGE:
            stgmOut.pstg->AddRef();
            break;
        }
        if (stgmOut.pUnkForRelease) {
            stgmOut.pUnkForRelease->AddRef();
        }

        *pstgmOut = stgmOut;
    }

    return hres;
}

HRESULT CreateDataObject(FORMATETC* fmtetc, STGMEDIUM* stgmeds, UINT count, IDataObject** ppDataObject) 
{ 
    if (ppDataObject == nullptr)        
        return E_INVALIDARG;     
    *ppDataObject = new (std::nothrow) CDataObject(fmtetc, stgmeds, count);
    return (*ppDataObject) ? S_OK : E_OUTOFMEMORY; 
}

#if 0
void DbgTraceDataObject(IDataObject* pDataObject) 
{
    FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };    
    STGMEDIUM stgmed;     
    // ask the IDataObject for some CF_TEXT data, stored as aHGLOBAL    
    if(pDataObject->GetData(&fmtetc,&stgmed) == S_OK)    
    {        
        // We need to lock the HGLOBAL handle because we can't        
        // be sure if this is GMEM_FIXED (i.e. normal heap) data ornot        
        char *data = (char*)GlobalLock(stgmed.hGlobal);         
        printf("%s\n",data);         
        
        // cleanup        
        GlobalUnlock(stgmed.hGlobal);        
        ReleaseStgMedium(&stgmed);    
    }
}
#endif
