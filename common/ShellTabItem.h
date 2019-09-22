#pragma once

#include <deque>
#include <vector>
#include <memory>
#include "ShellWrapper.h"
#include "IDListData.h"
#include "NavigatedPoint.h"

class CShellTabItem 
{
public:
    CShellTabItem();
    virtual ~CShellTabItem() { Release(); }

    void Release();

    bool NavigatedTo(const CIDListData& IdlData, const CIDLEx& cidl, const TString& url);
    void SetCurrentStatus(const TString& focusPath, std::vector<CIDListData *>& items);
    const std::vector<CIDListData*>& GetCurrentStatus(TString& focusPath) const;
    void ClearCurrentStatus();
    const std::vector<CNavigatedPoint>& GetTravelLogs(CNavigatedPoint& curItem) const;
    void SetTravelLogs(std::vector<CNavigatedPoint>& logs);

    std::tuple<const unsigned char*, int> GetCurrentIDLData() { return m_curIdlData.GetIDLData(); }
    const TString& GetUrl() const { return m_strUrl; }
    const TString& GetTitle() const { return m_strTitle; }
    const TString& GetTooltip() const { return m_strTooltip; }
    CIDLEx GetCurrentCIdl() const;
protected:
    void ReleaseStatus();

protected:
    TString m_strUrl;
    TString m_strTitle;
    TString m_strTooltip; //for normal folder, it's full path, m_strUrl, for shell space name, it's displayname, m_strTitle

    CIDListData m_curIdlData;

    //keep selected item and focus item while switch tab
    TString m_focusPath;
    std::vector<CIDListData*> m_SelectedItems;
    //keep travel log while switch tab
    std::vector<CNavigatedPoint> m_TravelLogs;
};
