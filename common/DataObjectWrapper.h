#pragma once

class CDataObjectWrapper
{
public:
    CDataObjectWrapper();
    CDataObjectWrapper(const CDataObjectWrapper& dow) { Attach(dow.m_dataObj, true); }
    CDataObjectWrapper(IDataObject* dataObj) { Attach(dataObj, true); }
    CDataObjectWrapper(CDataObjectWrapper&& dow);
    virtual ~CDataObjectWrapper() { Release(); }

    bool IsEmpty() const { return (m_dataObj == nullptr); }
    void Attach(IDataObject* dataObj, bool bRefAdd);
    IDataObject* Detach();
    void Release();

    CDataObjectWrapper& operator = (const CDataObjectWrapper& dow);
    CDataObjectWrapper& operator = (CDataObjectWrapper&& dow);
    operator IDataObject* () { return m_dataObj; }

    bool IsDataAvailable(CLIPFORMAT cfFormat);    
    HGLOBAL GetGlobalData(CLIPFORMAT cfFormat);

    //BOOL GetGlobalData(CLIPFORMAT cfFormat, void** ppData);    
    //BOOL GetGlobalDataArray(CLIPFORMAT cfFormat, HGLOBAL* pDataArray, DWORD dwCount);    
    //BOOL SetGlobalData(CLIPFORMAT cfFormat, void* pData, BOOL fRelease = TRUE);    
    //BOOL SetGlobalDataArray(CLIPFORMAT cfFormat, HGLOBAL* pDataArray, DWORD dwCount, BOOL fRelease = TRUE);    

    // IDataObject interface wrapper
    HRESULT GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
    HRESULT GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
    HRESULT QueryGetData(FORMATETC* pFormatEtc);
    HRESULT GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut);
    HRESULT SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease);
    HRESULT EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc);
    HRESULT DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection);
    HRESULT DUnadvise(DWORD dwConnection);
    HRESULT EnumDAdvise(IEnumSTATDATA** ppEnumAdvise);

protected:
    IDataObject *m_dataObj;
#ifdef DEBUG
    bool m_bAddRef;
#endif
};


//
