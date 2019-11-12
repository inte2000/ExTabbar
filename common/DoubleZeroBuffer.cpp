#include "pch.h"
#include "DoubleZeroBuffer.h"



CDoubleZeroBuffer::CDoubleZeroBuffer()
{
}

BOOL CDoubleZeroBuffer::Create(const std::vector<TString>& strArray)
{
    Release();

    UINT_PTR bufSize = CalcBufferSize(strArray);
    m_pData = new unsigned char[bufSize];
    if (m_pData == nullptr)
        return FALSE;
    
    m_BufSize = bufSize;
    TransStringListToDZBuffer(strArray, (TCHAR*)m_pData);

    return TRUE;
}

CDoubleZeroBuffer& CDoubleZeroBuffer::operator = (const CDoubleZeroBuffer& dzb)
{
    if (this != &dzb)
    {
        Release();
        CopyFrom(dzb);
    }

    return *this;
}

CDoubleZeroBuffer& CDoubleZeroBuffer::operator = (CDoubleZeroBuffer&& dzb)
{
    if (this != &dzb)
    {
        Release();
        MoveFrom(dzb);
    }

    return *this;
}

void CDoubleZeroBuffer::CopyFrom(const CDoubleZeroBuffer& dzb)
{
    if (dzb.m_BufSize == 0)
        return;

    m_pData = new unsigned char[dzb.m_BufSize];
    if (m_pData != nullptr)
    {
        memcpy(m_pData, dzb.m_pData, dzb.m_BufSize);
        m_BufSize = dzb.m_BufSize;
    }
}

void CDoubleZeroBuffer::MoveFrom(CDoubleZeroBuffer& dzb)
{
    m_pData = dzb.m_pData;
    m_BufSize = dzb.m_BufSize;

    dzb.m_pData = nullptr;
    dzb.m_BufSize = 0;
}

void CDoubleZeroBuffer::Release()
{
    if (m_pData != nullptr)
        delete[] m_pData;

    m_pData = nullptr;
    m_BufSize = 0;
}

UINT_PTR CDoubleZeroBuffer::CalcBufferSize(const std::vector<TString>& strArray)
{
    std::size_t bufSize = 0;
    for (auto& x : strArray)
    {
        bufSize += (x.length() + 1)* sizeof(TCHAR);
    }

    bufSize += sizeof(TCHAR); //extra '\0'

    return bufSize;
}
//[0] = L"ÐÂ½¨ Microsoft Word 97 - 2003 ÎÄµµ.doc"   34
void CDoubleZeroBuffer::TransStringListToDZBuffer(const std::vector<TString>& strArray, TCHAR* pBuffer)
{
    TCHAR *pCurPos = pBuffer;
    UINT_PTR sizeRemain = m_BufSize;

    for (auto& x : strArray)
    {
        UINT_PTR chCount = x.length() + 1;
        if (sizeRemain < (chCount * sizeof(TCHAR)))
            break;

        const TCHAR* ptr = x.c_str();
        _tcscpy_s(pCurPos, sizeRemain / sizeof(TCHAR), x.c_str());
        pCurPos += chCount;
        sizeRemain -= chCount * sizeof(TCHAR);
    }

    *pCurPos = _T('\0');
}
