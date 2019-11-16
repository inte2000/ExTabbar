#pragma once

#include "DataBuffer.h"
#include "DataObject.h"


//, public IDropSourceNotify 
class CDropSource : public IDropSource
{
public:
    CDropSource();
    CDropSource(HWND hSrcWnd);
    virtual ~CDropSource();

    void SetDataObject(IDataObject* pDataObject) { m_pDataObject = pDataObject; }
    void SetDragSourceWnd(HWND hWnd) { m_hSourceWnd = hWnd; }
    bool SetDragImageCursor(DWORD dwEffect);
    virtual HRESULT DoDragDropEx(POINT* point, DWORD dwOKEffects, LPDWORD pdwEffect);
    virtual HRESULT OnQueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    virtual HRESULT OnGiveFeedback(DWORD dwEffect);
    bool InitializeFromWindow(HWND hWnd, POINT* pPoint);
    bool InitializeFromBitmap(HBITMAP hBitmap, const POINT* pPoint, COLORREF clr);
    bool BeginDrag(POINT* point, HWND hWnd = NULL);

    // IUnknown interface
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    //IDropSource interface
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

protected:
    LONG  m_lRefCount;
    bool m_bDragStarted;
    HWND m_hSourceWnd;
    IDataObject* m_pDataObject;
    IDragSourceHelper* m_pDragSourceHelper;
};

