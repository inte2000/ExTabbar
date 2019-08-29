#pragma once

class CGuidEx final
{
public:
    CGuidEx(){}
    CGuidEx(const CGuidEx& guid) { m_guid = guid.m_guid; }
    CGuidEx(const TString& strGuid){ FromString(strGuid); }
    CGuidEx(const GUID& guid) { m_guid = guid; }
    ~CGuidEx() {}

    CGuidEx& operator = (const CGuidEx& guid);
    operator GUID() { return m_guid; }

    BOOL FromString(const TString& strGuid);
    TString ToString();
    GUID GetGuid() { return m_guid; }
protected:
    GUID m_guid;
};


