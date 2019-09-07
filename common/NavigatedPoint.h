#pragma once

#include "ShellWrapper.h"

class CNavigatedPoint final
{
public:
    CNavigatedPoint (const TString& path, const CIDLEx& cidl, int hash, bool autoNav);
    ~CNavigatedPoint () { m_cidl.Release(); }
    
    bool IsSameNPoint(const CNavigatedPoint& np);
    const TString& GetPath() const { return m_strPath; }
    const TString& GetTitle() const { return m_strTitle; }
    const TString& GetTooltip() const { return m_strTooltip; }
    CIDLEx& GetCIdl() { return m_cidl; }
    bool IsAutoVav() const { return m_autoNav; }
protected:
    TString m_strPath;
    CIDLEx m_cidl;
    TString m_strTitle;
    TString m_strTooltip;
    int m_Hash;
    bool m_autoNav;

};
