#include "pch.h"
#include "ShellWrapper.h"


//////////////////////            CEnumIDList              ////////////////////////
void CEnumIDList::Attach(const IEnumIDList* peidlist, bool AddRef)
{
    Release();

    m_Enum = const_cast<IEnumIDList*>(peidlist);
    if (AddRef)
        m_Enum->Release();
}

IEnumIDList* CEnumIDList::Detach() 
{ 
    IEnumIDList* tmp = m_Enum; 
    m_Enum = nullptr; 
    return tmp; 
}

CEnumIDList& CEnumIDList::operator = (const CEnumIDList& ceidlist)
{
    if (this != &ceidlist)
    {
        Release();
        Attach(ceidlist.m_Enum, true);
    }

    return *this;
}

CEnumIDList& CEnumIDList::operator = (CEnumIDList&& ceidlist)
{
    if (this != &ceidlist)
    {
        Release();
        m_Enum = ceidlist.m_Enum;
        ceidlist.m_Enum = nullptr;
    }

    return *this;
}

void CEnumIDList::Release()
{
    if (m_Enum != nullptr)
    {
        m_Enum->Release();
        m_Enum = nullptr;
    }
}

HRESULT CEnumIDList::Next(CIDLEx& rgelt)
{
    if(m_Enum != nullptr)
    {
        LPITEMIDLIST pidlNext = NULL;
        HRESULT hr = m_Enum->Next(1, &pidlNext, NULL);
        if (hr == S_OK)
        {
            rgelt.Attach(pidlNext, false);
        }
        
        return hr;
    }

    return E_FAIL;
}

HRESULT CEnumIDList::Skip(ULONG celt)
{
    if (m_Enum != nullptr)
    {
        return m_Enum->Skip(celt);
    }

    return E_FAIL;
}

HRESULT CEnumIDList::Reset(void)
{
    if (m_Enum != nullptr)
    {
        return m_Enum->Reset();
    }

    return E_FAIL;
}

HRESULT CEnumIDList::Clone(CEnumIDList& ceidlist)
{
    if (m_Enum != nullptr)
    {
        IEnumIDList* penum = nullptr;
        HRESULT hr = m_Enum->Clone(&penum);
        if (hr == S_OK)
        {
            ceidlist.Attach(penum, false);
        }

        return hr;
    }

    return E_FAIL;
}


//////////////////////            CShellFolder              ////////////////////////
void CShellFolder::Attach(const LPSHELLFOLDER psf, bool bAddRef)
{ 
    Release();

    m_pSF = psf; 
    if ((m_pSF != nullptr) && bAddRef)
        m_pSF->AddRef();
}

LPSHELLFOLDER CShellFolder::Detach() 
{ 
    LPSHELLFOLDER tmp = m_pSF; 
    m_pSF = nullptr; 
    return tmp; 
}

CShellFolder& CShellFolder::operator = (const CShellFolder& sf)
{
    if (this != &sf)
    {
        Release();
        Attach(sf.m_pSF, true);
    }

    return *this;
}

CShellFolder& CShellFolder::operator = (CShellFolder&& sf)
{
    if (this != &sf)
    {
        Release();
        MoveFrom(sf);
    }

    return *this;
}

HRESULT CShellFolder::EnumObjects(HWND hwnd, SHCONTF grfFlags, CEnumIDList& cdidlist)
{
    if (m_pSF != nullptr)
    {
        IEnumIDList* penumIDList = nullptr;
        HRESULT hr = m_pSF->EnumObjects(hwnd, grfFlags, &penumIDList);
        if (hr == S_OK)
        {
            cdidlist.Attach(penumIDList);
        }
        return hr;
    }

    return E_FAIL;
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

HRESULT CShellFolder::ParseDisplayName(LPCTSTR pszDisplayName, PIDLIST_RELATIVE* ppidl)
{
    ULONG chEaten = 0;
    ULONG dwAttributes = 0;

    return ParseDisplayName(pszDisplayName, &chEaten, ppidl, &dwAttributes);
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
    if (m_pSF != nullptr)
    {
        return m_pSF->BindToObject(pidl, NULL, riid, ppv);
    }

    return E_FAIL;
}

HRESULT CShellFolder::BindToFolder(PCUIDLIST_RELATIVE pidl, CShellFolder& sf)
{
    if (m_pSF != nullptr)
    {
        LPSHELLFOLDER   psfRoot;
        // get the IShellFolder of the root
        HRESULT hr = m_pSF->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*)& psfRoot);
        if (hr == S_OK)
        {
            sf.Attach(psfRoot, false);
        }

        return hr;
    }

    return E_FAIL;
}

HRESULT CShellFolder::GetUIObjectOf(HWND hwndOwner, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* rgfReserved, void** ppv)
{
    if (m_pSF != nullptr)
    {
        return m_pSF->GetUIObjectOf(hwndOwner, cidl, apidl, riid, rgfReserved, ppv);
    }

    return E_FAIL;
}

bool CShellFolder::ComparePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    if (pidl1 == NULL || pidl2 == NULL)
        return false;

    if (m_pSF != nullptr)
    {
        return ((int)m_pSF->CompareIDs(0, pidl1, pidl2) == 0);
    }

    return false;
}

HRESULT CShellFolder::GetAttributesOf(const CIDLEx& cidl, SFGAOF* rgfInOut)
{
    if (m_pSF != nullptr)
    {
        return m_pSF->GetAttributesOf(1, (LPCITEMIDLIST*)&cidl, rgfInOut);
    }

    return E_FAIL;
}

void CShellFolder::Release()
{
    if (m_pSF != nullptr)
    {
        m_pSF->Release();
        m_pSF = nullptr;
    }
}

CShellFolder CShellFolder::GetDesktopFolder()
{
    CShellFolder folder;
    IShellFolder* psf = nullptr;
    HRESULT hr = ::SHGetDesktopFolder(&psf);
    if (hr == S_OK)
    {
        folder.Attach(psf, false);
    }

    return std::move(folder);
}

void CShellFolder::MoveFrom(CShellFolder& sf)
{
    this->m_pSF = sf.m_pSF;
    sf.m_pSF = nullptr;
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

void CIDLEx::Attach(LPCITEMIDLIST pidl, bool bShare)
{
    Release();

    m_bShareMode = bShare;
    m_pidl = const_cast<LPITEMIDLIST>(pidl);
}

LPITEMIDLIST CIDLEx::Detach()
{
    LPITEMIDLIST tmp = m_pidl;
    m_pidl = nullptr;
    m_bShareMode = true;
    return tmp;
}


bool CIDLEx::CreateByIdListData(const unsigned char* data, int size)
{
    if ((data == nullptr) || (size == 0))
    {
        return false;
    }

    Release();

    m_pidl = (LPITEMIDLIST)CShMem::GetAllocator()->Alloc(size);
    ::CopyMemory(m_pidl, data, size);
    m_bShareMode = false;

    return true;;
}

unsigned char* CIDLEx::GetIDListData(int& size) const
{
    unsigned char* pData = nullptr;
    size = 0;
    if (m_pidl != nullptr) 
    {
        UINT dataSize = ::ILGetSize(m_pidl);
        if (dataSize != 0)
        {
            pData = new unsigned char[dataSize];
            if (pData != nullptr)
            {
                ::CopyMemory(pData, m_pidl, dataSize);
                size = static_cast<int>(dataSize);
            }
        }
    }

    return pData;
}

bool CIDLEx::GetCopyOf(LPCITEMIDLIST pidl)
{
    assert(m_pidl == nullptr);
    m_pidl = CIDLEx::CopyItemIDList(pidl);
    m_bShareMode = false;

    return (m_pidl != nullptr);
}

TString CIDLEx::GetParseName() const
{
    TString name;

    if (m_pidl != nullptr)
    {
        CShellFolder sf = CShellFolder::GetDesktopFolder();
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
        CShellFolder sf = CShellFolder::GetDesktopFolder();
        TString strName;
        sf.GetDisplayNameOf(m_pidl, SHGDN_NORMAL, name);
    }

    return std::move(name);
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
        parentSf = CShellFolder::GetDesktopFolder();

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

LPITEMIDLIST CIDLEx::CopyLastItemID() const
{
    LPITEMIDLIST pidl = m_pidl;

    if (pidl == nullptr)
        return nullptr;

    LPITEMIDLIST last_pidl = pidl;
    while (pidl->mkid.cb)
    {
        last_pidl = pidl;
        pidl = CIDLEx::GetNextItemID(pidl);
    }
    if (last_pidl == nullptr)
        return nullptr;
    return CIDLEx::CopyItemID(last_pidl, 0);
}

CIDLEx& CIDLEx::Concat(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST newpidl = CIDLEx::ConcatPidl(m_pidl, pidl);
    Attach(newpidl, false);  // not share, we owned it

    return *this;
}

CIDLEx& CIDLEx::operator = (const CIDLEx& idl)
{
    if (this != &idl)
    {
        Release();
        CopyFrom(idl);
    }

    return *this;
}

CIDLEx& CIDLEx::operator = (CIDLEx&& idl)
{
    if (this != &idl)
    {
        Release();
        MoveFrom(idl);
    }

    return *this;
}

CIDLEx& CIDLEx::operator + (const CIDLEx& pidl)
{
    LPITEMIDLIST newpidl = CIDLEx::ConcatPidl(m_pidl, pidl);
    Attach(newpidl, false);  // not share, we owned it
    
    return *this;
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

bool CIDLEx::CopyFrom(const CIDLEx& idl)
{
    m_pidl = idl.CopyItemIDList();
    m_bShareMode = false;

    return (m_pidl != nullptr);
}

void CIDLEx::MoveFrom(CIDLEx& idl)
{
    m_pidl = idl.m_pidl;
    m_bShareMode = idl.m_bShareMode;
    idl.m_pidl = nullptr;
    idl.m_bShareMode = true;
}

LPITEMIDLIST CIDLEx::Clone(LPCITEMIDLIST pidl)
{
    if (pidl == nullptr)
        return nullptr;
#if 0
    CShMem* shm = CShMem::GetAllocator();
    SIZE_T size = shm->GetSize((void *)pidl);
    void* pClone = shm->Alloc(size);
    if (pClone != nullptr)
    {
        ::CopyMemory((void *)pClone, (void *)pidl, size);
    }
#endif
    LPITEMIDLIST pClone = ::ILClone(pidl);

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

    result.m_pidl = (LPITEMIDLIST)CShMem::GetAllocator()->Alloc(cb1 + cb2); // allocate enough memory 
    ::CopyMemory(result.m_pidl, a.m_pidl, cb1);                 // 1st
    ::CopyMemory(((LPBYTE)result.m_pidl) + cb1, b.m_pidl, cb2); // 2nd
    result.m_bShareMode = false;
}

CIDLEx CIDLEx::CIDLFromFullPath(const TString& path)
{
    CIDLEx cidl;

    LPITEMIDLIST pidl = ::ILCreateFromPath(path.c_str());
    if (pidl != nullptr)
    {
        cidl.GetCopyOf(pidl);
        ::ILFree(pidl);
    }

    return std::move(cidl);
}

LPITEMIDLIST CIDLEx::CopyItemID(LPITEMIDLIST pidl, int n)
{
    if (pidl == nullptr)
        return nullptr;

    if (n == 0)
    {
        int cb = pidl->mkid.cb;
        int nSize = cb + sizeof(pidl->mkid.cb);
        // Allocate a new item identifier list. 
        LPITEMIDLIST pidlNew = (LPITEMIDLIST)CShMem::GetAllocator()->Alloc(nSize);
        if (pidlNew == NULL)
            return NULL;

        ZeroMemory(pidlNew, nSize);
        // Copy the specified item identifier. 
        CopyMemory(pidlNew, pidl, nSize - sizeof(pidl->mkid.cb));
        return pidlNew;
    }
    else
    {
        LPITEMIDLIST pidl_index = NULL;
        for (int i = 0; i < n && pidl->mkid.cb; i++)
        {
            pidl_index = pidl;
            pidl = GetNextItemID(pidl);
        }
        return pidl_index ? CIDLEx::CopyItemID(pidl_index, 0) : nullptr;
    }
}

LPITEMIDLIST CIDLEx::GetNextItemID(LPCITEMIDLIST pidl)
{
    LPBYTE lpMem = (LPBYTE)pidl;
    lpMem += pidl->mkid.cb;
    return (LPITEMIDLIST)lpMem;
}

UINT CIDLEx::GetSize(LPCITEMIDLIST pidl)
{
    UINT        cbTotal = 0;

    if (pidl)
    {
        cbTotal += sizeof(pidl->mkid.cb);       // Null terminator
        while (pidl->mkid.cb)
        {
            cbTotal += pidl->mkid.cb;
            pidl = CIDLEx::GetNextItemID(pidl);
        }
    }

    return cbTotal;
}

LPITEMIDLIST CIDLEx::CopyItemIDList(LPCITEMIDLIST pidl)
{
    if (pidl == nullptr)
        return nullptr;
    // Allocate a new item identifier list. 
    int nSize = CIDLEx::GetSize(pidl);
    LPITEMIDLIST pidlNew = (LPITEMIDLIST)CShMem::GetAllocator()->Alloc(nSize);
    if (pidlNew == nullptr)
        return nullptr;

    ::ZeroMemory(pidlNew, nSize);
    ::CopyMemory(pidlNew, pidl, nSize);

    return pidlNew;
}

LPITEMIDLIST CIDLEx::ConcatPidl(LPITEMIDLIST pidlDest, LPCITEMIDLIST pidlSrc)
{
    // Get the size of the specified item identifier. 
    UINT cbDest = 0;
    UINT cbSrc = 0;
    if (pidlDest)  //May be NULL
        cbDest = GetSize(pidlDest) - sizeof(pidlDest->mkid.cb);
    cbSrc = GetSize(pidlSrc);

    // Allocate a new item identifier list. 
    LPITEMIDLIST pidlNew = (LPITEMIDLIST)CShMem::GetAllocator()->Alloc(cbSrc + cbDest);
    if (pidlNew == NULL)
        return NULL;
    
    // Copy the specified item identifier. 
    if (pidlDest)
        ::CopyMemory(pidlNew, pidlDest, cbDest);
    
    ::CopyMemory(((LPBYTE)pidlNew) + cbDest, pidlSrc, cbSrc);

    return pidlNew;
}

int CIDLEx::GetShellIcon(LPCITEMIDLIST pidl, UINT uFlags)
{
    SHFILEINFO sfi;
    uFlags |= (SHGFI_PIDL | SHGFI_SYSICONINDEX);
    DWORD_PTR rtn = ::SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);
    if (rtn != 0)
    {
        return sfi.iIcon;
    }

    return -1;
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

bool TeGetSpecialFolderLocation(HWND hwnd, int csidl, CIDLEx& cidl)
{
    LPITEMIDLIST pidl = NULL;

    HRESULT hr = ::SHGetSpecialFolderLocation(hwnd, csidl, &pidl);
    if (!SUCCEEDED(hr))
        return false;

    cidl.Attach(pidl, false);
    return true;
}

