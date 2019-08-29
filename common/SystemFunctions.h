#pragma once

extern HINSTANCE g_Instance;

BOOL IsWindowsVersionEqual(DWORD major, DWORD minor, DWORD servicePack);
BOOL IsWindowsXP();
BOOL IsWindows7();
BOOL IsWindows8();

TString GetAppPathName(HMODULE hMod, LPCTSTR lpName);
TString GetSystemFolderPath(int nFolder);
TString GetLocalAppDataPath();
TString GetAppDataPath();
TString GetUserProfilePath();

HICON WzLoadIcon(int iconSize, const wchar_t* path, int index, std::vector<HMODULE>& modules, HMODULE hShell32);
HICON CreateDisabledIcon(HICON icon, int size);

BOOL FindAndRemoveTreeItem(HWND hTreeWnd, HTREEITEM hParent, LPCTSTR text);

BOOL IsLanguageRTL(void);

BOOL IsDirectoryExists(const TString& path);
BOOL IsFileExists(const TString& path);
BOOL IsNetworkPath(const TString& path);
BOOL IsNamespacePath(const TString& path);
BOOL IsDiskRootPath(const TString& path);
TString GetTabItemText(const TString& path);
