#include "pch.h"
#include "framework.h"
#include "ShellFunctions.h"
#include "ShellGuids.h"
#include "ShellWrapper.h"
#include "GuidEx.h"
#include "SystemFunctions.h"
#include "ShellBrowserEx.h"


void CShellBrowserEx::SetIShellBrowser(CComPtr<IShellBrowser>& spShellBrowser)
{
    if (m_spShellBrowser != nullptr)
    {
        m_spShellBrowser.Release();
        m_spShellBrowser = nullptr;
    }

    m_spShellBrowser = spShellBrowser;
}

void CShellBrowserEx::SetUsingListView(bool listview)
{
    CComPtr<IFolderViewOptions> spFVO;
    //m_spShellBrowser->QueryInterface(IID_IFolderViewOptions, (void **)& spFVO);
    m_spShellBrowser.QueryInterface(&spFVO);
    if (spFVO != nullptr)
    {
        spFVO->SetFolderViewOptions(FVO_VISTALAYOUT, listview ? FVO_VISTALAYOUT : FVO_DEFAULT);
    }
}

int CShellBrowserEx::GetFocusedIndex()
{
    int index = -1;
    CComPtr<IFolderView> spFolderView;
    HRESULT hr = GetFolderView(spFolderView);
    if (hr == S_OK)
    {
        int focusIdx = -1;
        hr = spFolderView->GetFocusedItem(&focusIdx);
        if (hr == S_OK)
        {
            index = focusIdx;
        }
    }

    return index;
}

CIDLEx CShellBrowserEx::GetFocusedItem()
{
    int focusedIndex = GetFocusedIndex();
    if (focusedIndex != -1)
    {
        return std::move(GetItem(focusedIndex));
    }

    return std::move(CIDLEx());
}

CIDLEx CShellBrowserEx::GetItem(int idx, bool noAppend) 
{
    CIDLEx rtnIdl;

    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr != S_OK)
        return std::move(rtnIdl);
    
    LPITEMIDLIST pidl = nullptr;
    pFV->Item(idx, &pidl);
    if (noAppend || pidl == nullptr) 
    {
        return std::move(CIDLEx(pidl, false));
    }
    rtnIdl.Attach(pidl, false);
    CIDLEx path = GetShellPath(pFV);
    //PIDLIST_ABSOLUTE ILCombine(PCIDLIST_ABSOLUTE  pidl1, PCUIDLIST_RELATIVE pidl2);
    if (!path.IsEmpty())
    {
        //CIDLEx result;
        //CIDLEx::Concat(path, rtnIdl, result);
        //return std::move(result);
        path.Concat(rtnIdl);
        return std::move(path);
    }

    if ((pidl != nullptr) && !noAppend)
    {
        //::CoTaskMemFree(pidl);
    }

    return std::move(CIDLEx());
}

int CShellBrowserEx::GetItemCount()
{
    int count = 0;
    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if(hr == S_OK)
    {
        hr = pFV->ItemCount(SVGIO_ALLVIEW, &count);
    }

    return count;
}

HRESULT CShellBrowserEx::GetItems(std::vector<CIDLEx *>& items, bool selectedOnly, bool noAppend)
{
    CGuidEx guid(strIID_IEnumIDList);

    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr != S_OK)
        return hr;

    CIDLEx path;
    if (!noAppend)
        path = GetShellPath(pFV);

    IEnumIDList* list = nullptr;
    UINT flags = SVGIO_FLAG_VIEWORDER | (selectedOnly ? SVGIO_SELECTION : SVGIO_ALLVIEW);
    hr = pFV->Items(flags, guid, (void **)& list);
    if (hr == S_OK)
    {
        LPITEMIDLIST pidl = nullptr;
        while (list->Next(1, &pidl, nullptr) == S_OK)
        {
            if (noAppend)
            {
                CIDLEx* pidlex = new CIDLEx(pidl, false);
                items.push_back(pidlex);
            }
            else
            {
                CIDLEx* pidlex = new CIDLEx(path);
                pidlex->Concat(pidl);
                items.push_back(pidlex);
            }
        }

        list->Release();
    }

    return hr;
}

void FreeCIdlist(std::vector<CIDLEx*>& cidlist)
{
    for (auto& cidl : cidlist)
    {
        delete cidl;
    }

    cidlist.clear();
}

HRESULT CShellBrowserEx::GetSelectedItems(std::vector<CIDListData *>& items, bool bDisplayName)
{ 
    std::vector<CIDLEx *> cidlist;
    HRESULT hr = GetItems(cidlist, true, false);
    if (hr == S_OK)
    {
        for (auto& cidl : cidlist)
        {
            TString path = bDisplayName ? cidl->GetDisplayName() : cidl->GetParseName();
            CIDListData* idlData = new CIDListData(cidl, path);
            items.push_back(idlData);
        }

        FreeCIdlist(cidlist);
    }
    return hr;
}

HRESULT CShellBrowserEx::SetSelectedItems(const std::vector<CIDListData*>& items, const TString& focusPath)
{
    CComPtr<IFolderView> spFolderView;
    CComPtr<IShellView> pShellView;
    HRESULT hr = m_spShellBrowser->QueryActiveShellView(&pShellView);
    if (hr != S_OK)
        return hr;

    hr = pShellView->QueryInterface(IID_IFolderView, (void**)& spFolderView);
    if (hr != S_OK)
        return hr;

    bool focusSeted = false;
    spFolderView->SelectItem(0, SVSI_DESELECTOTHERS);
    for (auto& x : items)
    {
        CIDLEx cidl;
        const TString& strPath = x->GetPath();
        auto rawIdl = x->GetIDLData();
        if ((std::get<0>(rawIdl) != nullptr) && (std::get<1>(rawIdl) > 0))
        {
            cidl.CreateByIdListData(std::get<0>(rawIdl), std::get<1>(rawIdl));
        }
        else
        {
            LPITEMIDLIST pidl = nullptr;
            CShellFolder desktopFolder = CShellFolder::GetDesktopFolder();
            hr = desktopFolder.ParseDisplayName(strPath.c_str(), &pidl);
            if (hr == S_OK)
            {
                cidl.Attach(pidl, false);
            }
        }
        LPITEMIDLIST pidlLast = ::ILFindLastID(cidl);
        UINT uFlags = SVSI_SELECT | SVSI_ENSUREVISIBLE;
        if (strPath == focusPath)
        {
            uFlags |= SVSI_FOCUSED; 
            focusSeted = true;
        }
        pShellView->SelectItem(pidlLast, uFlags);
        cidl.Release();
    }
    if (!focusSeted)
    {
        LPITEMIDLIST pidlFocus = nullptr;
        CShellFolder desktopFolder = CShellFolder::GetDesktopFolder();
        hr = desktopFolder.ParseDisplayName(focusPath.c_str(), &pidlFocus);
        if (hr == S_OK)
        {
            LPITEMIDLIST pidlFocusLast = ::ILFindLastID(pidlFocus);
            pShellView->SelectItem(pidlFocusLast, SVSI_FOCUSED | SVSI_ENSUREVISIBLE);
            ::CoTaskMemFree(pidlFocus);
        }
    }

    return S_OK;
}

int CShellBrowserEx::GetSelectedCount()
{
    int count = 0;
    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr == S_OK)
    {
        pFV->ItemCount(SVGIO_SELECTION, &count);
    }

    return count;
}

CIDLEx CShellBrowserEx::GetShellPath()
{
    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr == S_OK)
    {
        return std::move(CShellBrowserEx::GetShellPath(pFV));
    }

    return std::move(CIDLEx());
}

CIDLEx CShellBrowserEx::ILAppend(LPITEMIDLIST pidl)
{
    CIDLEx ptr(pidl, true);
    if (ptr.IsEmpty())
        return std::move(ptr);

    CIDLEx path = GetShellPath();
    if (!path.IsEmpty())
    {
        path.Concat(ptr);
        return std::move(path);
    }

    return std::move(ptr);
}

inline bool CShellBrowserEx::IsFolderTreeVisible() 
{
    HWND hWnd;
    return IsFolderTreeVisible(&hWnd);
}

bool CShellBrowserEx::IsFolderTreeVisible(HWND *hwnd)
{
    *hwnd = NULL;
    return (IsWindowsXP() && (0 == m_spShellBrowser->GetControlWindow(FCW_TREE, hwnd)));
}

HRESULT CShellBrowserEx::Navigate(const CIDLEx& cidl, UINT flags)
{
    if (!cidl.IsEmpty())
    {
        return m_spShellBrowser->BrowseObject(cidl, flags);
    }

    return S_FALSE;
}

void CShellBrowserEx::SelectItem(int idx)
{
    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr == S_OK)
    {
        pFV->SelectItem(idx, SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED);
    }
}

bool CShellBrowserEx::SelectionAvailable()
{
    CComPtr<IFolderView> pFV;
    HRESULT hr = GetFolderView(pFV);
    if (hr == S_OK)
    {
        int items = 0;
        return (pFV->ItemCount(SVGIO_SELECTION, &items) == S_OK);
    }

    return false;
}

void CShellBrowserEx::SetStatusText(const TString& status)
{
#ifdef UNICODE
    m_spShellBrowser->SetStatusTextSB(status.c_str());
#else
    WCHAR strBuf[128];
    ::MultiByteToWideChar(CP_ACP, 0, tatus.c_str(), -1, strBuf, 128);
    strBuf[127] = 0;
    m_spShellBrowser->SetStatusTextSB(strBuf);
#endif
}

//protected:
HRESULT CShellBrowserEx::GetFolderView(CComPtr<IFolderView>& spFolderView)
{
    CComPtr<IShellView> pShellView;
    HRESULT hr = m_spShellBrowser->QueryActiveShellView(&pShellView);
    if (hr == S_OK)
    {
        hr = pShellView->QueryInterface(IID_IFolderView, (void**)& spFolderView);
    }

    return hr;
}

CIDLEx CShellBrowserEx::GetShellPath(CComPtr<IFolderView> pFolderView)
{
    CIDLEx rtnIdl;
    if (pFolderView != nullptr)
    {
        CComPtr<IPersistFolder2> ppf2;
        CGuidEx guid(strIID_IPersistFolder2);
        HRESULT hr = pFolderView->GetFolder(guid, (void**)& ppf2);
        if (hr == S_OK)
        {
            LPITEMIDLIST pidl = nullptr;
            hr = ppf2->GetCurFolder(&pidl);
            if (hr == S_OK)
            {
                rtnIdl.Attach(pidl, false);
            }
        }
    }

    return std::move(rtnIdl);
}
