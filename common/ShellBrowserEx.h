#pragma once

#include <vector>
#include "ShellWrapper.h"

class CShellBrowserEx
{
public:
    CShellBrowserEx() {  }
    CShellBrowserEx(CComPtr<IShellBrowser>& spShellBrowser) { m_spShellBrowser = spShellBrowser; }
    virtual ~CShellBrowserEx()
    {
        m_spShellBrowser.Release();
        m_spShellBrowser = NULL;
    }

    void SetIShellBrowser(CComPtr<IShellBrowser>& spShellBrowser);
    CComPtr<IShellBrowser> GetIShellBrowser() { return m_spShellBrowser; }
    void SetUsingListView(bool listview);
    int GetFocusedIndex();
    CIDLEx GetFocusedItem();
    CIDLEx GetItem(int idx, bool noAppend = false);
    int GetItemCount();
    HRESULT GetItems(std::vector<CIDLEx *>& items, bool selectedOnly = false, bool noAppend = false);
    HRESULT GetSelectedItems(std::vector<CIDLEx*>& items, bool noAppend = false) { return GetItems(items, true, noAppend); }
    int GetSelectedCount();
    CIDLEx ILAppend(LPITEMIDLIST pidl);

    CIDLEx GetShellPath();
    bool IsFolderTreeVisible();
    bool IsFolderTreeVisible(HWND* hwnd);
    HRESULT Navigate(const CIDLEx& cidl, UINT flags = SBSP_SAMEBROWSER);
    void SelectItem(int idx);
    bool SelectionAvailable();
    void SetStatusText(const TString& status);

    static CIDLEx GetShellPath(CComPtr<IFolderView> pFolderView);
protected:
    HRESULT GetFolderView(CComPtr<IFolderView>& spFolderView);

protected:
    CComPtr<IShellBrowser> m_spShellBrowser;
};


