
#pragma once
#include "resource.h"       // main symbols


class CStatusBar
{
public:
    CStatusBar()
	{
        m_bStatusbarSubclass = false;
        m_hStatusbarWnd = NULL;
        m_bForceRefresh = false;
        m_bResetStatus = true;
        m_StatusbarSubclassId = 0x19761012;
    }
	
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hStatusbarWnd);
    void Unintialize();
    HWND GetHwnd() const { return m_hStatusbarWnd; }

    enum
    {
        SPACE_SHOW = 1, // show free space and selection size
        SPACE_TOTAL = 2, // show total size when nothing is selected
        SPACE_WIN7 = 4, // running on Win7 (fix the status bar parts and show the disk free space)
    };

protected:
    void ForceRefresh();
    void ResetStatus();
    __int64 GetCurrentSelectedItemSize();
    BOOL GetFreeSpaceOfCurFolderLocation(ULARGE_INTEGER& size);
	
protected:	
	CComPtr<IShellBrowser> m_spShellBrowser;
    HWND m_hStatusbarWnd;
    BOOL m_bStatusbarSubclass;
    UINT_PTR m_StatusbarSubclassId;
    bool m_bForceRefresh;
    bool m_bResetStatus;

public:
    static LRESULT CALLBACK StatusbarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,                                          DWORD_PTR dwRefData );
};

