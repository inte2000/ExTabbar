#include "pch.h"
#include "SystemFunctions.h"
#include "DragDropSource.h"
#include "DragDropHelper.h"

// Must set this definition to include OEM constants from winuser.h
#ifndef OCR_NORMAL
#error Add '#define OEMRESOURCE' on top of stdafx.h
#endif

#ifndef DDWM_UPDATEWINDOW
#define DDWM_UPDATEWINDOW	(WM_USER + 3)
#endif
// This message is undocumented.
// If no drop description data object exists, the WPARAM
//  parameter specifies the cursor type and the system default text is shown
#define DDWM_SETCURSOR		(WM_USER + 2)

CDropSource::CDropSource() 
{ 
    m_lRefCount = 1; 
    m_hSourceWnd = NULL;
    m_pDataObject = nullptr;
    m_bDragStarted = false;
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

bool CDropSource::SetDragImageCursor(DWORD dwEffect)
{
    ATLASSERT(m_pDataObject != nullptr);
    // Stored data is always a DWORD even with 64-bit apps.
    HWND hWnd = (HWND)ULongToHandle(GetGlobalDataDWord(m_pDataObject, _T("DragWindow")));
    if (hWnd)
    {
        WPARAM wParam = 0;								// Use DropDescription to get type and optional text
        switch (dwEffect & ~DROPEFFECT_SCROLL)
        {
        case DROPEFFECT_NONE: wParam = 1; break;
        case DROPEFFECT_COPY: wParam = 3; break;		// The order is not as for DROPEEFECT values!
        case DROPEFFECT_MOVE: wParam = 2; break;
        case DROPEFFECT_LINK: wParam = 4; break;
        }
        ::SendMessage(hWnd, DDWM_SETCURSOR, wParam, 0);
    }
    return NULL != hWnd;
}

HRESULT CDropSource::DoDragDropEx(POINT* point, DWORD dwOKEffects, LPDWORD pdwEffect)
{
    ATLASSERT(m_pDataObject != nullptr);

    if (!BeginDrag(point, m_hSourceWnd))
        return DROPEFFECT_NONE;

    HRESULT res = ::DoDragDrop(m_pDataObject, this, dwOKEffects, pdwEffect);

    return res;// DRAGDROP_S_DROP;
}

HRESULT CDropSource::OnQueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    // if the <Escape> key has been pressed since the last call, cancel the drop
    if (fEscapePressed == TRUE)
    {
        m_bDragStarted = false;
        return DRAGDROP_S_CANCEL;
    }

    // if the <LeftMouse> button has been released, then do the drop!
    if ((grfKeyState & MK_LBUTTON) == 0)
        return m_bDragStarted ? DRAGDROP_S_DROP : DRAGDROP_S_CANCEL;

    // continue with the drag-drop
    return S_OK;
}

HRESULT CDropSource::OnGiveFeedback(DWORD dwEffect)
{
    ULONG rcCode = m_bDragStarted ? DRAGDROP_S_USEDEFAULTCURSORS : S_OK;

    return rcCode;
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

bool CDropSource::BeginDrag(POINT* point, HWND hWnd)
{
    m_bDragStarted = false;

    if (hWnd == NULL)
        hWnd = m_hSourceWnd;
    
    if (hWnd == NULL)
        return m_bDragStarted;

    DWORD dwLastTick = ::GetTickCount();
    ::SetCapture(hWnd);

    while (!m_bDragStarted)
    {
        // some applications steal capture away at random times
        if (::GetCapture() != hWnd)
            break;

        // peek for next input message
        MSG msg;
        if (::PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) ||
            ::PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
        {
            // check for button cancellation (any button down will cancel)
            if (msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONUP ||
                msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
                break;

            // check for keyboard cancellation
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
                break;

            // check for drag start transition
            m_bDragStarted = (point->x != msg.pt.x) || (point->y != msg.pt.y);
        }

        // if the user sits here long enough, we eventually start the drag
        if (::GetTickCount() - dwLastTick > 0)
            m_bDragStarted = true;
    }
    ::ReleaseCapture();

    return m_bDragStarted;
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

    return OnQueryContinueDrag(fEscapePressed, grfKeyState);
}

HRESULT STDMETHODCALLTYPE CDropSource::GiveFeedback(DWORD dwEffect)
{
    ATLTRACE(_T("CDropSource::GiveFeedback() dwEffect=%x\n"), dwEffect);

    POINT ptMouse = { 0 };
    ::GetCursorPos(&ptMouse);

    return OnGiveFeedback(dwEffect);
}
