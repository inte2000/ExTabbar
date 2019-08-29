#pragma once

#include <map>
#include "GuidEx.h"

class CSystemFolders
{
public:
    CSystemFolders() { Initialize();  }
    virtual ~CSystemFolders() { m_FolderIdTbl.clear(); }

    BOOL Initialize();
protected:
    TString m_UserFileFolder;
    std::map<TString, CGuidEx> m_FolderIdTbl;
};

