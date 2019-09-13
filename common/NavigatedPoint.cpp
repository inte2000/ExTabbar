#include "pch.h"
#include "framework.h"
//#include "shlobj.h"
#include "SystemFunctions.h"
#include "NavigatedPoint.h"

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

CNavigatedPoint::CNavigatedPoint(const CIDListData& IdlData, const CIDLEx& cidl, int hash, bool autoNav)
{
    m_curItem = IdlData;

    m_strTitle = GetTabItemText(cidl, IdlData.GetPath());
    if (IsNamespacePath(IdlData.GetPath()))
        m_strTooltip = GetTabItemTooltip(cidl, true);
    else
        m_strTooltip = IdlData.GetPath();

    m_Hash = hash;
    m_autoNav = autoNav;
}

bool CNavigatedPoint::IsSameNPoint(const CNavigatedPoint& np)
{
    if (m_curItem.IsSame(np.m_curItem) == 0)
        return true;

    return false;
}
