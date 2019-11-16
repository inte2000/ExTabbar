#pragma once

#include "DataObjectWrapper.h"

template<class T>
class CDropTarget : public IDropTarget
{
public:
    CDropTarget() 
    { 
        m_hTargetWnd = NULL;
        m_lRefCount = 1;
        m_bAllowDrop = false;
        m_pDropTargetHelper = nullptr;
    }

    virtual ~CDropTarget() 
    { 
        DeregisterDropTarget();
    }

    bool RegisterDropTarget(HWND hWnd)
    {
        HRESULT hr = ::CoCreateInstance(CLSID_DragDropHelper, NULL, 
                                        CLSCTX_INPROC, IID_PPV_ARGS(&m_pDropTargetHelper));
        if (!SUCCEEDED(hr))
            return false;

        hr = ::RegisterDragDrop(hWnd, this);
        if (hr == S_OK)
        {
            m_hTargetWnd = hWnd;
            return true;
        }

        return false;
    }

    void DeregisterDropTarget()
    {
        if (m_hTargetWnd != NULL)
        {
            ::RevokeDragDrop(m_hTargetWnd);
            m_hTargetWnd = NULL;
        }
    }

    DWORD OnTargetDragEnter(CDataObjectWrapper& DataObj, const POINT& pt, DWORD grfKeyState)
    {
        return DROPEFFECT_NONE;
    }
    void OnTargetDragLeave() {}
    DWORD OnTargetDragOver(CDataObjectWrapper& DataObj, const POINT& pt, DWORD grfKeyState)
    {
        return DROPEFFECT_NONE;
    }
    DWORD OnTargetDropData(CDataObjectWrapper& DataObj, const POINT& pt, DWORD grfKeyState, DWORD dwEffect)
    {
        return DROPEFFECT_NONE;
    }

    //void SetSourceWnd(HWND hWnd) {}

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject)
    {
        if (iid == IID_IDropTarget || iid == IID_IUnknown)
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        else
        {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }
    }

    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        InterlockedIncrement(&m_lRefCount);
        return m_lRefCount;
    }

    ULONG STDMETHODCALLTYPE Release(void)
    {
        ULONG ulRefCount = InterlockedDecrement(&m_lRefCount);

        if (ulRefCount == 0)
        {
            delete this;
        }

        return ulRefCount;
    }

    //IDropTarget interface
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        T* pT = static_cast<T*>(this);

        ATLTRACE(_T("CDropTarget::DragEnter(DataObject=%p) m_hGropREgWnd=0x%x, grfKeyState=%d, pt={%d, %d}, *pdwEffect=%d\n"),
            pDataObj, m_hTargetWnd, grfKeyState, pt.x, pt.y, *pdwEffect);
        m_DataObject.Release();
        m_DataObject.Attach(pDataObj, true);
        POINT ppt = { pt.x, pt.y };
        ::ScreenToClient(m_hTargetWnd, &ppt);
        DWORD dropEffect = pT->OnTargetDragEnter(m_DataObject, ppt, grfKeyState);

        // get the dropeffect based on keyboard state
        *pdwEffect = DropEffect(dropEffect, *pdwEffect);
        //pT->SetFocus();

        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, &ppt, *pdwEffect);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave()
    {
        T* pT = static_cast<T*>(this);

        ATLTRACE(_T("CDropTarget::DragLeave() m_hGropREgWnd=0x%x\n"), m_hTargetWnd);
        
        pT->OnTargetDragLeave();

        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->DragLeave();
        }
        m_DataObject.Release();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        POINT ppt = { pt.x, pt.y };
        T* pT = static_cast<T*>(this);

        ::ScreenToClient(m_hTargetWnd, &ppt);
        ATLASSERT(!m_DataObject.IsEmpty());
        ATLTRACE(_T("CDropTarget::DragOver() m_hGropREgWnd=0x%x, grfKeyState=%d, pt={%d, %d}, *pdwEffect=%d\n"),
            m_hTargetWnd, grfKeyState, pt.x, pt.y, *pdwEffect);

        DWORD dropEffect = pT->OnTargetDragOver(m_DataObject, ppt, grfKeyState);
        *pdwEffect = DropEffect(dropEffect, *pdwEffect);
        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->DragOver(&ppt, *pdwEffect);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        T* pT = static_cast<T*>(this);

        ATLASSERT(!m_DataObject.IsEmpty());

        ATLTRACE(_T("CDropTarget::Drop() m_hGropREgWnd=0x%x, grfKeyState=%d, pt={%d, %d}, *pdwEffect=%d\n"),
            m_hTargetWnd, grfKeyState, pt.x, pt.y, *pdwEffect);
        
        POINT ppt = { pt.x, pt.y };
        ::ScreenToClient(m_hTargetWnd, &ppt);

        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->Drop(pDataObj, &ppt, *pdwEffect);
        }
        
        //PositionCursor(GetWnd(), pt);
        DWORD dropEffect = DropEffect(pT->OnTargetDragOver(m_DataObject, ppt, grfKeyState), *pdwEffect);
        if (dropEffect != DROPEFFECT_NONE)
        {
            pT->OnTargetDropData(m_DataObject, ppt, grfKeyState, dropEffect);
        }
        else
        {
            pT->OnTargetDragLeave();
        }

        *pdwEffect = dropEffect;
        m_DataObject.Release();

        return S_OK;
    }

protected:
    DWORD DropEffect(DWORD dropEffect, DWORD dwEffects)
    {
        // return allowed dropEffect and DROPEFFECT_NONE
        if ((dropEffect & dwEffects) != 0)
            return dropEffect;

        // map common operations (copy/move) to alternates, but give negative
        //  feedback for DROPEFFECT_LINK.
        switch (dropEffect)
        {
        case DROPEFFECT_COPY:
            if (dwEffects & DROPEFFECT_MOVE)
                return DROPEFFECT_MOVE;
            else if (dwEffects & DROPEFFECT_LINK)
                return DROPEFFECT_LINK;
            break;

        case DROPEFFECT_MOVE:
            if (dwEffects & DROPEFFECT_COPY)
                return DROPEFFECT_COPY;
            else if (dwEffects & DROPEFFECT_LINK)
                return DROPEFFECT_LINK;
            break;

        case DROPEFFECT_LINK:
            break;
        }

        return DROPEFFECT_NONE;
    }

protected:
    HWND m_hTargetWnd;
    LONG m_lRefCount;
    bool m_bAllowDrop;
    CDataObjectWrapper m_DataObject;
    IDropTargetHelper* m_pDropTargetHelper;
};

/*
void DropData(HWND hwnd, IDataObject *pDataObject)
{
    // construct a FORMATETC object
    FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;

    // See if the dataobject contains any TEXT stored as a HGLOBAL
    if(pDataObject->QueryGetData(&fmtetc) == S_OK)
    {
        // Yippie! the data is there, so go get it!
        if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
        {
            // we asked for the data as a HGLOBAL, so access it appropriately
            PVOID data = GlobalLock(stgmed.hGlobal);

            SetWindowText(hwnd, (char *)data);

            GlobalUnlock(stgmed.hGlobal);

            // release the data using the COM API
            ReleaseStgMedium(&stgmed);
        }
    }
}*/
