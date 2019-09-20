#include "pch.h"
#include "framework.h"
#include "shlobj.h"
#include "SystemFunctions.h"

HINSTANCE g_Instance = NULL;

BOOL IsWindowsVersionEqual(DWORD major, DWORD minor, DWORD servicePack)
{
    OSVERSIONINFOEX ove = { 0 };
    DWORDLONG dwlConditionMask = 0;

    ove.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    ove.dwMajorVersion = major;
    ove.dwMinorVersion = minor;
    ove.wServicePackMajor = (WORD)servicePack;

    DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, VER_EQUAL);

    return VerifyVersionInfo(&ove, mask, dwlConditionMask);
}
/*
Windows 10     10.0
Windows Server 2016   10.0
Windows 8.1   6.3
Windows Server 2012 R2   6.3
Windows 8   6.2
Windows Server 2012    6.2
Windows 7   6.1      (6.1.7600)
Windows Server 2008 R2  6.1
Windows Server 2008  6.0
Windows Vista  6.0    (6.0.6000)
Windows Server 2003 R2  5.2
Windows Server 2003  5.2
Windows XP 64-Bit Edition  5.2   (5.2.3790)
Windows XP  5.1      (5.1.2600)
Windows 2000  5.0   (5.0.2195)
Windows Me   (4.90.3000)
Windows 98 Second Edition 	4.1.2222
Windows 98 	4.1.1998
Windows NT Workstation 4.0 	4.0.1381
Windows 95 	4.0.950
Windows NT Workstation 3.51 	3.51.1057
Windows NT Workstation 3.5 	3.5.807
Windows for Workgroups 3.11 	3.11
Windows NT 3.1 	3.10.528
Windows 3.0 	3
Windows 2.0 	2.11
Windows 1.0 	1.04
*/

BOOL IsWindowsXP()
{
    return IsWindowsVersionEqual(5, 1, 0);
}

BOOL IsWindows7()
{
    return (IsWindowsVersionEqual(6, 1, 0) || IsWindowsVersionEqual(6, 1, 1));
}

BOOL IsWindows8()
{
    return (IsWindowsVersionEqual(6, 2, 0) || IsWindowsVersionEqual(6, 3, 0));
}

BOOL IsWindows10()
{
    return IsWindowsVersionEqual(10, 0, 0);
}

TString GetAppPathName(HMODULE hMod, LPCTSTR lpName)
{
	TCHAR szBuf[MAX_PATH];

	DWORD len = ::GetModuleFileName(hMod, szBuf, MAX_PATH);
    szBuf[len] = 0;

    TCHAR *pSlash = _tcsrchr(szBuf, _T('\\'));
    if (pSlash != nullptr)
    {
        std::size_t space = MAX_PATH - _tcslen(szBuf) + _tcslen(pSlash + 1);
        _tcscpy_s(pSlash + 1, space, lpName);
    }
    else
    {
        _tcscpy_s(szBuf, MAX_PATH, lpName);
    }

    return TString(szBuf);
}

TString GetSystemFolderPath(int nFolder)
{
    TCHAR szBuf[MAX_PATH];

    HRESULT hr = ::SHGetFolderPath(NULL, nFolder, NULL, 0, szBuf);
    if (SUCCEEDED(hr))
    {
        return TString(szBuf);
    }

    return TString(_T(""));
}

TString GetMyComputerPath()
{
    return GetSystemFolderPath(CSIDL_DRIVES);
}

TString GetLocalAppDataPath()
{
    return GetSystemFolderPath(CSIDL_LOCAL_APPDATA);
}

TString GetAppDataPath()
{
    return GetSystemFolderPath(CSIDL_APPDATA);
}

TString GetUserProfilePath()
{
    return GetSystemFolderPath(CSIDL_PROFILE);
}

/*
TString GetTempFilePathName(LPCTSTR prefix)
{
    TCHAR szBuf[MAX_PATH];
    ::GetTempFileName(sTempPath, _T(“MyPre_”), 0, sTempFile.GetBuffer(MAX_PATH));
}
*/

HICON WzLoadIcon(int iconSize, const wchar_t* path, int index, std::vector<HMODULE>& modules, HMODULE hShell32)
{
    if (!path)
        return (HICON)LoadImage(hShell32, MAKEINTRESOURCE(index), IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
    wchar_t text[1024];
    wcscpy_s(text, path);
    DoEnvironmentSubst(text, _countof(text));
    wchar_t* c = wcsrchr(text, ',');
    if (c)
    {
        // resource file
        *c = 0;
        const wchar_t* res = c + 1;
        int idx = _wtol(res);
        if (idx > 0) res = MAKEINTRESOURCE(idx);
        if (!text[0])
            return (HICON)LoadImage(g_Instance, res, IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
        HMODULE hMod = GetModuleHandle(PathFindFileName(text));
        if (!hMod)
        {
            hMod = LoadLibraryEx(text, NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
            if (!hMod) return NULL;
            modules.push_back(hMod);
        }
        return (HICON)LoadImage(hMod, res, IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
    }
    else
    {
        return (HICON)LoadImage(NULL, text, IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
    }
}

HICON CreateDisabledIcon(HICON icon, int size)
{
    // convert normal icon to grayscale
    ICONINFO info;
    GetIconInfo(icon, &info);

    BITMAPINFO bi = { 0 };
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bi.bmiHeader.biHeight = size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    HDC hdc = CreateCompatibleDC(NULL);

    unsigned int* bits;
    HBITMAP bmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)& bits, NULL, 0);
    HGDIOBJ bmp0 = SelectObject(hdc, bmp);
    RECT rc = { 0,0,size,size };
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    DrawIconEx(hdc, 0, 0, icon, size, size, 0, NULL, DI_NORMAL);
    SelectObject(hdc, bmp0);
    DeleteDC(hdc);
    if (info.hbmColor) DeleteObject(info.hbmColor);
    info.hbmColor = bmp;
    int n = size * size;
    for (int i = 0; i < n; i++)
    {
        unsigned int& pixel = bits[i];
        int r = (pixel & 255);
        int g = ((pixel >> 8) & 255);
        int b = ((pixel >> 16) & 255);
        int l = (77 * r + 151 * g + 28 * b) / 256;
        pixel = (pixel & 0xFF000000) | (l * 0x010101);
    }

    icon = CreateIconIndirect(&info);
    DeleteObject(bmp);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    return icon;
}

//_T("快速访问")
BOOL FindAndRemoveTreeItem(HWND hTreeWnd, HTREEITEM hParent, LPCTSTR text)
{
    int count = TreeView_GetCount(hTreeWnd);
    HTREEITEM hChild = TreeView_GetChild(hTreeWnd, NULL);
    while (hChild != NULL)
    {
        TCHAR szBuf[128] = { 0 };
        TVITEM item;
        item.hItem = hChild;
        item.mask = TVIF_TEXT;
        item.pszText = szBuf;
        item.cchTextMax = 128;

        if (TreeView_GetItem(hTreeWnd, &item))
        {
            if (lstrcmp(szBuf, text) == 0)
            {
                TreeView_DeleteItem(hTreeWnd, hChild);
                break;
            }
        }
        hChild = TreeView_GetNextItem(hTreeWnd, hChild, TVGN_NEXT);
    }
    return TRUE;
}

HBITMAP GetParentBkBitmap(HWND hWnd, HWND hParentWnd)
{
    HDC dc = ::GetDC(NULL);

    RECT cRect;
    ::GetClientRect(hWnd, &cRect);

    HBITMAP bitmap = CreateCompatibleBitmap(dc, cRect.right, cRect.bottom);

    ReleaseDC(NULL, dc);

    HDC memDC = CreateCompatibleDC(NULL);
    HGDIOBJ oldBitmap = SelectObject(memDC, bitmap);

    //此处可以调用SetClipRect()等函数来限制绘制范围
    SendMessage(hParentWnd, WM_ERASEBKGND, (WPARAM)memDC, 0);
    SendMessage(hParentWnd, WM_PAINT, (WPARAM)memDC, 0);
    //至此memDC上已经保存了父窗口的背景内容

    //用户可以调用BitBlt(...)等函数拷贝memDC的内容到子窗口的某个区域，这样就达到了透明效果;

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);

    return bitmap;
    //DeleteObject(bitmap);
}

BOOL IsLanguageRTL(void)
{
#ifdef _DEBUG
    //	return true; // uncomment this to simulate RTL environment
#endif
    LOCALESIGNATURE localesig;
    LANGID language = GetUserDefaultUILanguage();
    if (GetLocaleInfoW(language, LOCALE_FONTSIGNATURE, (LPWSTR)&localesig, (sizeof(localesig) / sizeof(wchar_t))) && (localesig.lsUsb[3] & 0x08000000))
        return TRUE;
    
    return FALSE;
}

BOOL IsDirectoryExists(const TString& path)
{
    if (path.empty())
        return FALSE;

    if (path.find(_T("::"), 0) == 0)
        return FALSE;

    return ::PathFileExists(path.c_str());
}

BOOL IsFileExists(const TString& path) 
{
    if (path.empty())
        return FALSE;

    if (path.find(_T("::"), 0) == 0)
        return FALSE;

    return ::PathFileExists(path.c_str());
}

LPCTSTR  INDICATOR_FTP = _T("ftp://");
LPCTSTR  INDICATOR_HTTP = _T("http://");
LPCTSTR  INDICATOR_NAMESPACE = _T("::");
LPCTSTR  INDICATOR_NETWORK = _T("\\\\");

BOOL IsNetworkPath(const TString& path)
{
    if (path.empty())
        return FALSE;

    if ((path.find(INDICATOR_NETWORK, 0) == 0)
        || (path.find(INDICATOR_HTTP, 0) == 0)
        || (path.find(INDICATOR_FTP, 0) == 0))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsNamespacePath(const TString& path)
{
    if (path.find(INDICATOR_NAMESPACE, 0) == 0)
        return TRUE;

    return FALSE;
}

BOOL IsDiskRootPath(const TString& path)
{
    std::size_t pos = path.find(_T(':'), 0);
    if (pos >= 0)
    {
        if ((path.length() - pos) <= 2)
            return TRUE;
    }

    return FALSE;
}

struct FindChildClass
{
    LPCTSTR className;
    LPCTSTR wndCaption;
    HWND hWnd;
};

static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    FindChildClass *find = (FindChildClass *)lParam;
    TCHAR className[256];
    ::GetClassName(hwnd, className, _countof(className));
    ATLTRACE(_T("EnumChildProc find class %s\n"), className);
    if (find->wndCaption != nullptr)
    {
        TCHAR caption[256] = { 0 };
        ::GetWindowText(hwnd, caption, _countof(caption));
        if ((lstrcmpi(className, find->className) == 0) && (lstrcmpi(caption, find->wndCaption) == 0))
        {
            find->hWnd = hwnd;
            return FALSE;
        }
    }
    else
    {
        if (lstrcmpi(className, find->className) == 0)
        {
            find->hWnd = hwnd;
            return FALSE;
        }
    }

    return TRUE;
}

HWND FindChildWndEx(HWND hwnd, LPCTSTR className, LPCTSTR caption)
{
    FindChildClass find = { className, caption, NULL };
    EnumChildWindows(hwnd, EnumChildProc, (LPARAM)&find);
    
    return find.hWnd;
}

HIMAGELIST GetSystemImageList(BOOL bLargeIcon)
{
    HIMAGELIST hSysImgList = NULL;
    SHFILEINFO shfi;
    UINT uFlags = SHGFI_SYSICONINDEX | (bLargeIcon ? SHGFI_LARGEICON : SHGFI_SMALLICON);
    hSysImgList = (HIMAGELIST)::SHGetFileInfo((LPCTSTR)_T(""), 0, &shfi, sizeof(SHFILEINFO), uFlags);
    if (hSysImgList == NULL)
    {
        return NULL;
    }

    return hSysImgList;
}

int GetShellObjectIcon(LPCITEMIDLIST pidl)
{
    SHFILEINFO sfi;
    DWORD_PTR rtn = ::SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    if (rtn != 0)
    {
        return sfi.iIcon;
    }

    return -1;
}

TString TStringFromWStr(LPWSTR wstr)
{
    TString strRtn;
#ifdef UNICODE
    strRtn = wstr;
#else
    //don't forget ...
#endif

    return std::move(strRtn);
}

/*
template<class T>
void ClearStack(std::stack<T>& stk)
{
    while (stk.size() > 0)
        stk.pop();
}
*/
