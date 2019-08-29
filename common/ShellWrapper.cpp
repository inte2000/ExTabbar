#include "pch.h"
#include "ShellWrapper.h"


//////////////////////            CShellFolder              ////////////////////////
void CShellFolder::Attach(const LPSHELLFOLDER psf, BOOL bShare) 
{ 
    m_pSF = psf; 
    m_bShareMode = bShare;
}

CShellFolder& CShellFolder::operator = (const CShellFolder& sf)
{
    if (this != &sf)
    {
        this->m_pSF = sf.m_pSF;
        this->m_bShareMode = true;
    }

    return *this;
}

HRESULT CShellFolder::ParseDisplayName(LPCTSTR pszDisplayName, ULONG* pchEaten, PIDLIST_RELATIVE* ppidl, ULONG* pdwAttributes)
{
    if (m_pSF != nullptr)
    {
#ifdef UNICODE
        return m_pSF->ParseDisplayName(NULL, nullptr, (LPTSTR)pszDisplayName, pchEaten, ppidl, pdwAttributes);
#else
        WCHAR olePath[MAX_PATH] = { 0 };
        int wchars = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszDisplayName, -1, olePath, MAX_PATH);
        if (wchars == 0)
            return E_FAIL;

        return m_pSF->ParseDisplayName(NULL, nullptr, olePath, pchEaten, ppidl, pdwAttributes);
#endif
    }

    return E_FAIL;
}

HRESULT CShellFolder::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, TString& name)
{
    if (m_pSF != nullptr)
    {
        STRRET strret;
        HRESULT hr = m_pSF->GetDisplayNameOf(pidl, uFlags, &strret);
        if (hr == S_OK)
        {
            TransStrrefToString(pidl, strret, name);
        }
    }

    return E_FAIL;
}

HRESULT CShellFolder::BindToObject(PCUIDLIST_RELATIVE pidl, REFIID riid, void** ppv)
{
    return m_pSF->BindToObject(pidl, NULL, riid, ppv);
}

HRESULT CShellFolder::BindToFolder(PCUIDLIST_RELATIVE pidl, CShellFolder& sf)
{
    LPSHELLFOLDER   psfRoot;

    // get the IShellFolder of the root
    HRESULT hr = m_pSF->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*)&psfRoot);
    if (hr == S_OK)
    {
        sf.Attach(psfRoot, false);
    }

    return hr;
}

HRESULT CShellFolder::GetUIObjectOf(HWND hwndOwner, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* rgfReserved, void** ppv)
{
    return m_pSF->GetUIObjectOf(hwndOwner, cidl, apidl, riid, rgfReserved, ppv);
}

void CShellFolder::Release()
{
    if (m_pSF != nullptr)
    {
        if (!m_bShareMode)
            m_pSF->Release();
        
        m_pSF = nullptr;
    }
}


//////////////////////            CDesktopFolder              ////////////////////////
CDesktopFolder CDesktopFolder::s_DesktopFolder;

bool CDesktopFolder::Initialize()
{
    IShellFolder* psf = nullptr;
    HRESULT hr = ::SHGetDesktopFolder(&psf);
    if (hr == S_OK)
    {
        Attach(psf, false);
        return true;
    }

    return false;
}


//////////////////////            CShMem              ////////////////////////
CShMem CShMem::s_ShlMem;

bool CShMem::Initialize()
{
    IMalloc* allocator = nullptr;
    HRESULT hr = ::SHGetMalloc(&allocator);
    if (hr == S_OK)
    {
        m_pAllocator = allocator;
        return true;
    }

    return false;
}

void CShMem::Release()
{
    if (m_pAllocator != nullptr)
    {
        m_pAllocator->Release();
        m_pAllocator = nullptr;
    }
}

void* CShMem::Alloc(SIZE_T size)
{
    if (m_pAllocator != nullptr)
    {
        return m_pAllocator->Alloc(size);
    }

    return nullptr;
}

void CShMem::Free(void *pv)
{
    if (m_pAllocator != nullptr)
    {
        m_pAllocator->Free(pv);
    }
}

SIZE_T CShMem::GetSize(void* pv)
{
    if ((m_pAllocator != nullptr) && (pv != nullptr))
    {
        return m_pAllocator->GetSize(pv);
    }

    return 0;
}


//////////////////////            CIDLEx              ////////////////////////
CIDLEx::CIDLEx(CIDLEx&& idl)
{
    m_pidl = idl.m_pidl;
    m_bShareMode = idl.m_bShareMode;
    idl.m_pidl = nullptr;
    idl.m_bShareMode = true;
}

void CIDLEx::Attach(LPITEMIDLIST pidl, bool bShare)
{
    Release();

    m_bShareMode = bShare;
    if (m_bShareMode)
    {
        m_pidl = pidl;
    }
    else
    {
        m_pidl = Clone(pidl);
    }
}

LPITEMIDLIST CIDLEx::Detach()
{
    LPITEMIDLIST tmp = m_pidl;
    m_pidl = nullptr;
    m_bShareMode = true;
    return tmp;
}

HRESULT CIDLEx::Set(LPCTSTR szPath, LPSHELLFOLDER psf)
{
    CShellFolder parentSf;
    if (psf != nullptr)
        parentSf.Attach(psf, true);
    else
        parentSf = CDesktopFolder::GetDesktopFolder();

    Release();

    ULONG   chEaten;
    ULONG   dwAttributes;
    LPITEMIDLIST pidl = nullptr;
    HRESULT hr = parentSf.ParseDisplayName(szPath, &chEaten, &pidl, &dwAttributes);
    if (hr == S_OK)
    {
        m_pidl = pidl;
        m_bShareMode = false;
    }

    return hr;
}

TString CIDLEx::GetParseName() const
{
    TString name;

    if (m_pidl != nullptr)
    {
        CShellFolder sf = CDesktopFolder::GetDesktopFolder();
        TString strName;
        sf.GetDisplayNameOf(m_pidl, SHGDN_FORPARSING, name);
    }

    return std::move(name);
}

TString CIDLEx::GetDisplayName() const
{
    TString name;

    if (m_pidl != nullptr)
    {
        CShellFolder sf = CDesktopFolder::GetDesktopFolder();
        TString strName;
        sf.GetDisplayNameOf(m_pidl, SHGDN_NORMAL, name);
    }

    return std::move(name);
}

UINT CIDLEx::GetSize() const
{
    UINT        cbTotal = 0;
    LPSHITEMID  pid = GetFirstItemID();

    if (pid) {
        do {
            cbTotal += pid->cb;
            GetNextItemID(pid);
        } while (pid->cb);
        cbTotal += sizeof(pid->cb); // count the terminator
    }

    return cbTotal;
}

#define CB_SIZE  (sizeof(piid->cb))     // size of terminator

void CIDLEx::Split(CIDLEx& root, CIDLEx& obj) const
{
    int         size;
    //SHITEMID* piid, * piidLast;
    LPSHITEMID piid, piidLast;
    // find last item-id and calculate total size of pidl
    size = 0;
    piid = &m_pidl->mkid;
    piidLast = piid;
    while (piid->cb)
    {
        piidLast = piid;
        size += (piid->cb);
        piid = (SHITEMID*)((LPBYTE)piid + (piid->cb));
    }

    CShMem* shm = CShMem::GetAllocator();
    // copy "root" portion
    size -= piidLast->cb;  // don't count "object" item-id
    shm->Alloc(size + CB_SIZE);
    CopyMemory(root.m_pidl, m_pidl, size);
    ZeroMemory((LPBYTE)root.m_pidl + size, CB_SIZE); // terminator
    root.m_bShareMode = false;

    // copy "object" portion
    size = piidLast->cb + CB_SIZE;
    shm->Alloc(size);
    CopyMemory(obj.m_pidl, piidLast, size);
    obj.m_bShareMode = false;
}

HRESULT CIDLEx::MakeAbsPIDLOf(LPSHELLFOLDER psf, LPITEMIDLIST pidl)
{
    ULONG   ulEaten, ulAttribs;
    HRESULT hr;

    Release();

    CShellFolder sf(psf, true);
    TString strName;
    hr = sf.GetDisplayNameOf(pidl, SHGDN_FORPARSING, strName);
    if (SUCCEEDED(hr))
    {
        hr = sf.ParseDisplayName(strName.c_str(), &ulEaten, &m_pidl, &ulAttribs);
    }

    return hr;
}

HRESULT CIDLEx::GetUIObjectOf(REFIID riid, LPVOID* ppvOut, HWND hWnd, LPSHELLFOLDER psf)
{
    CIDLEx  root, obj;
    HRESULT  hr;

    Split(root, obj);

    CShellFolder parentSf;
    if (psf != nullptr)
        parentSf.Attach(psf, true);
    else
        parentSf = CDesktopFolder::GetDesktopFolder();

    // get the IShellFolder of the root
    CShellFolder   sfRoot;
    hr = parentSf.BindToFolder(root, sfRoot);
    if (SUCCEEDED(hr)) 
    {
        hr = sfRoot.GetUIObjectOf(hWnd, 1, obj, riid, 0, ppvOut);
        sfRoot.Release();
    }

    return hr;
}

CIDLEx& CIDLEx::operator = (const CIDLEx& idl)
{
    if (this != &idl)
    {
        Release();
        Attach(idl.m_pidl, false);
    }

    return *this;
}

CIDLEx& CIDLEx::operator = (CIDLEx&& idl)
{
    if (this != &idl)
    {
        Release();
        m_pidl = idl.m_pidl;
        m_bShareMode = idl.m_bShareMode;
        idl.m_pidl = nullptr;
        idl.m_bShareMode = true;
    }

    return *this;
}

CIDLEx CIDLEx::operator + (const CIDLEx& pidl) const
{
    CIDLEx ret;
    Concat(*this, pidl, ret);
    return ret;
}

void CIDLEx::Release()
{
    if (m_pidl != nullptr)
    {
        if (!m_bShareMode)
            CShMem::GetAllocator()->Free(m_pidl);

        m_pidl = nullptr;
    }
}

LPITEMIDLIST CIDLEx::Clone(LPITEMIDLIST pidl)
{
    if (pidl == nullptr)
        return nullptr;

    CShMem* shm = CShMem::GetAllocator();
    SIZE_T size = shm->GetSize((void *)pidl);
    void* pClone = shm->Alloc(size);
    if (pClone != nullptr)
    {
        ::CopyMemory((void *)pClone, (void *)pidl, size);
    }
    return (LPITEMIDLIST)pClone;
}

// (result = a+b)
void CIDLEx::Concat(const CIDLEx& a, const CIDLEx& b, CIDLEx& result)
{
    result.Release();

    if (a.IsEmpty() && b.IsEmpty()) 
        return;

    if (a.IsEmpty()) 
    { 
        result = b; 
        return; 
    }

    if (b.IsEmpty()) 
    { 
        result = a; 
        return; 
    }

    UINT cb1, cb2;
    cb1 = a.GetSize() - sizeof(a.m_pidl->mkid.cb);
    cb2 = b.GetSize();

    CShMem::GetAllocator()->Alloc(cb1 + cb2); // allocate enough memory 
    CopyMemory(result.m_pidl, a.m_pidl, cb1);                 // 1st
    CopyMemory(((LPBYTE)result.m_pidl) + cb1, b.m_pidl, cb2); // 2nd
    result.m_bShareMode = false;
}


void TransStrrefToString(LPCITEMIDLIST pidl, STRRET& strRet, TString& tString)
{
    switch (strRet.uType)
    {
    case STRRET_WSTR:
    {
        // pOleStr points to a WCHAR string - convert it to ANSI
#ifdef UNICODE
        tString = strRet.pOleStr;
#else
        ::WideCharToMultiByte(CP_ACP, 0, strRet.pOleStr, -1, strRet.cStr, MAX_PATH, NULL, NULL);
        tString = strRet.cStr;
#endif
        CShMem* shm = CShMem::GetAllocator();
        shm->Free(strRet.pOleStr);
        strRet.pOleStr = nullptr;
        break;
    }
    case STRRET_OFFSET:
    {
        // The string lives inside the pidl, so copy it out.
#ifdef UNICODE
        LPWSTR pOleStr = (LPWSTR)((LPBYTE)pidl + strRet.uOffset);
        tString = pOleStr;
        //::WideCharToMultiByte(CP_ACP, 0, pOleStr, -1, strRet.cStr, MAX_PATH, NULL, NULL);
#else
        lstrcpyn(strRet.cStr, (LPSTR)((LPBYTE)m_pidl + strRet.uOffset), MAX_PATH);
        tString = strRet.cStr;
#endif
        break;
    }
    case STRRET_CSTR:
    {
        // The string already is in the right place.
#ifdef UNICODE
        WCHAR strBuf[MAX_PATH];
        ::MultiByteToWideChar(CP_ACP, 0, strRet.cStr, -1, strBuf, MAX_PATH);
        strBuf[MAX_PATH - 1] = 0;
        tString = strBuf;
#else
        tString = strRet.cStr;
#endif
        break;
    }
    }
}


