#pragma once

#include <map>
#include "IDListData.h"

class CShellFoldersMap
{
public:
    CShellFoldersMap() { m_bInitialized = false;  }
    virtual ~CShellFoldersMap() { m_ShlFolders.clear(); }

    bool IsInitialized() const { return m_bInitialized; }
    bool Initialize();
    bool InsertFolder(const TString& shlName, LPCITEMIDLIST pidl);
    bool FindFolder(const TString& shlName, CIDListData& data) const;

protected:
    bool m_bInitialized;
    std::map<TString, CIDListData> m_ShlFolders;
};

CShellFoldersMap& TeGetFolderMap();
