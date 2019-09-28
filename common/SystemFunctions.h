#pragma once

extern HINSTANCE g_Instance;

BOOL IsWindowsVersionEqual(DWORD major, DWORD minor, DWORD servicePack);
BOOL IsWindowsXP();
BOOL IsWindows7();
BOOL IsWindows8();
BOOL IsWindows10();

TString GetAppPathName(HMODULE hMod, LPCTSTR lpName);
TString GetSystemFolderPath(int nFolder);
TString GetMyComputerPath();
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

HWND FindChildWndEx(HWND hwnd, LPCTSTR className, LPCTSTR caption = nullptr);

HIMAGELIST GetSystemImageList(BOOL bLargeIcon);
int GetShellObjectIcon(LPCITEMIDLIST pidl);
TString TStringFromWStr(LPWSTR wstr);
WCHAR* WStrFromTString(WCHAR* wstrBuf, int bufChs, const TString& tstr);

HGLOBAL CopyGlobalMemoryHandle(HGLOBAL hDest, HGLOBAL hSource);

HBITMAP GetWindowSnapBitmap(HWND hWnd, SIZE* size = nullptr);
