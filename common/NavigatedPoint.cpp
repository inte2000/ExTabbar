#include "pch.h"
#include "framework.h"
//#include "shlobj.h"
#include "SystemFunctions.h"
#include "NavigatedPoint.h"

TString GetTabItemText(CIDLEx& cidl, const TString& path)
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

TString GetTabItemTooltip(CIDLEx& cidl, bool bSlowMethod)
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

CNavigatedPoint::CNavigatedPoint(const TString& path, const CIDLEx& cidl, int hash, bool autoNav)
{
    m_strPath = path;

    if (cidl.IsEmpty())
        m_cidl = CIDLEx::CIDLFromFullPath(path);
    else
        m_cidl = cidl;

    m_strTitle = GetTabItemText(m_cidl, path);
    if (IsNamespacePath(path))
        m_strTooltip = GetTabItemTooltip(m_cidl, true);
    else
        m_strTooltip = path;

    m_Hash = hash;
    m_autoNav = autoNav;
}

bool CNavigatedPoint::IsSameNPoint(const CNavigatedPoint& np)
{
    if (m_strPath.compare(np.m_strPath) == 0)
        return true;

    return false;
}
