#pragma once


template<class T>
class CDropTarget : public IDropTarget
{
public:
    CDropTarget() 
    { 
        m_hGropREgWnd = NULL; 
        m_lRefCount = 1;
        m_bAllowDrop = false;
    }

    virtual ~CDropTarget() 
    { 
        Deregister(); 
    }

    bool Register(HWND hWnd)
    {
        HRESULT hr = ::RegisterDragDrop(hWnd, this);
        if (hr == S_OK)
        {
            m_hGropREgWnd = hWnd;
            return true;
        }

        return false;
    }

    void Deregister()
    {
        if (m_hGropREgWnd != NULL)
        {
            ::RevokeDragDrop(m_hGropREgWnd);
            m_hGropREgWnd = NULL;
        }
    }


    // IUnknown interface
    HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject)
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

    ULONG   __stdcall AddRef(void)
    {
        InterlockedIncrement(&m_lRefCount);
        return m_lRefCount;
    }

    ULONG   __stdcall Release(void)
    {
        ULONG ulRefCount = InterlockedDecrement(&m_lRefCount);

        if (ulRefCount == 0)
        {
            delete this;
        }

        return ulRefCount;
    }

    //IDropTarget interface
    HRESULT __stdcall DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        // does the dataobject contain data we want?
        m_bAllowDrop = QueryDataObject(pDataObj);
        if (m_bAllowDrop)
        {
            T* pT = static_cast<T*>(this);
            // get the dropeffect based on keyboard state
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
            pT->SetFocus();
            //PositionCursor(GetWnd(), pt);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        return S_OK;
    }

    HRESULT __stdcall DragLeave()
    {
        return S_OK;
    }

    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        if (m_bAllowDrop)
        {
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
            //PositionCursor(GetWnd(), pt);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        return S_OK;
    }

    HRESULT __stdcall Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        //PositionCursor(GetWnd(), pt);
        if (m_bAllowDrop)
        {
            T* pT = static_cast<T*>(this);
            //pT->DropData(GetWnd(), pDataObj);
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

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

    bool QueryDataObject(IDataObject* pDataObject)
    {
        FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

        // does the data object support CF_TEXT using a HGLOBAL?
        return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
    }

protected:
    HWND m_hGropREgWnd;
    LONG m_lRefCount;
    bool m_bAllowDrop;
    IDataObject* m_pDataObject;
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