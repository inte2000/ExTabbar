#pragma once

extern HINSTANCE g_Instance;

BOOL IsWindowsVersionEqual(DWORD major, DWORD minor, DWORD servicePack);
BOOL IsWindows7();
BOOL IsWindows8();

TString GetAppPathName(HMODULE hMod, LPCTSTR lpName);
TString GetLocalAppDataPath();
TString GetAppDataPath();
TString GetUserProfilePath();

HICON WzLoadIcon(int iconSize, const wchar_t* path, int index, std::vector<HMODULE>& modules, HMODULE hShell32);
HICON CreateDisabledIcon(HICON icon, int size);

BOOL FindAndRemoveTreeItem(HWND hTreeWnd, HTREEITEM hParent, LPCTSTR text);

BOOL IsLanguageRTL(void);
