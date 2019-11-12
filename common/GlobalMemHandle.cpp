#include "pch.h"
#include "GlobalMemHandle.h"


bool CGlobalHandle::Alloc(UINT flags, SIZE_T size)
{
    ATLASSERT(m_hGlobal == NULL);

    m_hGlobal = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, size);
    m_bLocked = false;

    return (m_hGlobal != NULL);
}

void CGlobalHandle::Free()
{
    if (m_hGlobal != NULL)
    {
        ATLASSERT(!m_bLocked);
        if(m_bLocked)
        {
            ::GlobalUnlock(m_hGlobal);
            m_bLocked = false;
        }

        ::GlobalFree(m_hGlobal);
        m_hGlobal = NULL;
    }
}

LPVOID CGlobalHandle::Lock()
{
    if (m_hGlobal != NULL)
    {
        ATLASSERT(!m_bLocked);
        m_bLocked = true;
        return ::GlobalLock(m_hGlobal);
    }

    return nullptr;
}

void CGlobalHandle::Unlock()
{
    if (m_hGlobal != NULL)
    {
        ATLASSERT(m_bLocked);
        m_bLocked = false;
        ::GlobalUnlock(m_hGlobal);
    }
}
