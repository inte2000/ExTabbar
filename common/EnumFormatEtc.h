#pragma once


class CEnumFormatEtc : public IEnumFORMATETC
{
public:
    CEnumFormatEtc(FORMATETC* fmt, int count);
    virtual ~CEnumFormatEtc();

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IEnumFormatEtc interface
    HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC* rgelt, ULONG* pFetched);
    HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
    HRESULT STDMETHODCALLTYPE Reset(void);
    HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC** ppEnumFmtEtc);

protected:

protected:
    ULONG m_lRefCount;
    ULONG m_nIndex;           // current enumerator index
    ULONG m_nNumFormats;      // number of FORMATETC members
    FORMATETC *m_pFormatEtc;       // array of FORMATETC objects
};

HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC* pFormatEtc, IEnumFORMATETC** ppEnumFormatEtc);
//
