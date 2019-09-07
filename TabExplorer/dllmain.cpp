// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "TabExplorer_i.h"
#include "dllmain.h"
#include "DebugLog.h"
#include "SystemFunctions.h"
#include "HookLibManager.h"
#include "PrivateMessage.h"

CTabExplorerModule _AtlModule;

void HookResult(int hookId, int retcode)
{
}

bool OnNewWindow(LPCITEMIDLIST pIDL)
{
    LogTrace(_T("OnNewWindow invoked by QTHookLib!"));
    return true;
}

BOOL OnShellExtLoaded(HMODULE hShlExtMod)
{
    RegisterPrivateMessage();
    CallbackStruct callbacks = { HookResult, OnNewWindow };
    if (!GetHookMgmt().LoadHookLib(hShlExtMod))
    {
        LogError(_T("HookLibManager fail to load QTHookLib!"));
        return FALSE;
    }

    int rtn = GetHookMgmt().Initialize(&callbacks);
    if (rtn != 0)
    {
        LogError(_T("HookLibManager fail to call Initialize, rtn = %d!"), rtn);
        return FALSE;
    }
    return TRUE;
}

void OnShellExtUnloaded()
{
    LogTrace(_T("HookLibManager try to unload QTHookLib!"));
    GetHookMgmt().UnloadHookLib();
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        LogTrace(_T("TabExplorer::DllMain() process(%u) attach..."), ::GetCurrentProcessId());

        wchar_t path[_MAX_PATH];
        GetModuleFileName(NULL, path, _countof(path));
        if (_wcsicmp(PathFindFileName(path), L"iexplore.exe") == 0)
        {
            LogError(_T("TabExplorer::DllMain() loaded in IE, quit now..."));
            return FALSE;
        }

        g_Instance = hInstance;
        //DisableThreadLibraryCalls(hInstance);
#if 1

        if (!OnShellExtLoaded(hInstance))
        {
            LogFatalErr(_T("OnShellExtLoaded failed in DllMain()!"));
            return FALSE;
        }
#endif
    }
    else if (dwReason == DLL_THREAD_ATTACH)
    {
        LogTrace(_T("TabExplorer::DllMain() Thread(%u) attach..."), ::GetCurrentThreadId());
        //if (g_bHookCopyThreads)
            //InitClassicCopyThread();
    }
    else if (dwReason == DLL_THREAD_DETACH)
    {
        LogTrace(_T("TabExplorer::DllMain() Thread(%u) detach..."), ::GetCurrentThreadId());
        //if (g_bHookCopyThreads)
            //FreeClassicCopyThread();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        LogTrace(_T("TabExplorer::DllMain() process(%u) detach..."), ::GetCurrentProcessId());
#if 1
        OnShellExtUnloaded();
#endif
    }
    return _AtlModule.DllMain(dwReason, lpReserved);
}
