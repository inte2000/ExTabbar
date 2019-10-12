#pragma once


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
        CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&m_pDropTargetHelper));

        HRESULT hr = ::RegisterDragDrop(hWnd, this);
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

        // does the dataobject contain data we want?
        m_bAllowDrop = pT->IsDragAccepted(pDataObj);
        if (m_bAllowDrop)
        {
            ATLTRACE(_T("CDropTarget::DragEnter(DataObject=%p) AllowDrop with m_hGropREgWnd=0x%x\n"),
                                                               m_pDataObject, m_hTargetWnd);
            m_pDataObject = pDataObj;
            // get the dropeffect based on keyboard state
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
            pT->SetFocus();
            pT->OnTargetDragEnter(pDataObj, &pt, grfKeyState);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        if (m_pDropTargetHelper != nullptr)
        {
            POINT ppt = { pt.x, pt.y };
            m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, &ppt, *pdwEffect);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave()
    {
        T* pT = static_cast<T*>(this);

        ATLTRACE(_T("CDropTarget::DragLeave(DataObject=%p) m_hGropREgWnd=0x%x\n"), m_pDataObject, m_hTargetWnd);
        
        if (m_bAllowDrop)
        {
            pT->OnTargetDragLeave();
            m_pDataObject = nullptr;
        }

        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->DragLeave();
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        POINT ppt = { pt.x, pt.y };
        T* pT = static_cast<T*>(this);

        ATLTRACE(_T("CDropTarget::DragOver(DataObject=%p) m_hGropREgWnd=0x%x, grfKeyState=%d, pt={%d, %d}, *pdwEffect=%d\n"),
            m_pDataObject, m_hTargetWnd, grfKeyState, pt.x, pt.y, *pdwEffect);

        if (m_bAllowDrop)
        {
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
            //PositionCursor(GetWnd(), pt);
            pT->OnTargetDragOver(&ppt, grfKeyState);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->DragOver(&ppt, *pdwEffect);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        POINT ppt = { pt.x, pt.y };
        //PositionCursor(GetWnd(), pt);
        if (m_bAllowDrop)
        {
            T* pT = static_cast<T*>(this);
            //pT->DropData(GetWnd(), pDataObj);
            pT->OnTargetDropData(pDataObj, grfKeyState);
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        if (m_pDropTargetHelper != nullptr)
        {
            m_pDropTargetHelper->Drop(pDataObj, &ppt, *pdwEffect);
        }
        ATLTRACE(_T("CDropTarget::Drop() m_hGropREgWnd=0x%x, grfKeyState=%d, pt={%d, %d}, *pdwEffect=%d\n"),
            m_hTargetWnd, grfKeyState, pt.x, pt.y, *pdwEffect);
        return S_OK;
    }

protected:
    DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
    {
        DWORD dwEffect = 0;

        // 1. check "pt" -> do we allow a drop at the specified coordinates?
        T* pT = static_cast<T*>(this);
        //if (!pT->CanFropHere(&pt))
        //    return DROPEFFECT_NONE;

        // 2. work out that the drop-effect should be based on grfKeyState
        if (grfKeyState & MK_CONTROL)
        {
            dwEffect = dwAllowed & DROPEFFECT_COPY;
        }
        else if (grfKeyState & MK_SHIFT)
        {
            dwEffect = dwAllowed & DROPEFFECT_MOVE;
        }

        // 3. no key-modifiers were specified (or drop effect not allowed), so
        //    base the effect on those allowed by the dropsource
        if (dwEffect == 0)
        {
            if (dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
            if (dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
        }

        return dwEffect;
    }

protected:
    HWND m_hTargetWnd;
    LONG m_lRefCount;
    bool m_bAllowDrop;
    IDataObject* m_pDataObject;
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
