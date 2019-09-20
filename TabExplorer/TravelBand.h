
#pragma once
#include "resource.h"       // main symbols


class CTravelBand
{
public:
	CTravelBand()
	{
        m_bUpToolbarAdd = false;
        m_hParentRebarWnd = NULL;
        m_RebarSubclassId = 0x19990612;
	}
	
    BOOL Initialize(CComPtr<IShellBrowser>& spShellBrowser, HWND hExplorerWnd);
    void Unintialize();
    void UpdateToolbarStatus();
    void SetTravelButtonStatus(bool canBack, bool canForward);
    HWND GetHwnd() const { return m_hParentRebarWnd; }

protected:
    BOOL AddUpButtonToTravelBand(HWND hParentRebarWnd);
    void RemoveUpButtonFromTravelBand();
    void SetUpButtonStatus(bool bEnable) //only for win7
    {
        m_Toolbar.SendMessage(TB_ENABLEBUTTON, 1, bEnable ? 1 : 0);
    }
    void ItemCustonDrawRepaint(NMTBCUSTOMDRAW* pDraw);
    LRESULT OnToolbarNotifyCustomDraw(NMTBCUSTOMDRAW* pDraw, BOOL& handled);
    LRESULT OnToolbarNotifyGetToolTip(NMTBGETINFOTIP* pTip, BOOL& handled);
    LRESULT OnToolbarNotifyRClick(NMMOUSE* pInfo, BOOL& handled);
    void OnThemeChanged();
    void OnToolbarCommand();

protected:	
	CComPtr<IShellBrowser> m_spShellBrowser;
    CWindow m_Toolbar;
    HWND m_hParentRebarWnd;
    BOOL m_bUpToolbarAdd;
	HICON m_IconNormal, m_IconHot, m_IconPressed, m_IconDisabled;
    UINT_PTR m_RebarSubclassId;

public:
    static LRESULT CALLBACK RebarSubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData );
};

