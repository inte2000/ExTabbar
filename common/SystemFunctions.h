#pragma once

extern HINSTANCE g_Instance;

BOOL VerifyWindowsVersion(DWORD major, DWORD minor, DWORD servicePack, int op);

inline BOOL IsWinVersionEqual(DWORD major, DWORD minor, DWORD servicePack)
{
    return VerifyWindowsVersion(major, minor, servicePack, VER_EQUAL);
}
inline BOOL IsWinVersionGreatEqual(DWORD major, DWORD minor, DWORD servicePack)
{
    return VerifyWindowsVersion(major, minor, servicePack, VER_GREATER_EQUAL);
}
inline BOOL IsWindowsXP()
{
    return IsWinVersionEqual(5, 1, 0);
}
inline BOOL IsWindowsVista()
{
    return IsWinVersionEqual(6, 0, 0);
}
inline BOOL IsWindows7()
{
    return (IsWinVersionEqual(6, 1, 0) || IsWinVersionEqual(6, 1, 1));
}
inline BOOL IsWindows8()
{
    return (IsWinVersionEqual(6, 2, 0) || IsWinVersionEqual(6, 3, 0));
}
inline BOOL IsWindows10()
{
    return IsWinVersionEqual(10, 0, 0);
}

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

BOOL IsWindowMatchClassName(HWND hwnd, LPCTSTR className, LPCTSTR caption = nullptr);
HWND FindChildWndEx(HWND hwnd, LPCTSTR className, LPCTSTR caption = nullptr);

HIMAGELIST GetSystemImageList(BOOL bLargeIcon);
TString TStringFromWStr(LPWSTR wstr);
WCHAR* WStrFromTString(WCHAR* wstrBuf, int bufChs, const TString& tstr);

HGLOBAL CopyGlobalMemoryHandle(HGLOBAL hDest, HGLOBAL hSource);

HBITMAP GetWindowSnapBitmap(HWND hWnd, SIZE* size = nullptr);
void DoProcessEvent(HWND hWnd = NULL);
BOOL BringWindowForeground(HWND hWnd);
int CalcIconSizeByDpi(BOOL bUsingLargeIcon);
