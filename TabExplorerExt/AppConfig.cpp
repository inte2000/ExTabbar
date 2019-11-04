#include "stdafx.h"
#include "AppConfig.h"


CAppConfig::CAppConfig() 
{ 
    m_bInit = FALSE; 

    m_strCmdProgramPathName = _T("C:\\Windows\\System32\\cmd.exe");
}

CAppConfig::~CAppConfig()
{
}

BOOL CAppConfig::LoadConfig()
{
    m_bInit = TRUE;
    return TRUE;
}

BOOL CAppConfig::SaveConfig()
{
    return TRUE;
}


CAppConfig& AppGetConfig() 
{ 
    static CAppConfig s_AppCfg;

    if (!s_AppCfg.IsInit())
    {
        s_AppCfg.LoadConfig();
    }

    return s_AppCfg; 
}
