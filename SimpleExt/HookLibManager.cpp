#include "stdafx.h"
#include "HookLibManager.h"
#include "SystemFunctions.h"


bool HookLibManager::LoadHookLib(HMODULE hAppMod)
{
    TString hookDllPathName = GetAppPathName(hAppMod, _T("QTHookLib.dll"));
    HMODULE hHookDll = ::LoadLibrary(hookDllPathName.c_str());
    if (hHookDll == NULL)
        return false;
    
    m_DllInitialize = reinterpret_cast<fpInitialize>(::GetProcAddress(hHookDll, "Initialize"));
    m_DllDispose = reinterpret_cast<fpDispose>(::GetProcAddress(hHookDll, "Dispose"));
    m_DllInitShellBrowserHook = reinterpret_cast<fpInitShellBrowserHook>(::GetProcAddress(hHookDll, "InitShellBrowserHook"));

    if ((m_DllInitialize == nullptr) || (m_DllDispose == nullptr)
        || (m_DllInitShellBrowserHook == nullptr))
    {
        ::FreeLibrary(hHookDll);
        InitInternalData();
        return false;
    }

    m_hHookDll = hHookDll;
    return true;
}

void HookLibManager::UnloadHookLib()
{
    if (m_hHookDll == NULL)
        return;

    if (m_libInitlized)
    {
        Dispose();
    }
    ::FreeLibrary(m_hHookDll);
    InitInternalData();
}

//interface
int HookLibManager::Initialize(CallbackStruct* cb)
{
    int rtn = -1;

    if (m_DllInitialize != nullptr)
    {
        rtn = m_DllInitialize(cb);
        m_libInitlized = true;
    }

    return rtn;
}

int HookLibManager::Dispose()
{
    int rtn = -1;

    if (m_DllDispose != nullptr)
    {
        rtn = m_DllDispose();
        m_libInitlized = false;
    }

    return rtn;
}

int HookLibManager::InitShellBrowserHook(IShellBrowser* psb)
{
    int rtn = -1;

    if (m_DllInitShellBrowserHook != nullptr)
    {
        rtn = m_DllInitShellBrowserHook(psb);
    }

    return rtn;
}

void HookLibManager::InitInternalData()
{
    m_DllInitialize = nullptr;
    m_hHookDll = NULL;
    m_DllDispose = nullptr;
    m_DllInitShellBrowserHook = nullptr;
    m_libInitlized = false;
}

static HookLibManager s_hookMng;

HookLibManager& GetHookMgmt()
{
    return s_hookMng;
}