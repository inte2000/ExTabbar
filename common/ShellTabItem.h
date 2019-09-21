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
    TString m_strTooltip; //对于普通目录是full path，即m_strUrl，对于shell space name是displayname，即title

    CIDListData m_curIdlData;

    //切换时保存选中的item和焦点item
    TString m_focusPath;
    std::vector<CIDListData*> m_SelectedItems;
    //切换时保存浏览日志
    std::vector<CNavigatedPoint> m_TravelLogs;
};
