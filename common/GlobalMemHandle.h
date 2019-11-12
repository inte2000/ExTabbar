#pragma once

class CGlobalHandle
{
public:
    CGlobalHandle(HANDLE hGlobal = NULL) { Attach(hGlobal); }
    CGlobalHandle(UINT flags, SIZE_T size) { Alloc(flags, size); }
    virtual ~CGlobalHandle() { Free(); }
    
    operator HANDLE() { return m_hGlobal; }

    void Attach(HANDLE hGlobal) { m_hGlobal = hGlobal; m_bLocked = false; }
    HANDLE Detach() { HANDLE tmp = m_hGlobal; m_hGlobal = NULL; return tmp; }
    bool Alloc(UINT flags, SIZE_T size);
    void Free();
    LPVOID Lock();
    void Unlock();
    bool IsLock() const { return m_bLocked; }
    bool IsValid() const { return (m_hGlobal != NULL); }

protected:
    HANDLE m_hGlobal;
    bool m_bLocked;
};

