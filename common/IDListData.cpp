#include "pch.h"
#include "IDListData.h"

CIDListData::CIDListData(const CIDLEx* pcidl, const TString& path)
{
    m_pData = pcidl->GetIDListData(m_dataSize);
    m_path = path;
}

CIDListData::CIDListData(const CIDLEx* pcidl)
{
    m_pData = pcidl->GetIDListData(m_dataSize);
    m_path.clear();
}

CIDListData::CIDListData(unsigned char* pData, int dataSize, const TString& path)
{
    if ((pData != nullptr) && (dataSize > 0))
    {
        m_pData = new unsigned char[dataSize];
        if (m_pData != nullptr)
        {
            m_dataSize = dataSize;
            ::CopyMemory(m_pData, pData, m_dataSize);
            m_path = path;
        }
    }
}

bool CIDListData::IsSame(const CIDListData& data)
{
    if (m_path.compare(data.m_path) == 0)
        return true;

    return false;
}

CIDListData& CIDListData::operator = (const CIDListData& data)
{
    if(this != &data)
    {
        Release();
        CopyFrom(data);
    }
    
    return *this;
}

CIDListData& CIDListData::operator = (CIDListData&& data)
{
    if (this != &data)
    {
        Release();
        MoveFrom(data);
    }

    return *this;
}

bool CIDListData::CopyFrom(const CIDListData& data)
{
    if ((data.m_pData != nullptr) && (data.m_dataSize > 0))
    {
        m_pData = new unsigned char[data.m_dataSize];
        if (m_pData != nullptr)
        {
            m_dataSize = data.m_dataSize;
            ::CopyMemory(m_pData, data.m_pData, m_dataSize);
            m_path = data.m_path;

            return true;
        }
    }

    return false;
}

void CIDListData::MoveFrom(CIDListData& data)
{
    m_pData = data.m_pData;
    m_dataSize = data.m_dataSize;
    m_path = std::move(data.m_path);

    data.m_pData = nullptr;
    data.m_dataSize = 0;
}

void CIDListData::Release()
{
    if (m_pData != nullptr)
    {
        delete[] m_pData;
        m_pData = nullptr;
    }

    m_path.clear();
    m_dataSize = 0;
}

