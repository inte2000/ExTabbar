#include "pch.h"
#include "framework.h"
#include "HookLibMgmt.h"
#include "SystemFunctions.h"


bool HookLibMgmt::LoadHookLib(HMODULE hAppMod)
{
    TString hookDllPathName = GetAppPathName(hAppMod, _T("ETHookLib.dll"));
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

void HookLibMgmt::UnloadHookLib()
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
int HookLibMgmt::Initialize(CallbackStruct* cb)
{
    int rtn = -1;

    if (m_DllInitialize != nullptr)
    {
        rtn = m_DllInitialize(cb);
        m_libInitlized = true;
    }

    return rtn;
}

int HookLibMgmt::Dispose()
{
    int rtn = -1;

    if (m_DllDispose != nullptr)
    {
        rtn = m_DllDispose();
        m_libInitlized = false;
    }

    return rtn;
}

int HookLibMgmt::InitShellBrowserHook(IShellBrowser* psb)
{
    int rtn = -1;

    if (m_DllInitShellBrowserHook != nullptr)
    {
        rtn = m_DllInitShellBrowserHook(psb);
    }

    return rtn;
}

void HookLibMgmt::InitInternalData()
{
    m_DllInitialize = nullptr;
    m_hHookDll = NULL;
    m_DllDispose = nullptr;
    m_DllInitShellBrowserHook = nullptr;
    m_libInitlized = false;
}

static HookLibMgmt s_hookMng;

HookLibMgmt& GetHookMgmt()
{
    return s_hookMng;
}
