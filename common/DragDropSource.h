#pragma once


template<class T>
class CDropSource : public IDropSource
{
public:
    CDropSource() { m_lRefCount = 1; }
    virtual ~CDropSource() {  }

    // IUnknown interface
    HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject)
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

    //IDropSource interface
    HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
    {
        // if the <Escape> key has been pressed since the last call, cancel the drop
        if (fEscapePressed == TRUE)
            return DRAGDROP_S_CANCEL;

        // if the <LeftMouse> button has been released, then do the drop!
        if ((grfKeyState & MK_LBUTTON) == 0)
            return DRAGDROP_S_DROP;

        // continue with the drag-drop
        return S_OK;
    }

    HRESULT __stdcall GiveFeedback(DWORD dwEffect)
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }

    HRESULT DoDragDrop(DWORD dwOKEffects, LPDWORD pdwEffect)
    {
        T* pT = static_cast<T*>(this);

        return DRAGDROP_S_DROP;
    }
protected:
    LONG  m_lRefCount;
};

