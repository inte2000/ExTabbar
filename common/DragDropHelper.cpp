#include "pch.h"
#include "SystemFunctions.h"
#include "DragDropHelper.h"

CLIPFORMAT GetClipboardFormat(CLIPFORMAT* pcf, LPCTSTR lpszFormat)
{
    if (*pcf == 0)
    {
        *pcf = RegisterFormat(lpszFormat);
    }
    return *pcf;
}

bool IsRegisteredFormat(CLIPFORMAT cfFormat, LPCTSTR lpszFormat)
{
    bool bRet = false;
    if (cfFormat >= 0xC000)
    {
        TCHAR lpszName[128];
        if (::GetClipboardFormatName(cfFormat, lpszName, sizeof(lpszName) / sizeof(lpszName[0])))
            bRet = (0 == _tcsicmp(lpszFormat, lpszName));
    }
    
    return bRet;
}

HRESULT SetBlob(IDataObject *pDataObj, CLIPFORMAT cf, const void* pvBlob, UINT cbBlob)
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
        hr = pDataObj->SetData(&fmte, &medium, TRUE);
        if (FAILED(hr))
        {
            ::GlobalFree(pv);
        }
    }
    return hr;
}

BOOL SetDropTip(IDataObject* pDataObj, DROPIMAGETYPE type, LPCTSTR lpszMsg, LPCTSTR lpszInsert)
{
    DROPDESCRIPTION dd = { type };

    WStrFromTString(dd.szMessage, ARRAYSIZE(dd.szMessage), lpszMsg);
    WStrFromTString(dd.szInsert, ARRAYSIZE(dd.szInsert), lpszInsert ? lpszInsert : L"");

    static CLIPFORMAT s_cfDropDescription = 0;
    return SetBlob(pDataObj, GetClipboardFormat(&s_cfDropDescription, CFSTR_DROPDESCRIPTION), &dd, sizeof(dd));
}

// StgMedium should be released when this function succeeds.
bool GetGlobalData(IDataObject* pDataObj, LPCTSTR lpszFormat, FORMATETC& FormatEtc, STGMEDIUM& StgMedium)
{
    bool bRet = false;

    FormatEtc.cfFormat = RegisterFormat(lpszFormat);
    FormatEtc.ptd = NULL;
    FormatEtc.dwAspect = DVASPECT_CONTENT;
    FormatEtc.lindex = -1;
    FormatEtc.tymed = TYMED_HGLOBAL;
    if (SUCCEEDED(pDataObj->QueryGetData(&FormatEtc)))
    {
        if (SUCCEEDED(pDataObj->GetData(&FormatEtc, &StgMedium)))
        {
            bRet = (TYMED_HGLOBAL == StgMedium.tymed);
            if (!bRet)
                ::ReleaseStgMedium(&StgMedium);
        }
    }
    return bRet;
}

DWORD GetGlobalDataDWord(IDataObject* pDataObj, LPCTSTR lpszFormat)
{
    DWORD dwData = 0;
    FORMATETC FormatEtc;
    STGMEDIUM StgMedium;
    if (GetGlobalData(pDataObj, lpszFormat, FormatEtc, StgMedium))
    {
        if (::GlobalSize(StgMedium.hGlobal) >= sizeof(DWORD))
        {
            dwData = *(static_cast<LPDWORD>(::GlobalLock(StgMedium.hGlobal)));
            ::GlobalUnlock(StgMedium.hGlobal);
        }
        ::ReleaseStgMedium(&StgMedium);
    }
    return dwData;
}

bool SetGlobalDataDWord(IDataObject* pDataObj, LPCTSTR lpszFormat, DWORD dwValue)
{
    bool bSuccess = false;
    FORMATETC FormatEtc;
    STGMEDIUM StgMedium;

    // object exists already
    if (GetGlobalData(pDataObj, lpszFormat, FormatEtc, StgMedium))
    {
        LPDWORD pData = static_cast<LPDWORD>(::GlobalLock(StgMedium.hGlobal));
        if (*pData != dwValue)
        {
            *pData = dwValue;
            ::GlobalUnlock(StgMedium.hGlobal);
            bSuccess = SUCCEEDED(pDataObj->SetData(&FormatEtc, &StgMedium, TRUE));
            if(!bSuccess)
                ::ReleaseStgMedium(&StgMedium);
        }
        else
        {
            bSuccess = true;
            ::GlobalUnlock(StgMedium.hGlobal);
            ::ReleaseStgMedium(&StgMedium);
        }
    }
    else
    {
        StgMedium.hGlobal = AllocDwordGlobalHandle(dwValue);
        if (StgMedium.hGlobal)
        {
            StgMedium.tymed = TYMED_HGLOBAL;
            StgMedium.pUnkForRelease = NULL;
            bSuccess = SUCCEEDED(pDataObj->SetData(&FormatEtc, &StgMedium, TRUE));
            if (!bSuccess)
                ::GlobalFree(StgMedium.hGlobal);
        }
    }

    return bSuccess;
}

bool SetGlobalData(IDataObject* pDataObj, LPCTSTR lpszFormat, HGLOBAL hGlobal)
{
    FORMATETC FormatEtc;
    STGMEDIUM StgMedium;

    bool bSuccess = false;
    if (GetGlobalData(pDataObj, lpszFormat, FormatEtc, StgMedium))
    {
        if (StgMedium.hGlobal)
            ::GlobalFree(StgMedium.hGlobal);

        StgMedium.hGlobal = hGlobal;
        bSuccess = SUCCEEDED(pDataObj->SetData(&FormatEtc, &StgMedium, TRUE));
        if (!bSuccess)
            ::ReleaseStgMedium(&StgMedium);
    }
    else
    {
        FormatEtc.cfFormat = RegisterFormat(lpszFormat);

        StgMedium.hGlobal = hGlobal;
        StgMedium.tymed = TYMED_HGLOBAL;
        StgMedium.pUnkForRelease = NULL;
        bSuccess = SUCCEEDED(pDataObj->SetData(&FormatEtc, &StgMedium, TRUE));
    }

    return bSuccess;
}

bool SetDropDescription(IDataObject* pDataObj, DROPDESCRIPTION* pDescription)
{
    HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPDESCRIPTION));
    if (hGlobal)
    {
        DROPDESCRIPTION* pDropDescription = static_cast<DROPDESCRIPTION*>(::GlobalLock(hGlobal));
        ::CopyMemory(pDropDescription, pDescription, sizeof(DROPDESCRIPTION));
        ::GlobalUnlock(hGlobal);
        
        if (SetGlobalData(pDataObj, CFSTR_DROPDESCRIPTION, hGlobal))
        {
            return true;
        }
        ::GlobalFree(hGlobal);
    }

    return false;
}

// Get corresponding drop description image type from drop effect. 
// With valid code, nType = static_cast<DROPIMAGETYPE>(dwEffect & ~DROPEFFECT_SCROLL)
//  can be used. But drop handlers may return effects with multiple bits set.
DROPIMAGETYPE DropEffectToDropImage(DWORD dwEffect)
{
    DROPIMAGETYPE nImageType = DROPIMAGE_INVALID;
    dwEffect &= ~DROPEFFECT_SCROLL;
    if (DROPEFFECT_NONE == dwEffect)
        nImageType = DROPIMAGE_NONE;
    else if (dwEffect & DROPEFFECT_MOVE)
        nImageType = DROPIMAGE_MOVE;
    else if (dwEffect & DROPEFFECT_COPY)
        nImageType = DROPIMAGE_COPY;
    else if (dwEffect & DROPEFFECT_LINK)
        nImageType = DROPIMAGE_LINK;
    return nImageType;
}

