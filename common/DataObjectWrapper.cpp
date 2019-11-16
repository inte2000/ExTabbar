#include "pch.h"
#include "SystemFunctions.h"
#include "DataObjectWrapper.h"


CDataObjectWrapper::CDataObjectWrapper() 
{ 
    m_dataObj = nullptr; 
#ifdef DEBUG
    m_bAddRef = false;
#endif
}

CDataObjectWrapper::CDataObjectWrapper(CDataObjectWrapper&& dow) 
{ 
    m_dataObj = dow.m_dataObj; 
    dow.m_dataObj = nullptr; 
#ifdef DEBUG
    m_bAddRef = false;
    dow.m_bAddRef = false;
#endif
}

void CDataObjectWrapper::Attach(IDataObject* dataObj, bool bRefAdd)
{
    Release();
    m_dataObj = dataObj;
#ifdef DEBUG
    m_bAddRef = bRefAdd;
#endif
    if (bRefAdd)
        m_dataObj->AddRef();
}

IDataObject* CDataObjectWrapper::Detach()
{
    IDataObject* tmp = m_dataObj;
    m_dataObj = nullptr;
#ifdef DEBUG
    m_bAddRef = false;
#endif

    return tmp;
}

void CDataObjectWrapper::Release()
{
    if (m_dataObj != nullptr)
    {
#ifdef DEBUG
        ATLASSERT(m_bAddRef);
#endif
        m_dataObj->Release();
        m_dataObj = nullptr;
    }
}

CDataObjectWrapper& CDataObjectWrapper::operator = (const CDataObjectWrapper& dow)
{
    if (this != &dow)
    {
        Release();
        Attach(dow.m_dataObj, true);
    }

    return *this;
}

CDataObjectWrapper& CDataObjectWrapper::operator = (CDataObjectWrapper&& dow)
{
    if (this != &dow)
    {
        Release();
        m_dataObj = dow.m_dataObj;
        dow.m_dataObj = nullptr;
#ifdef DEBUG
        m_bAddRef = false;
        dow.m_bAddRef = false;
#endif
    }

    return *this;
}

//    bool m_bClipboard;
LPFORMATETC WzFillFormatEtc(LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat)
{
    if (lpFormatEtc == NULL && cfFormat != 0)
    {
        lpFormatEtc->cfFormat = cfFormat;
        lpFormatEtc->ptd = NULL;
        lpFormatEtc->dwAspect = DVASPECT_CONTENT;
        lpFormatEtc->lindex = -1;
        lpFormatEtc->tymed = (DWORD)-1;
    }
    return lpFormatEtc;
}

//copy from MFC: _AfxCopyStgMedium
extern BOOL CopyStgMedium(LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource, CLIPFORMAT cfFormat);
#if 0
BOOL CopyStgMedium(LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource, CLIPFORMAT cfFormat)
{
    if (lpDest->tymed == TYMED_NULL)
    {
        if (lpSource->tymed == TYMED_NULL)
            return FALSE;
        switch (lpSource->tymed)
        {
        case TYMED_ENHMF:
        case TYMED_HGLOBAL:
            if (sizeof(HGLOBAL) != sizeof(HENHMETAFILE))
                return FALSE;
            lpDest->tymed = lpSource->tymed;
            lpDest->hGlobal = NULL;
            break;  // fall through to CopyGlobalMemory case
        case TYMED_ISTREAM:
            lpDest->pstm = lpSource->pstm;
            lpDest->pstm->AddRef();
            lpDest->tymed = TYMED_ISTREAM;
            return TRUE;
        case TYMED_ISTORAGE:
            lpDest->pstg = lpSource->pstg;
            lpDest->pstg->AddRef();
            lpDest->tymed = TYMED_ISTORAGE;
            return TRUE;
        case TYMED_MFPICT:
        {
            // copy LPMETAFILEPICT struct + embedded HMETAFILE
            HGLOBAL hDest = CopyGlobalMemoryHandle(NULL, lpSource->hGlobal);
            if (hDest == NULL)
                return FALSE;
            LPMETAFILEPICT lpPict = (LPMETAFILEPICT)::GlobalLock(hDest);
            lpPict->hMF = ::CopyMetaFile(lpPict->hMF, NULL);
            if (lpPict->hMF == NULL)
            {
                ::GlobalUnlock(hDest);
                ::GlobalFree(hDest);
                return FALSE;
            }
            ::GlobalUnlock(hDest);

            // fill STGMEDIUM struct
            lpDest->hGlobal = hDest;
            lpDest->tymed = TYMED_MFPICT;
        }
        return TRUE;

        case TYMED_GDI:
            lpDest->tymed = TYMED_GDI;
            lpDest->hGlobal = NULL;
            break;
        case TYMED_FILE:
        {
            lpDest->tymed = TYMED_FILE;
            ATLASSERT(lpSource->lpszFileName != NULL);
            if (lpSource->lpszFileName == NULL)
            {
                return FALSE;
            }
            UINT cbSrc = static_cast<UINT>(wcslen(lpSource->lpszFileName));
            LPOLESTR szFileName = (LPOLESTR)::ATL::AtlCoTaskMemCAlloc((cbSrc + 1), sizeof(OLECHAR));
            lpDest->lpszFileName = szFileName;
            if (szFileName == NULL)
                return FALSE;

            ::CopyMemory(szFileName, lpSource->lpszFileName, (cbSrc + 1) * sizeof(OLECHAR));
            return TRUE;
        }

        // unable to create + copy other TYMEDs
        default:
            return FALSE;
        }
    }

    ATLASSERT(lpDest->tymed == lpSource->tymed);

    switch (lpSource->tymed)
    {
    case TYMED_HGLOBAL:
    {
        HGLOBAL hDest = CopyGlobalMemoryHandle(lpDest->hGlobal, lpSource->hGlobal);
        if (hDest == NULL)
            return FALSE;

        lpDest->hGlobal = hDest;
    }
    return TRUE;

    case TYMED_ISTREAM:
    {
        ATLASSERT(lpDest->pstm != NULL);
        ATLASSERT(lpSource->pstm != NULL);

        // get the size of the source stream
        STATSTG stat;
        if (lpSource->pstm->Stat(&stat, STATFLAG_NONAME) != S_OK)
        {
            // unable to get size of source stream
            return FALSE;
        }
        ATLASSERT(stat.pwcsName == NULL);

        // always seek to zero before copy
        LARGE_INTEGER zero = { 0, 0 };
        lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
        lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);

        // copy source to destination
        if (lpSource->pstm->CopyTo(lpDest->pstm, stat.cbSize,
            NULL, NULL) != NULL)
        {
            // copy from source to dest failed
            return FALSE;
        }

        // always seek to zero after copy
        lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
        lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
    }
    return TRUE;

    case TYMED_ISTORAGE:
    {
        ATLASSERT(lpDest->pstg != NULL);
        ATLASSERT(lpSource->pstg != NULL);

        // just copy source to destination
        if (lpSource->pstg->CopyTo(0, NULL, NULL, lpDest->pstg) != S_OK)
            return FALSE;
    }
    return TRUE;

    case TYMED_FILE:
    {
        ATLASSERT(lpSource->lpszFileName != NULL);
        ATLASSERT(lpDest->lpszFileName != NULL);
        CString strSource(lpSource->lpszFileName);
        CString strDest(lpDest->lpszFileName);
        return ::CopyFile(lpSource->lpszFileName ? strSource.GetString() : NULL,
            lpDest->lpszFileName ? strDest.GetString() : NULL, FALSE);
    }

    case TYMED_ENHMF:
    case TYMED_GDI:
    {
        ATLASSERT(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));

        // with TYMED_GDI cannot copy into existing HANDLE
        if (lpDest->hGlobal != NULL)
            return FALSE;

        // otherwise, use OleDuplicateData for the copy
        lpDest->hGlobal = ::OleDuplicateData(lpSource->hGlobal, cfFormat, 0);
        if (lpDest->hGlobal == NULL)
            return FALSE;
    }
    return TRUE;

    // other TYMEDs cannot be copied
    default:
        return FALSE;
    }
}
#endif
bool CDataObjectWrapper::IsDataAvailable(CLIPFORMAT cfFormat)
{
    if (cfFormat == 0)
    {
        return FALSE;
    }

    // fill in FORMATETC struct
//    FORMATETC formatEtc;
//    WzFillFormatEtc(&formatEtc, cfFormat);
    FORMATETC formatEtc = { cfFormat, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // attempt to get the data
    return m_dataObj->QueryGetData(&formatEtc) == S_OK;
}

HGLOBAL CDataObjectWrapper::GetGlobalData(CLIPFORMAT cfFormat)
{
    if (cfFormat == 0)
        return NULL;

    //    FORMATETC formatEtc;
    //    WzFillFormatEtc(&formatEtc, cfFormat);
    FORMATETC formatEtc = { cfFormat, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL | TYMED_MFPICT };
    STGMEDIUM stgMedium;
    HRESULT hr = m_dataObj->GetData(&formatEtc, &stgMedium);
    if (FAILED(hr))
        return NULL;

    // handle just hGlobal types
    switch (stgMedium.tymed)
    {
    case TYMED_MFPICT:
    case TYMED_HGLOBAL:
        if (stgMedium.pUnkForRelease == NULL)
        {
            return stgMedium.hGlobal;
        }

        STGMEDIUM stgMediumDest;
        stgMediumDest.tymed = TYMED_NULL;
        stgMediumDest.pUnkForRelease = NULL;
        if (!CopyStgMedium(&stgMediumDest, &stgMedium, cfFormat))
        {
            ::ReleaseStgMedium(&stgMedium);
            return NULL;
        }

        ::ReleaseStgMedium(&stgMedium);
        return stgMediumDest.hGlobal;

        // default -- falls through to error condition...
    }

    ::ReleaseStgMedium(&stgMedium);
    return NULL;
}

//CF_TEXT
HRESULT CDataObjectWrapper::GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->GetData(pFormatEtc, pMedium);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->GetDataHere(pFormatEtc, pMedium);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::QueryGetData(FORMATETC* pFormatEtc)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->QueryGetData(pFormatEtc);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->GetCanonicalFormatEtc(pFormatEct, pFormatEtcOut);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->SetData(pFormatEtc, pMedium, fRelease);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->EnumFormatEtc(dwDirection, ppEnumFormatEtc);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->DAdvise(pFormatEtc, advf, pAdvSink, pdwConnection);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::DUnadvise(DWORD dwConnection)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->DUnadvise(dwConnection);
    }

    return E_FAIL;
}

HRESULT CDataObjectWrapper::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise)
{
    if (m_dataObj != nullptr)
    {
        return m_dataObj->EnumDAdvise(ppEnumAdvise);
    }

    return E_FAIL;
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