#include "pch.h"
#include "SystemFunctions.h"
#include "DebugLog.h"
#include "DataObject.h"
#include "DragDropData.h"
#include "ShellTabItem.h"
#include "ShellBrowserEx.h"
#include "ExplorerWindow.h" //
#include "WspFunctions.h"
#include "ShellFunctions.h"
#include "TeTabctrl.h"
#include "AppConfig.h"

bool CTeTabCtrl::PrepareDataObject(int nItem, const POINT& pt, IDataObject** ppDataObject)
{
    FORMATETC fmtetc = { CF_PRV_TETAB_DRAG, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;

    *ppDataObject = nullptr;

    stgmed.hGlobal = ::GlobalAlloc(GHND | GMEM_SHARE, sizeof(TeTabDragDrop));
    if (stgmed.hGlobal == NULL)
        return false;

    TeTabDragDrop *pDropData = (TeTabDragDrop *)::GlobalLock(stgmed.hGlobal);
    if (pDropData == nullptr)
    {
        ::GlobalFree(stgmed.hGlobal);
        return false;
    }

    pDropData->nDragItem = GetCurSel();
    GetItemId(nItem, pDropData->DragItemId);
    pDropData->hSourceWnd = m_hWnd;
    CShellTabItem* pTabItem = (CShellTabItem*)GetItemData(pDropData->nDragItem);
    WStrFromTString(pDropData->wcUrl, MAX_PATH, pTabItem->GetUrl());

    auto rtn = pTabItem->GetCurrentIDLData();
    pDropData->idlSize = 0;
    if (std::get<1>(rtn) <= 512)
    {
        ::CopyMemory(pDropData->idlData, std::get<0>(rtn), std::get<1>(rtn));
        pDropData->idlSize = std::get<1>(rtn);
    }

    ::GlobalUnlock(stgmed.hGlobal);
    stgmed.tymed = TYMED_HGLOBAL;
    stgmed.pUnkForRelease = nullptr;

    HRESULT hr = CreateDataObject(&fmtetc, &stgmed, 1, ppDataObject);
    if (hr == S_OK)
        return true;

    return false;
}

bool CTeTabCtrl::QueryContinueDrag(const POINT* ptMouse, BOOL fEscapePressed, DWORD grfKeyState)
{
    HWND wnd = ::WindowFromPoint(*ptMouse);

    if (wnd != m_hWnd)
    {
        return false;
    }

    return true;
}

bool CTeTabCtrl::GiveFeedback(const POINT* ptMouse, DWORD dwEffect)
{
    HWND wnd = ::WindowFromPoint(*ptMouse);

    if (wnd != m_hWnd)
    {
        return false;
    }

    return true;
}

bool CTeTabCtrl::IsDragAccepted(IDataObject* pDataObj)
{
    ATLTRACE(_T("CTeTabCtrl::IsDragAccepted invoked!\n"));
    FORMATETC fmtetc = { CF_PRV_TETAB_DRAG, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // does the data object support CF_TEXT using a HGLOBAL?
    return pDataObj->QueryGetData(&fmtetc) == S_OK ? true : false;
}

void CTeTabCtrl::OnTargetDragEnter(IDataObject* pDataObj, const POINTL* pt, DWORD grfKeyState)
{
    ATLTRACE(_T("CTeTabCtrl::OnTargetDragEnter(IDataObject=%p) invoked!\n"), pDataObj);
}

void CTeTabCtrl::OnTargetDropData(IDataObject* pDataObj, DWORD grfKeyState)
{
    ATLTRACE(_T("CTeTabCtrl::OnTargetDropData(IDataObject=%p) invoked!\n"), pDataObj);
}

void CTeTabCtrl::OnTargetDragOver(const POINT* pt, DWORD grfKeyState)
{
    ATLTRACE(_T("CTeTabCtrl::OnTargetDragOver(m_pDataObject=%p, x=%d,y=%d,grfKeyState=%x) invoked!\n"), m_pDataObject, pt->x, pt->y, grfKeyState);
    POINT ptMouse = *pt;
    ::ScreenToClient(m_hTargetWnd, &ptMouse);
    RECT rcItem = { 0 };
    int nItem = HitTestForDrag(&ptMouse, &rcItem);
    ATLTRACE(_T("CTeTabCtrl::OnTargetDragOver(x=%d,y=%d,grfKeyState=%x, hittest=%d) invoked!\n"), ptMouse.x, ptMouse.y, grfKeyState, nItem);
    if ((nItem >= 0) && (nItem != m_iDragItem))
    {
        int wrange = (rcItem.right - rcItem.left) / 5;
        if ( ((nItem > m_iDragItem) && ((ptMouse.x - rcItem.left) >= wrange))
            || ((nItem < m_iDragItem) && ((rcItem.right - ptMouse.x) >= wrange)) )
        {
            SwapItemPositions(m_iDragItem, nItem, false, false);
            RedrawWindow();
        }
    }
}

void CTeTabCtrl::OnTargetDragLeave()
{
    ATLASSERT(m_pDataObject != nullptr);
    ATLTRACE(_T("CTeTabCtrl::OnTargetDragLeave(m_pDataObject=%p) invoked!\n"), m_pDataObject);
}

BOOL CTeTabCtrl::AddNewTab(const TString& path)
{
    CShellTabItem* psti = new CShellTabItem();
    if (psti != nullptr)
    {
        CIDLEx cidl;
        CIDListData IdlData;
        if (!GetCIDLDataByParseName(path, cidl, IdlData))
        {
        }

        int nInsertItem;
        if (g_bNewTabInsertBegin)
            nInsertItem = 0;
        else
            nInsertItem = GetItemCount();

        psti->NavigatedTo(IdlData, cidl, path);
        int iconIdx = GetShellObjectIcon(cidl);

        bool bSelected = g_bSwitchNewTab ? true : false;
        int index = InternalInsertItem(nInsertItem, psti->GetTitle().c_str(), iconIdx, psti->GetTooltip().c_str(), (ULONG_PTR)psti, bSelected);

        if (index < 0)
        {
            delete psti;
            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

LRESULT CTeTabCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LogTrace(_T("CTeTabCtrl::OnCreate() invoked"));

    HIMAGELIST hSysImgList = GetSystemImageList(FALSE);
    if (hSysImgList == NULL)
    {
        LogError(_T("CTeTabCtrl::OnCreate() get system imagelist failed!"));
        return 0;
    }

    m_sysImgList.Attach(hSysImgList);
    CImageList oldImgList = SetImageList(hSysImgList);
    if(!oldImgList.IsNull())
        oldImgList.Destroy();

    RegisterDropTarget(m_hWnd);

    bHandled = FALSE;
    return 0;
}

LRESULT CTeTabCtrl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HIMAGELIST hSysImgList = m_sysImgList.Detach();
    if (hSysImgList != NULL)
        ::ImageList_Destroy(hSysImgList);
    
    DeregisterDropTarget();
    bHandled = FALSE;
    return 0;
}

/*
LRESULT CTeTabCtrl::OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTC2ITEMS* pNmItem = (NMCTC2ITEMS*)pnmh;
    return 0;
}
*/
LRESULT CTeTabCtrl::OnBeginItemDrag(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTCITEM *pItem = (NMCTCITEM *)pnmh;
    
    CDropSource dropsrc(m_hWnd);
    dropsrc.SetCallback(this);
    IDataObject* pDataObject = nullptr;
    if (PrepareDataObject(m_iDragItem, pItem->pt, &pDataObject))
    {
        dropsrc.SetDataObject(pDataObject);
        POINT ptOffset = pItem->pt;
        HBITMAP dragImg = GetItemDragImage(pItem->iItem, &ptOffset, nullptr);
        if (dragImg != NULL)
        {
            dropsrc.InitializeFromBitmap(dragImg, &ptOffset, RGB(0, 0, 0));
        }

        DWORD dwEffect = 0;
        HRESULT rtn = dropsrc.DoDragDropEx(DROPEFFECT_MOVE/* | DROPEFFECT_COPY*/, &dwEffect);
        ATLTRACE(_T("DoDragDropEx() rtn = %x, dwEffect=%x\n"), rtn, dwEffect);
        if (rtn == DRAGDROP_S_DROP)
        {
            FORMATETC fmtetc = { CF_PRV_TETAB_DRAG, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stgmed;

            CIDLEx cidl;
            if (pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
            {
                // we asked for the data as a HGLOBAL, so access it appropriately
                //SetWindowText(hwnd, (char*)data);
                ::GlobalUnlock(stgmed.hGlobal);
                ::ReleaseStgMedium(&stgmed);
            }
        }
        SetItemDragDrop(m_iDragItem, false);
        m_iDragItem = -1;

        pDataObject->Release();
    }

    return 1;
}

LRESULT CTeTabCtrl::OnNewTabButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    NMCTCITEM* pNtItem = reinterpret_cast<NMCTCITEM*>(pnmh);
    //if(pNtItem->pt.x, pNtItem->pt.y)
    TString path = GetMyComputerPath();

    if (!AddNewTab(path))
    {
        ::MessageBox(m_hWnd, _T("add new tab fail!"), _T("TabExplorer"), MB_OK);
    }

    return 0;
}


LRESULT CTeTabCtrl::OnCloseButton(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    ATLASSERT(m_pExplorerWnd != nullptr);

    NMCTCITEM* pNtItem = reinterpret_cast<NMCTCITEM*>(pnmh);

    if (pNtItem->iItem >= 0)
    {
        int nItemCount = GetItemCount();
        if (nItemCount == 1) //last tab，should give an alert to close explorer window?
        {
        }
        InternalRemoveItem(pNtItem->iItem);
        if (GetItemCount() <= 0)
            m_pExplorerWnd->CloseExplorerWindow();
    }

    return 0;
}

BOOL CTeTabCtrl::InternalRemoveItem(int nItem)
{
    int nItemCount = GetItemCount();
    if ((nItem < 0) || (nItem >= nItemCount))
        return FALSE;

    if (m_iDragItem == nItem)
        m_iDragItem = -1;

    if (m_iDragItemOriginal == nItem)
        m_iDragItemOriginal = -1;

    int nCurSel = GetCurSel();
    if (nCurSel == nItem)
    {
        if (nItem == (nItemCount - 1))
            SetCurSel(nItem - 1);
        else
            SetCurSel(nItem + 1);
    }
    
    return DeleteItem(nItem);
}

int CTeTabCtrl::InternalInsertItem(int nItem, LPCTSTR sText, int nImage, LPCTSTR sToolTip, ULONG_PTR data, bool bSelectItem)
{
    int index = InsertItem(nItem, sText, nImage, sToolTip, false);
    if (index >= 0)
    {
        SetItemData(index, data);
        if(bSelectItem)
            SetCurSel(index);
    }

    return index;
}


/*
int InsertItem(int nItem, LPCTSTR sText = NULL, int nImage = -1, LPCTSTR sToolTip = NULL, bool bSelectItem = false)
SetItemDrag();
*/

