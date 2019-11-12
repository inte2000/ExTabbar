#pragma once

class CDoubleZeroBuffer
{
public:
    CDoubleZeroBuffer();
    CDoubleZeroBuffer(const CDoubleZeroBuffer& dzb) { CopyFrom(dzb); }
    CDoubleZeroBuffer(CDoubleZeroBuffer&& dzb) { MoveFrom(dzb); }
    virtual ~CDoubleZeroBuffer() { Release(); }
    
    BOOL Create(const std::vector<TString>& strArray);
    CDoubleZeroBuffer& operator = (const CDoubleZeroBuffer& db);
    CDoubleZeroBuffer& operator = (CDoubleZeroBuffer&& db);
    const TCHAR* GetRawData() const { return reinterpret_cast<TCHAR *>(m_pData); }
    TCHAR* GetRawData() { return reinterpret_cast<TCHAR*>(m_pData);}
    UINT_PTR GetBufferSize() const { return m_BufSize; }
    
protected:
    void CopyFrom(const CDoubleZeroBuffer& dzb);
    void MoveFrom(CDoubleZeroBuffer& dzb);
    void Release();
    UINT_PTR CalcBufferSize(const std::vector<TString>& strArray);
    void TransStringListToDZBuffer(const std::vector<TString>& strArray, TCHAR* pBuffer);

protected:
    unsigned char* m_pData;
    UINT_PTR m_BufSize;
};

