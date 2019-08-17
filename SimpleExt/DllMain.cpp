#include "stdafx.h"
#include "DllMain.h"
#include "DebugLog.h"
#include "SystemFunctions.h"
#include "HookLibManager.h"


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
