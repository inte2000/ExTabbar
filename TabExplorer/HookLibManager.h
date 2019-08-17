#pragma once


struct CallbackStruct 
{
    void (*fpHookResult)(int hookId, int retcode);
    bool (*fpNewWindow)(LPCITEMIDLIST pIDL);
};

typedef int (*fpInitialize)(CallbackStruct* cb);
typedef int (*fpDispose)();
typedef int (*fpInitShellBrowserHook)(IShellBrowser* psb);

class HookLibManager final
{
public:
    HookLibManager() { InitInternalData(); }
    ~HookLibManager() { UnloadHookLib(); }
    
    bool LoadHookLib(HMODULE hAppMod);
    void UnloadHookLib();
    
    //interface from hooklib dll
    int Initialize(CallbackStruct* cb);
    int Dispose();
    int InitShellBrowserHook(IShellBrowser* psb);

protected:
    void InitInternalData();
private:
    bool m_libInitlized;
    HMODULE m_hHookDll;
    fpInitialize m_DllInitialize;
    fpDispose m_DllDispose;
    fpInitShellBrowserHook m_DllInitShellBrowserHook;
};

HookLibManager& GetHookMgmt();
