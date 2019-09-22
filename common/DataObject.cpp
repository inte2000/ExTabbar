#include "pch.h"
#include <strsafe.h>
#include "SystemFunctions.h"
#include "DataObject.h"

CDataObject::CDataObject()
{
    m_lRefCount = 1;
}

CDataObject::CDataObject(FORMATETC* fmt, STGMEDIUM* stgmed, int count)
{
    m_lRefCount = 1;
    for (int i = 0; i < count; i++)
    {
        InternalAddData(&fmt[i], &stgmed[i], FALSE);
    }
}

CDataObject::~CDataObject()
{
    int nSize = static_cast<int>(m_dataStorage.size());    
    for (int i = 0; i < nSize; ++i) 
    { 
        DATASTORAGETYPE& dataEntry = m_dataStorage.at(i);        
        ::ReleaseStgMedium(dataEntry.stgMedium);        
        delete dataEntry.stgMedium;        
        delete dataEntry.formatEtc; 
    }
}

__inline CLIPFORMAT GetClipboardFormat(CLIPFORMAT* pcf, PCWSTR pszForamt)
{
    if (*pcf == 0)
    {
        *pcf = (CLIPFORMAT)RegisterClipboardFormat(pszForamt);
    }
    return *pcf;
}

BOOL CDataObject::SetDropTip(DROPIMAGETYPE type, LPCTSTR pszMsg, LPCTSTR pszInsert)
{
    DROPDESCRIPTION dd = { type };

#ifdef UNICODE
    StringCchCopyW(dd.szMessage, ARRAYSIZE(dd.szMessage), pszMsg);
    StringCchCopyW(dd.szInsert, ARRAYSIZE(dd.szInsert), pszInsert ? pszInsert : L"");
#else
    int _convert = ::MultiByteToWideChar(CP_ACP, 0, (LPCCH)pszMsg, -1, dd.szMessage, ARRAYSIZE(dd.szMessage));
    dd.szMessage[_convert - 1] = 0;
    _convert = ::MultiByteToWideChar(CP_ACP, 0, (LPCCH)pszInsert, -1, dd.szInsert, ARRAYSIZE(dd.szInsert));
    dd.szInsert[_convert - 1] = 0;
#endif

    static CLIPFORMAT s_cfDropDescription = 0;
    return SetBlob(GetClipboardFormat(&s_cfDropDescription, CFSTR_DROPDESCRIPTION), &dd, sizeof(dd));
}

// IUnknown interface
HRESULT STDMETHODCALLTYPE CDataObject::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IDropSource || iid == IID_IUnknown)
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

// IDataObject interface
HRESULT STDMETHODCALLTYPE CDataObject::GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    if ((pFormatEtc == nullptr) || (pMedium == nullptr))
    {
        return E_INVALIDARG;
    }

    pMedium->hGlobal = NULL;
    for (const auto& dataEntry: m_dataStorage)
    { 
        if ((pFormatEtc->tymed & dataEntry.formatEtc->tymed) 
            && (pFormatEtc->dwAspect == dataEntry.formatEtc->dwAspect) 
            && (pFormatEtc->cfFormat == dataEntry.formatEtc->cfFormat))
        { 
            return CopyMedium(pMedium, dataEntry.stgMedium, dataEntry.formatEtc);
        } 
    }     
    
    return DV_E_FORMATETC;
}

HRESULT STDMETHODCALLTYPE CDataObject::GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    UNREFERENCED_PARAMETER(pFormatEtc);
    UNREFERENCED_PARAMETER(pMedium);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObject::QueryGetData(FORMATETC* pFormatEtc)
{
    if (pFormatEtc == nullptr) 
    { 
        return E_INVALIDARG; 
    }

    if (!(DVASPECT_CONTENT & pFormatEtc->dwAspect)) 
    { 
        return DV_E_DVASPECT; 
    }
    HRESULT hr = DV_E_TYMED;    
    for (const auto& dataEnrty : m_dataStorage)
    {
        if (dataEnrty.formatEtc->tymed & pFormatEtc->tymed)
        { 
            if (dataEnrty.formatEtc->cfFormat == pFormatEtc->cfFormat)
            { 
                return S_OK; 
            } 
            else 
            { 
                hr = DV_E_CLIPFORMAT; 
            } 
        } 
        else
        { 
            hr = DV_E_TYMED; 
        } 
    } 

    return hr;
}

HRESULT STDMETHODCALLTYPE CDataObject::GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut)
{
    *pFormatEtcOut = *pFormatEct;
    pFormatEtcOut->ptd = NULL;
    return DATA_S_SAMEFORMATETC;
}

HRESULT STDMETHODCALLTYPE CDataObject::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
    if ((pFormatEtc == nullptr) || (pMedium == nullptr))
    { 
        return E_INVALIDARG; 
    }     
    
    if (pFormatEtc->tymed != pMedium->tymed)
    { 
        return E_FAIL; 
    }    

    return InternalAddData(pFormatEtc, pMedium, fRelease);
}

//派生类需要重写这个接口
HRESULT STDMETHODCALLTYPE CDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc)
{
    if (ppEnumFormatEtc == nullptr)
    { 
        return E_INVALIDARG; 
    }    
    *ppEnumFormatEtc = NULL;
    HRESULT hr = E_NOTIMPL;  

    if (DATADIR_GET == dwDirection) 
    { 
        //for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however        
        //to support all Windows platforms we need to implement IEnumFormatEtc ourselves.   

        FORMATETC* ptcs = new (std::nothrow) FORMATETC[m_dataStorage.size()];
        if (ptcs == nullptr)
            return E_OUTOFMEMORY;

        //for(const auto& x : m_dataStorage)
        int i = 0;
        for_each(m_dataStorage.begin(), m_dataStorage.end(), [ptcs, &i](const auto& x) { ptcs[i++] = *x.formatEtc; });
        hr = SHCreateStdEnumFmtEtc(static_cast<UINT>(m_dataStorage.size()), ptcs, ppEnumFormatEtc);

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

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObject::DUnadvise(DWORD dwConnection)
{
    UNREFERENCED_PARAMETER(dwConnection);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObject::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise)
{
    UNREFERENCED_PARAMETER(ppEnumAdvise);
    return E_NOTIMPL;
}

//protected
HRESULT CDataObject::InternalAddData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
    FORMATETC* fetc = new (std::nothrow) FORMATETC;
    if (fetc == nullptr)
        return E_OUTOFMEMORY;

    STGMEDIUM* pStgMed = new (std::nothrow) STGMEDIUM;
    if (pStgMed == nullptr)
    {
        delete fetc;
        return E_OUTOFMEMORY;
    }

    ::ZeroMemory(fetc, sizeof(FORMATETC));
    ::ZeroMemory(pStgMed, sizeof(STGMEDIUM));
    *fetc = *pFormatEtc;
    if (fRelease)
    {
        *pStgMed = *pMedium;
    }
    else
    {
        CopyMedium(pStgMed, pMedium, pFormatEtc);
    }
    DATASTORAGETYPE dataEntry = { fetc, pStgMed };
    m_dataStorage.push_back(dataEntry);

    return S_OK;
}

int CDataObject::LookupFormatEtc(FORMATETC* pFormatEtc)
{
    // check each of ourformats in turn to see if one matches
    for(int i = 0; i < static_cast<int>(m_dataStorage.size()); i++)
    {        
        if( (m_dataStorage[i].formatEtc->tymed & pFormatEtc->tymed)
            && (m_dataStorage[i].formatEtc->cfFormat == pFormatEtc->cfFormat)
            &&  (m_dataStorage[i].formatEtc->dwAspect == pFormatEtc->dwAspect) )
        {            
            //return index of stored format            
            return i;        
        }    
    }

    // error, format notfound    
    return -1;
}

HRESULT CDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
    if ((pMedDest == nullptr) || (pMedSrc == nullptr) || (pFmtSrc == nullptr)) 
    { 
        return E_INVALIDARG; 
    }    

    switch (pMedSrc->tymed) 
    { 
    case TYMED_HGLOBAL:        
        pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);        
        break;    
    case TYMED_GDI:        
        pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);        
        break;    
    case TYMED_MFPICT:        
        pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);       
        break;    
    case TYMED_ENHMF:        
        pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);        
        break;    
    case TYMED_FILE:        
        pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);        
        break;   
    case TYMED_ISTREAM:        
        pMedDest->pstm = pMedSrc->pstm;        
        pMedSrc->pstm->AddRef();        
        break;    
    case TYMED_ISTORAGE:        
        pMedDest->pstg = pMedSrc->pstg;        
        pMedSrc->pstg->AddRef();       
        break;    
    case TYMED_NULL:    
    default:        
        break; 
    }    
    pMedDest->tymed = pMedSrc->tymed;   
    pMedDest->pUnkForRelease = nullptr;    
    if (pMedSrc->pUnkForRelease != nullptr)
    { 
        pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;        
        pMedSrc->pUnkForRelease->AddRef(); 
    }    
    
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