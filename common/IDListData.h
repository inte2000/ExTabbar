#pragma once

#include <tuple> 
#include "ShellWrapper.h"

class CIDListData final
{
public:
    CIDListData() { m_pData = nullptr; m_dataSize = 0; }
    CIDListData(const CIDListData& data) { CopyFrom(data); }
    CIDListData(CIDListData&& data) { MoveFrom(data); }
    CIDListData(const CIDLEx *pcidl, const TString& path);
    CIDListData(const CIDLEx *pcidl);
    CIDListData(const TString& path) : CIDListData() { m_path = path; }
    CIDListData(unsigned char* pData, int dataSize, const TString& path);
    ~CIDListData() { Release(); }

    bool IsEmpty() const { return (m_pData == nullptr); }
    bool GetCopyOf(const CIDLEx* pcidl);
    bool GetCopyOf(const CIDLEx* pcidl, const TString& path);
    bool IsSame(const CIDListData& data);
    const TString& GetPath() const { return m_path; }
    std::tuple<const unsigned char*, int> GetIDLData() const { return { m_pData, m_dataSize}; }
    CIDListData& operator = (const CIDListData& data);
    CIDListData& operator = (CIDListData&& data);

protected:
    bool CopyFrom(const CIDListData& data);
    void MoveFrom(CIDListData& data);
    void Release();
protected:
    unsigned char *m_pData;
    int m_dataSize;
    TString m_path;
};


