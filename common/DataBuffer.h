#pragma once

class CDataBuffer
{
public:
    CDataBuffer(unsigned int bufSize = 0);
    CDataBuffer(void* pData, unsigned int length, bool bCopy = true) { Attach(pData, length, bCopy); }
    CDataBuffer(const CDataBuffer& db) { CopyFrom(db); }
    CDataBuffer(CDataBuffer&& db) { MoveFrom(db); }
    virtual ~CDataBuffer() { Release(); }
    
    CDataBuffer& operator = (const CDataBuffer& db);
    CDataBuffer& operator = (CDataBuffer&& db);
    bool Attach(void* pData, unsigned int length, bool bCopy);
    void* Detach(unsigned int *pSize = nullptr);
    bool Append(void* pData, unsigned int length);
    bool InsertData(unsigned int begin, void* pData, unsigned int length);
    bool RemoveData(unsigned int begin, unsigned int length);
    const void* GetRawData() const { return m_pData; }
    void* GetRawData() { return m_pData; }
    unsigned int GetBufferSize() const { return m_BufSize; }
    unsigned int GetDataLength() const { return m_DataLength; }

protected:
    bool CopyFrom(const CDataBuffer& db);
    bool MoveFrom(CDataBuffer& db);
    void Release();
    bool AllocBuf(unsigned int size);
    bool CheckSpace(unsigned int length);

protected:
    bool m_bOwned;
    unsigned char* m_pData;
    unsigned int m_DataLength;
    unsigned int m_BufSize;
};

