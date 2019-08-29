#include "pch.h"
#include "SystemFunctions.h"
#include "SystemFolders.h"


LPCTSTR DownloadFoldeRid = _T("{374DE290-123F-4565-9164-39C4925E467B}");
LPCTSTR SavedGameFolderRid = _T("{4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}");
LPCTSTR LinksFolderRid = _T("{bfb9d5e0-c6a9-404c-b2b2-ae6db6af4968}");
LPCTSTR ContactsFolderRid = _T("{56784854-C6CB-462b-8169-88E350ACB882}");
LPCTSTR FavoritesFolderRid = _T("{1777F761-68AD-4D8A-87BD-30B759FA33DD}");

static LPCTSTR KnownRids[] =
{
    DownloadFoldeRid, SavedGameFolderRid, LinksFolderRid, ContactsFolderRid, FavoritesFolderRid
};

#include "ShellWrapper.h"

BOOL CSystemFolders::Initialize()
{
    m_UserFileFolder = GetSystemFolderPath(CSIDL_PROFILE);
    m_FolderIdTbl[m_UserFileFolder] = CGuidEx(_T("{f3ce0f7c-4901-4acc-8648-d5d44b04ef8f}"));
    m_FolderIdTbl[m_UserFileFolder + _T("\\searches")] = CGuidEx(_T("{7d1d3a04-debb-4115-95cf-2f29da2920da}"));
    for (int i = 0; i < sizeof(KnownRids) / sizeof(KnownRids[0]); i++)
    {
        PWSTR ppszPath = NULL;
        CGuidEx guid(KnownRids[i]);
        if (::SHGetKnownFolderPath(guid, 0, NULL, &ppszPath) == S_OK)
        {
            m_FolderIdTbl[TString(ppszPath)] = guid;
            ::CoTaskMemFree(ppszPath);
        }
    }
/*
    TString pathTmp = GetSystemFolderPath(CSIDL_DRIVES);
    LPITEMIDLIST pidl = nullptr;
    HRESULT hr = SHGetFolderLocation(NULL, CSIDL_DRIVES, NULL, 0, &pidl);
    if (hr == S_OK)
    {
        CIDLEx cidl(pidl, true);
        TString strTtt = cidl.GetParseName();
        ILFree(pidl);
    }
*/
    return TRUE;
}

