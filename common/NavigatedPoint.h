#pragma once

#include "ShellWrapper.h"
#include "IDListData.h"

class CNavigatedPoint final
{
public:
    CNavigatedPoint (const CIDListData& IdlData, const CIDLEx& cidl, int hash, bool autoNav);
    ~CNavigatedPoint () { }
    
    bool IsSameNPoint(const CNavigatedPoint& np);
    const CIDListData& GetCurrent() const { return m_curItem; }
    const TString& GetTitle() const { return m_strTitle; }
    const TString& GetTooltip() const { return m_strTooltip; }
    bool IsAutoVav() const { return m_autoNav; }
protected:
    CIDListData m_curItem;
    TString m_strTitle;
    TString m_strTooltip;
    int m_Hash;
    bool m_autoNav;

};
