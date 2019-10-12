#pragma once

#include "DataBuffer.h"

class CDropSource;

class IDragSourceCallback
{
public:
    virtual bool QueryContinueDrag(const POINT *ptMouse, BOOL fEscapePressed, DWORD grfKeyState) = 0;
    virtual bool GiveFeedback(const POINT* ptMouse, DWORD dwEffect) = 0;
};

//, public IDropSourceNotify 
class CDropSource : public IDropSource
{
public:
    CDropSource();
    CDropSource(HWND hSrcWnd);
    virtual ~CDropSource();

    void SetDataObject(IDataObject* pDataObject) { m_pDataObject = pDataObject; }
    void SetDragSourceWnd(HWND hWnd) { m_hSourceWnd = hWnd; }
    void SetCallback(IDragSourceCallback* pCallback) { m_pSourceCallback = pCallback; }
    HRESULT DoDragDropEx(DWORD dwOKEffects, LPDWORD pdwEffect);
    bool InitializeFromWindow(HWND hWnd, POINT* pPoint);
    bool InitializeFromBitmap(HBITMAP hBitmap, const POINT* pPoint, COLORREF clr);

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    //IDropSource interface
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

protected:
    LONG  m_lRefCount;
    HWND m_hSourceWnd;
    IDataObject* m_pDataObject;
    IDragSourceHelper* m_pDragSourceHelper;
    IDragSourceCallback* m_pSourceCallback;
};

