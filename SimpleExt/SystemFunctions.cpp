#include "stdafx.h"
#include "shlobj.h"
#include "SystemFunctions.h"

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
    ::GetTempFileName(sTempPath, _T(¡°MyPre_¡±), 0, sTempFile.GetBuffer(MAX_PATH));
}
*/