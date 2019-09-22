#pragma once


template<class T>
class CDropSource : public IDropSource
{
public:
    CDropSource() { m_lRefCount = 1; m_hCurTargetWnd = NULL; }
    virtual ~CDropSource() {  }

    HRESULT DoDragDropEx(IDataObject* pDataObject, DWORD dwOKEffects, LPDWORD pdwEffect)
    {
        T* pT = static_cast<T*>(this);

        HRESULT res = ::DoDragDrop(pDataObject, this, dwOKEffects, pdwEffect);

        return res;// DRAGDROP_S_DROP;
    }

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject)
    {
        if (iid == IID_IDropSource || iid == IID_IUnknown)
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

    //IDropSource interface
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
    {
        ATLTRACE(_T("CDropSource::QueryContinueDrag() grfKeyState=%d, fEscapePressed=%d"),
                       grfKeyState, fEscapePressed);

        // if the <Escape> key has been pressed since the last call, cancel the drop
        if (fEscapePressed == TRUE)
            return DRAGDROP_S_CANCEL;

        // if the <LeftMouse> button has been released, then do the drop!
        if ((grfKeyState & MK_LBUTTON) == 0)
            return DRAGDROP_S_DROP;

        // continue with the drag-drop
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect)
    {
        ATLTRACE(_T("CDropSource::GiveFeedback() dwEffect=%d"), dwEffect);
        T* pT = static_cast<T*>(this);

        return DRAGDROP_S_USEDEFAULTCURSORS;
    }

    //IDropSourceNotify interface
    HRESULT STDMETHODCALLTYPE DragEnterTarget(HWND hwndTarget)
    {
        ATLTRACE(_T("CDropSource::DragEnterTarget() hwndTarget=%x"), hwndTarget);
        m_hCurTargetWnd = hwndTarget;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeaveTarget()
    {
        ATLTRACE(_T("CDropSource::DragLeaveTarget() hwndTarget=%x"), hwndTarget);
        m_hCurTargetWnd = NULL;
        return S_OK;
    }

protected:
    LONG  m_lRefCount;
    HWND m_hCurTargetWnd;
};

