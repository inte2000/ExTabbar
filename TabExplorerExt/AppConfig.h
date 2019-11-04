#pragma once

class CAppConfig final
{
    friend CAppConfig& AppGetConfig();
protected:
    CAppConfig();
    ~CAppConfig();

public:
    BOOL IsInit() const { return m_bInit; }
    BOOL LoadConfig();
    BOOL SaveConfig();

    // attr
    CString GetCmdProgramPathName() const { return m_strCmdProgramPathName; }
    void SetCmdProgramPathName(LPCTSTR lpszPathName) { m_strCmdProgramPathName = lpszPathName; }

protected:
    BOOL m_bInit;

    CString m_strCmdProgramPathName;
};

extern CAppConfig& AppGetConfig();
