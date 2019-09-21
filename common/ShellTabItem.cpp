#include "pch.h"
#include "framework.h"
#include "shlobj.h"
#include "GuidEx.h"
#include "SystemFunctions.h"
#include "ShellTabItem.h"

TString GetTabItemText(const CIDLEx& cidl, const TString& path)
{
    TString text;

    if (IsNamespacePath(path) || IsDiskRootPath(path))
    {
        return std::move(cidl.GetDisplayName());
    }

    std::size_t rSlash = path.rfind(_T('\\'));
    if (rSlash == TString::npos)
    {
        return path;
    }

    std::size_t pos = path.find(_T(':'), 0);
    if (pos > 0)
    {
        text = path.substr(0, pos);
        text += _T("...");
        text += path.substr(rSlash + 1);
    }
    else
    {
        text = path.substr(rSlash + 1);
    }

    return std::move(text);
}

TString GetTabItemTooltip(const CIDLEx& cidl, bool bSlowMethod)
{
    TString text;

    CComPtr<IShellFolder> pSF;
    LPCITEMIDLIST pidl = nullptr;
    HRESULT hr = ::SHBindToParent(cidl, IID_IShellFolder, (void**)& pSF, &pidl);
    if (hr == S_OK)
    {
        CComPtr<IQueryInfo> pQI;
        LPCITEMIDLIST apidl = pidl;
        UINT rgfReserved = 0;
        hr = pSF->GetUIObjectOf(NULL, 1, &apidl, IID_IQueryInfo, &rgfReserved, (void**)& pQI);
        if (hr == S_OK)
        {
            WCHAR* pBuf = nullptr;
            hr = pQI->GetInfoTip(bSlowMethod ? QITIPF_USESLOWTIP : QITIPF_DEFAULT, &pBuf);
            if (hr == S_OK)
            {
                text = pBuf;
            }
        }
    }

    return std::move(text);
}

CShellTabItem::CShellTabItem()
{
}

void CShellTabItem::Release()
{
    m_TravelLogs.clear();
    ReleaseStatus();
    //m_cIdl.Release();
}

bool CShellTabItem::NavigatedTo(const CIDListData& IdlData, const CIDLEx& cidl, const TString& url)
{
    m_curIdlData = IdlData;
    m_strUrl = url;

    m_strTitle = GetTabItemText(cidl, IdlData.GetPath());
    if (IsNamespacePath(IdlData.GetPath()))
        m_strTooltip = GetTabItemTooltip(cidl, true);
    else
        m_strTooltip = IdlData.GetPath();

    return true;
}

void CShellTabItem::SetCurrentStatus(const TString& focusPath, std::vector<CIDListData *>& items)
{
    ReleaseStatus();
    m_focusPath = std::move(focusPath);
    m_SelectedItems = std::move(items);
}

const std::vector<CIDListData*>& CShellTabItem::GetCurrentStatus(TString& focusPath) const
{
    focusPath = m_focusPath;

    return m_SelectedItems;
}

void CShellTabItem::ClearCurrentStatus()
{
    ReleaseStatus();
}

const std::vector<CNavigatedPoint>& CShellTabItem::GetTravelLogs(CNavigatedPoint& curItem) const
{
    curItem.strTitle = m_strTitle;
    curItem.strUrl = m_strUrl;

    return m_TravelLogs;
}

void CShellTabItem::SetTravelLogs(std::vector<CNavigatedPoint>& logs)
{
    m_TravelLogs.clear();

    m_TravelLogs = std::move(logs);
}

CIDLEx CShellTabItem::GetCurrentCIdl() const
{
    CIDLEx tmpIdl;
    
    auto idlData = m_curIdlData.GetIDLData();
    tmpIdl.CreateByIdListData(std::get<0>(idlData), std::get<1>(idlData));

    return std::move(tmpIdl);
}

void CShellTabItem::ReleaseStatus()
{
    m_focusPath.clear();
    for (auto& x : m_SelectedItems)
    {
        delete x;
    }

    m_SelectedItems.clear();
}
