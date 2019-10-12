#include "pch.h"
#include "SystemFunctions.h"
#include "DragDropSource.h"

CDropSource::CDropSource() 
{ 
    m_lRefCount = 1; 
    m_hSourceWnd = NULL;
    m_pSourceCallback = nullptr;
    m_pDataObject = nullptr;
    /*
        CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (LPVOID*)&m_pdsh);
    */
    ::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pDragSourceHelper));
}

CDropSource::CDropSource(HWND hSrcWnd) : CDropSource()
{
    m_hSourceWnd = hSrcWnd;
}

CDropSource::~CDropSource()
{
    if (m_pDragSourceHelper != nullptr)
        m_pDragSourceHelper->Release();
}

HRESULT CDropSource::DoDragDropEx(DWORD dwOKEffects, LPDWORD pdwEffect)
{
    ATLASSERT(m_pDataObject != nullptr);

    HRESULT res = ::DoDragDrop(m_pDataObject, this, dwOKEffects, pdwEffect);

    return res;// DRAGDROP_S_DROP;
}

//  static_cast<LPDATAOBJECT>(GetInterface(&IID_IDataObject)));
bool CDropSource::InitializeFromWindow(HWND hWnd, POINT* pPoint)
{
    ATLASSERT(m_pDataObject != nullptr);

    if (m_pDragSourceHelper == nullptr)
        return false;

    HRESULT hr = m_pDragSourceHelper->InitializeFromWindow(hWnd, pPoint, m_pDataObject);
    return SUCCEEDED(hr);
}

/*take the ownership*/
bool CDropSource::InitializeFromBitmap(HBITMAP hBitmap, const POINT* pPoint, COLORREF clr)
{
    ATLASSERT(m_pDataObject != nullptr);

    if ((hBitmap == NULL) || (m_pDragSourceHelper == nullptr))
        return false;

    BITMAP bm = { 0 };
    if (::GetObject(hBitmap, sizeof(BITMAP), &bm) == 0)
        return false;

    SHDRAGIMAGE di;
    di.sizeDragImage.cx = bm.bmWidth;
    di.sizeDragImage.cy = bm.bmHeight;
    if (pPoint)
        di.ptOffset = *pPoint;
    else
    {
        di.ptOffset.x = di.sizeDragImage.cx >> 1;
        di.ptOffset.y = di.sizeDragImage.cy >> 1;
    }
    di.hbmpDragImage = hBitmap;
    di.crColorKey = (CLR_INVALID == clr) ? ::GetSysColor(COLOR_WINDOW) : clr;
    HRESULT hr = m_pDragSourceHelper->InitializeFromBitmap(&di, m_pDataObject);

    if (FAILED(hr) && hBitmap)
        ::DeleteObject(hBitmap);                    // delete image to avoid a memory leak
    
    return SUCCEEDED(hr);
}

// IUnknown interface
HRESULT STDMETHODCALLTYPE CDropSource::QueryInterface(REFIID iid, void** ppvObject)
{
//    if ((iid == IID_IDropSource) || (iid == IID_IDropSourceNotify) || (iid == IID_IUnknown))
    if ((iid == IID_IDropSource) || (iid == IID_IUnknown))
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

ULONG STDMETHODCALLTYPE CDropSource::AddRef(void)
{
    InterlockedIncrement(&m_lRefCount);
    return m_lRefCount;
}
    
ULONG STDMETHODCALLTYPE CDropSource::Release(void)
{
    ULONG ulRefCount = InterlockedDecrement(&m_lRefCount);
    if (ulRefCount == 0)
    {
        delete this;
    }

    return ulRefCount;
}

//IDropSource interface
HRESULT STDMETHODCALLTYPE CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    POINT ptMouse = { 0 };
    ::GetCursorPos(&ptMouse);

    ATLTRACE(_T("CDropSource::QueryContinueDrag() m_hSourceWnd=0x%x, grfKeyState=%d, fEscapePressed=%d\n"),
                m_hSourceWnd, grfKeyState, fEscapePressed);

    if (m_pSourceCallback)
    {
        if (!m_pSourceCallback->QueryContinueDrag(&ptMouse, fEscapePressed, grfKeyState))
            return DRAGDROP_S_CANCEL;
    }

    // if the <Escape> key has been pressed since the last call, cancel the drop
    if (fEscapePressed == TRUE)
        return DRAGDROP_S_CANCEL;

    // if the <LeftMouse> button has been released, then do the drop!
    if ((grfKeyState & MK_LBUTTON) == 0)
        return DRAGDROP_S_DROP;

    // continue with the drag-drop
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropSource::GiveFeedback(DWORD dwEffect)
{
    ATLTRACE(_T("CDropSource::GiveFeedback() dwEffect=%x\n"), dwEffect);

    POINT ptMouse = { 0 };
    ::GetCursorPos(&ptMouse);

    if (m_pSourceCallback)
        m_pSourceCallback->GiveFeedback(&ptMouse, dwEffect);

    return DRAGDROP_S_USEDEFAULTCURSORS;
}
