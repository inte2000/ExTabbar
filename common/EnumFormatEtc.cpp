#include "pch.h"
#include <strsafe.h>
#include "SystemFunctions.h"
#include "EnumFormatEtc.h"

static void DeepCopyFormatEtc(FORMATETC* dest, FORMATETC* source)
{
    // copy the source FORMATETC into dest
    *dest = *source;

    if (source->ptd)
    {
        // allocate memory for the DVTARGETDEVICE if necessary
        dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

        // copy the contents of the source DVTARGETDEVICE into dest->ptd
        *(dest->ptd) = *(source->ptd);
    }
}

CEnumFormatEtc::CEnumFormatEtc(FORMATETC* fmt, int count)
{
    m_lRefCount = 1;
    m_nIndex = 0;
    m_nNumFormats = count;
    m_pFormatEtc = new FORMATETC[count];

    // copy the FORMATETC structures
    for (int i = 0; i < count; i++)
    {
        DeepCopyFormatEtc(&m_pFormatEtc[i], &fmt[i]);
    }
}

CEnumFormatEtc::~CEnumFormatEtc()
{
    if (m_pFormatEtc)
    {
        for (ULONG i = 0; i < m_nNumFormats; i++)
        {
            if (m_pFormatEtc[i].ptd)
                CoTaskMemFree(m_pFormatEtc[i].ptd);
        }

        delete[] m_pFormatEtc;
    }
}

// IUnknown interface
HRESULT STDMETHODCALLTYPE CEnumFormatEtc::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IEnumFORMATETC || iid == IID_IUnknown)
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

ULONG STDMETHODCALLTYPE CEnumFormatEtc::AddRef(void)
{
    InterlockedIncrement(&m_lRefCount);
    return m_lRefCount;
}

ULONG STDMETHODCALLTYPE CEnumFormatEtc::Release(void)
{
    ULONG ulRefCount = InterlockedDecrement(&m_lRefCount);
    if (ulRefCount == 0)
    {
        delete this;
    }

    return ulRefCount;
}

// IEnumFormatEtc interface
HRESULT STDMETHODCALLTYPE CEnumFormatEtc::Next(ULONG celt, FORMATETC* rgelt, ULONG* pFetched)
{
    ULONG copied = 0;

    // validate arguments
    if (celt == 0 || rgelt == 0)
        return E_INVALIDARG;

    // copy FORMATETC structures into caller's buffer
    while (m_nIndex < m_nNumFormats && copied < celt)
    {
        DeepCopyFormatEtc(&rgelt[copied], &m_pFormatEtc[m_nIndex]);
        copied++;
        m_nIndex++;
    }

    // store result
    if (pFetched != 0)
        * pFetched = copied;

    // did we copy all that was requested?
    return (copied == celt) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEnumFormatEtc::Skip(ULONG celt)
{
    m_nIndex += celt;
    return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEnumFormatEtc::Reset(void)
{
    m_nIndex = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CEnumFormatEtc::Clone(IEnumFORMATETC** ppEnumFmtEtc)
{
    HRESULT hResult;

    // make a duplicate enumerator
    hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFmtEtc);

    if (hResult == S_OK)
    {
        // manually set the index state
        ((CEnumFormatEtc*)*ppEnumFmtEtc)->m_nIndex = m_nIndex;
    }

    return hResult;
}

//  "Drop-in" replacement for SHCreateStdEnumFmtEtc. Called by CDataObject::EnumFormatEtc
HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC* pFormatEtc, IEnumFORMATETC** ppEnumFormatEtc)
{
    if (nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0)
        return E_INVALIDARG;

    *ppEnumFormatEtc = new CEnumFormatEtc(pFormatEtc, nNumFormats);

    return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}