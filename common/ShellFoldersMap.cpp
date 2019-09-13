#include "pch.h"
#include "SystemFunctions.h"
#include "ShellWrapper.h"
#include "ShellFoldersMap.h"

LPCTSTR lpszShlParsenameQuickAccess = _T("::{679F85CB-0220-4080-B29B-5540CC05AAB6}");
LPCTSTR lpszShlParsenameThisComputer = _T("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
LPCTSTR lpszShlParsenameFrequentFolder = _T("::{3936E9E4-D92C-4EEE-A85A-BC16D5EA0819}");//frequent folder win10

static LPCTSTR ShlParseNames[] =
{
    lpszShlParsenameQuickAccess, lpszShlParsenameThisComputer, lpszShlParsenameFrequentFolder 
};

bool CShellFoldersMap::Initialize()
{
    SFGAOF aog;
    LPITEMIDLIST pidlxx = nullptr;
    for (int i = 0; i < _countof(ShlParseNames); i++)
    {
        TString displayName = _T("shell:");
        displayName += ShlParseNames[i];
        LPITEMIDLIST pidl = nullptr;
        HRESULT hr = ::SHParseDisplayName(displayName.c_str(), NULL, &pidl, SFGAO_CANCOPY, &aog);
        if (hr == S_OK)
        {
            CIDLEx cidl(pidl, true);
            m_ShlFolders[ShlParseNames[i]] = CIDListData(&cidl, ShlParseNames[i]);
            ::CoTaskMemFree(pidl);
        }
    }

    return true;
}

bool CShellFoldersMap::InsertFolder(const TString& shlName, LPCITEMIDLIST pidl)
{
    auto it = m_ShlFolders.find(shlName);
    if (it == m_ShlFolders.end())
    {
        CIDLEx cidl(pidl, true);
        m_ShlFolders[shlName] = CIDListData(&cidl, shlName);
    }

    return true;
}

bool CShellFoldersMap::FindFolder(const TString& shlName, CIDListData& data) const
{
    auto it = m_ShlFolders.find(shlName);
    if (it != m_ShlFolders.end())
    {
        data = it->second;
        return true;
    }

    return false;
}
