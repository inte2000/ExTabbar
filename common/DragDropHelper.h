#pragma once

#include "DataBuffer.h"
#include "DataObject.h"

inline CLIPFORMAT RegisterFormat(LPCTSTR lpszFormat)
{
    return static_cast<CLIPFORMAT>(::RegisterClipboardFormat(lpszFormat));
}

CLIPFORMAT GetClipboardFormat(CLIPFORMAT* pcf, LPCTSTR lpszFormat);
bool IsRegisteredFormat(CLIPFORMAT cfFormat, LPCTSTR lpszFormat);
HRESULT SetBlob(IDataObject* pDataObj, CLIPFORMAT cf, const void* pvBlob, UINT cbBlob);
BOOL SetDropTip(IDataObject* pDataObj, DROPIMAGETYPE type, LPCTSTR lpszMsg, LPCTSTR lpszInsert);

inline void ClearDropTip(IDataObject* pDataObj)
{ 
    SetDropTip(pDataObj, DROPIMAGE_INVALID, L"", NULL); 
}

bool GetGlobalData(IDataObject* pDataObj, LPCTSTR lpszFormat, FORMATETC& FormatEtc, STGMEDIUM& StgMedium);
DWORD GetGlobalDataDWord(IDataObject* pDataObj, LPCTSTR lpszFormat);
bool SetGlobalDataDWord(IDataObject* pDataObj, LPCTSTR lpszFormat, DWORD dwValue);
bool SetGlobalData(IDataObject* pDataObj, LPCTSTR lpszFormat, HGLOBAL hGlobal);
bool SetDropDescription(IDataObject* pDataObj, DROPDESCRIPTION* pDescription);

DROPIMAGETYPE DropEffectToDropImage(DWORD dwEffect);
