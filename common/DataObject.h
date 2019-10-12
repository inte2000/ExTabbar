#pragma once


typedef struct tagDATASTORAGE
{
    FORMATETC formatEtc;
    STGMEDIUM stgMedium;
}DATASTORAGETYPE, *PDATASTORAGETYPE;


class CDataObject : public IDataObject
{
public:
    CDataObject();
    CDataObject(FORMATETC* fmt, STGMEDIUM* stgmed, int count);
    virtual ~CDataObject();

    //BOOL IsDataAvailable(CLIPFORMAT cfFormat);    
    //BOOL GetGlobalData(CLIPFORMAT cfFormat, void** ppData);    
    //BOOL GetGlobalDataArray(CLIPFORMAT cfFormat, HGLOBAL* pDataArray, DWORD dwCount);    
    //BOOL SetGlobalData(CLIPFORMAT cfFormat, void* pData, BOOL fRelease = TRUE);    
    //BOOL SetGlobalDataArray(CLIPFORMAT cfFormat, HGLOBAL* pDataArray, DWORD dwCount, BOOL fRelease = TRUE);    
    BOOL SetDropTip(DROPIMAGETYPE type, LPCTSTR pszMsg, LPCTSTR pszInsert);
    void ClearDropTip() { SetDropTip(DROPIMAGE_INVALID, L"", NULL); }

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IDataObject interface
    HRESULT STDMETHODCALLTYPE GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
    HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* pFormatEtc);
    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut);
    HRESULT STDMETHODCALLTYPE SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease);
    HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD      dwDirection, IEnumFORMATETC** ppEnumFormatEtc);
    HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection);
    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD      dwConnection);
    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** ppEnumAdvise);

protected:
    HRESULT InternalAddData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease);
    HRESULT FindFormatEtc(FORMATETC* pFormatEtc, PDATASTORAGETYPE *ppde, BOOL bAdd);
    HRESULT SetBlob(CLIPFORMAT cf, const void* pvBlob, UINT cbBlob);
    HRESULT AddRefStgMedium(STGMEDIUM* pstgmIn, STGMEDIUM* pstgmOut, BOOL fCopyIn);

protected:
    ULONG m_lRefCount;
    DATASTORAGETYPE *m_dataStorage;
    UINT   m_dsCount;
};

HRESULT CreateDataObject(FORMATETC* fmtetc, STGMEDIUM* stgmeds, UINT count, IDataObject** ppDataObject);

//
