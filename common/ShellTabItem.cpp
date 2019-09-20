#include "pch.h"
#include "framework.h"
#include "shlobj.h"
#include "GuidEx.h"
#include "SystemFunctions.h"
#include "ShellTabItem.h"


CShellTabItem::CShellTabItem()
{
}

void CShellTabItem::Release()
{
    m_hisBackward.clear();
    m_hisForward.clear();
    m_hisBranches.clear();
    ReleaseStatus();
    //m_cIdl.Release();
}

void CShellTabItem::GetHistoryBack(std::vector<TString>& historys)
{
    for (auto it = m_hisBackward.begin(); it != m_hisBackward.end(); ++it)
        historys.push_back((*it)->GetCurrent().GetPath());
}

void CShellTabItem::GetHistoryForward(std::vector<TString>& historys)
{
    for (auto it = m_hisForward.begin(); it != m_hisForward.end(); ++it)
        historys.push_back((*it)->GetCurrent().GetPath());
}

bool CShellTabItem::GoBackward(std::shared_ptr<CNavigatedPoint>& np)
{
    if (m_hisBackward.size() > 1)
    {
        m_hisForward.push_front(m_hisBackward.front());
        m_hisBackward.pop_front();
        np = m_hisBackward.front();
        m_CurNp = np;
        //m_cIdl.Attach(np->GetCIdl(), false);
        //m_strPath = np->GetPath();
        return true;
    }

    np = m_CurNp;
    return false;
}

bool CShellTabItem::GoForward(std::shared_ptr<CNavigatedPoint>& np)
{
    if (m_hisForward.size() > 1)
    {
        m_hisBackward.push_front(m_hisForward.front());
        m_hisForward.pop_front();
        np = m_hisBackward.front();
        m_CurNp = np;
        //m_cIdl.Attach(np->GetCIdl(), false);
        //m_strPath = np->GetPath();
        return true;
    }

    np = m_CurNp;
    return false;
}

bool CShellTabItem::NavigatedTo(const CIDListData& IdlData, const CIDLEx& cidl, int hash, bool autoNav)
{
    m_CurNp = ::std::make_shared<CNavigatedPoint>(CNavigatedPoint(IdlData, cidl, hash, autoNav));
    if (m_CurNp == nullptr)
        return false;

    if (autoNav && m_hisBackward.size() > 0 && m_hisBackward.front()->IsAutoVav()) 
    {
        m_hisBackward.pop_front();
    }
    m_hisBackward.push_front(m_CurNp);
    
    for (auto& x : m_hisForward)
    {
        if (std::find_if(m_hisBranches.begin(), m_hisBranches.end(),
            [x](auto& l)->bool {return x->IsSameNPoint(*l); }) == m_hisBranches.end())
        {
            m_hisBranches.push_back(x);
        }
    }
    m_hisForward.clear();
    for (auto& x : m_hisBackward)
    {
        auto it = std::find_if(m_hisBranches.begin(), m_hisBranches.end(),
            [x](auto& l)->bool {return x->IsSameNPoint(*l); });
        if (it != m_hisBranches.end())
        {
            m_hisBranches.erase(it);
        }
    }

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

CIDLEx CShellTabItem::GetCurrentCIdl() const
{
    CIDLEx tmpIdl;
    
    if (m_CurNp != nullptr)
    {
        auto idlData = m_CurNp->GetCurrent().GetIDLData();
        tmpIdl.CreateByIdListData(std::get<0>(idlData), std::get<1>(idlData));
    }
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
