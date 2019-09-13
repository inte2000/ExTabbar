#pragma once

#include <map>
#include "IDListData.h"

class CShellFoldersMap
{
public:
    CShellFoldersMap() { Initialize();  }
    virtual ~CShellFoldersMap() { m_ShlFolders.clear(); }

    bool Initialize();
    bool InsertFolder(const TString& shlName, LPCITEMIDLIST pidl);
    bool FindFolder(const TString& shlName, CIDListData& data) const;
protected:
    std::map<TString, CIDListData> m_ShlFolders;
};

