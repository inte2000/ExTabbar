#include "pch.h"
#include "DataBuffer.h"


CDataBuffer::CDataBuffer(unsigned int bufSize)
{
    if (bufSize > 0)
    {
        AllocBuf(bufSize);
        m_bOwned = true;
    }
    else
    {
        m_pData = nullptr;
        m_DataLength = 0;
        m_BufSize = 0;
        m_bOwned = false;
    }
}

CDataBuffer& CDataBuffer::operator = (const CDataBuffer& db)
{
    if (this != &db)
    {
        Release();
        CopyFrom(db);
    }

    return *this;
}

CDataBuffer& CDataBuffer::operator = (CDataBuffer&& db)
{
    if (this != &db)
    {
        Release();
        MoveFrom(db);
    }

    return *this;
}

bool CDataBuffer::Attach(void* pData, unsigned int length, bool bCopy)
{
    if (m_pData != nullptr)
        return false;

    if (bCopy)
    {
        if (!AllocBuf(length))
            return false;
        memcpy(m_pData, pData, length);
        m_DataLength = length;
        m_bOwned = true;
    }
    else
    {
        m_pData = (unsigned char *)pData;
        m_BufSize = length;
        m_DataLength = length;
        m_bOwned = false;
    }

    return true;
}

void* CDataBuffer::Detach(unsigned int* pSize)
{
    if (pSize != nullptr)
        *pSize = m_DataLength;

    void* pTmp = m_pData;
    m_pData = nullptr;
    m_DataLength = 0;
    m_BufSize = 0;
    m_bOwned = false;

    return pTmp;
}

bool CDataBuffer::Append(void* pData, unsigned int length)
{
    if (!CheckSpace(length))
        return false;

    memcpy(m_pData + m_DataLength, pData, length);
    m_DataLength += length;
    return true;
}

bool CDataBuffer::InsertData(unsigned int begin, void* pData, unsigned int length)
{
    if (!CheckSpace(length))
        return false;

    unsigned char* pBegin = m_pData + begin;
    memmove(pBegin + length, pBegin, length);
    memcpy(pBegin, pData, length);
    m_DataLength += length;

    return true;
}

bool CDataBuffer::RemoveData(unsigned int begin, unsigned int length)
{
    if ((m_pData == nullptr) || (begin > m_DataLength))
        return false;

    if (begin == 0)
        return true;

    if (begin == m_DataLength)
    {
        m_DataLength = 0;
        return true;
    }
    if ((begin + length) >= m_DataLength)
    {
        m_DataLength = begin;
        return true;
    }

    unsigned int moveLength = m_DataLength - begin - length;
    unsigned char* pBegin = m_pData + begin;
    unsigned char* pStartPos = pBegin + length;
    memmove(pBegin, pStartPos, length);
    m_DataLength -= length;

    return true;
}

bool CDataBuffer::CopyFrom(const CDataBuffer& db)
{
    if (!AllocBuf(db.m_BufSize))
        return false;

    memcpy(m_pData, db.m_pData, db.m_DataLength);
    m_DataLength = db.m_DataLength;
    m_bOwned = true;
    return true;
}

bool CDataBuffer::MoveFrom(CDataBuffer& db)
{
    m_pData = db.m_pData;
    m_DataLength = db.m_DataLength;
    m_BufSize = db.m_BufSize;
    m_bOwned = true;

    db.m_pData = nullptr;
    db.m_DataLength = 0;
    db.m_BufSize = 0;
    db.m_bOwned = false;

    return true;
}

void CDataBuffer::Release()
{
    if (m_bOwned)
    {
        if (m_pData != nullptr)
            delete[] m_pData;

        m_bOwned = false;
    }

    m_pData = nullptr;
    m_DataLength = 0;
    m_BufSize = 0;
}

inline static unsigned int RoundSize(unsigned int size, unsigned int round)
{
    if (round == 0)
        round = 32;

    return (size / round + 1) * round;
}

bool CDataBuffer::AllocBuf(unsigned int bufSize)
{
    unsigned int roundSize = RoundSize(bufSize, 32);
    m_pData = new unsigned char[roundSize];
    if (m_pData == nullptr)
        return false;

    memset(m_pData, 0, roundSize);
    m_DataLength = 0;
    m_BufSize = roundSize;

    return true;
}

bool CDataBuffer::CheckSpace(unsigned int length)
{
    if (m_pData == nullptr)
        return false;

    unsigned int space = m_BufSize - m_DataLength;
    if (space < length)
        return false;

    return true;
}

