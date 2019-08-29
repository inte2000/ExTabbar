#include "pch.h"
#include "GuidEx.h"


CGuidEx& CGuidEx::operator = (const CGuidEx& guid) 
{
    if(this != &guid)
    {
        this->m_guid = guid.m_guid;
    }
    
    return *this;
}

BOOL CGuidEx::FromString(const TString& strGuid)
{
    if (::CLSIDFromString(strGuid.c_str(), &m_guid) == S_OK)
        return TRUE;
    
    return FALSE;
}

TString CGuidEx::ToString()
{
    WCHAR szGuid[128] = { 0 };
    ::StringFromGUID2(m_guid, szGuid, 128);

    return TString(szGuid);
}
