#pragma once

class CIDLEx;

// a wrapper class for IEnumIDList*
class CEnumIDList
{
public:
    CEnumIDList() { m_Enum = nullptr; }
    explicit CEnumIDList(const IEnumIDList* peidlist, bool bAddRef = false) { Attach(peidlist, bAddRef); }
    CEnumIDList(const CEnumIDList& ceidlist) { Attach(ceidlist.m_Enum, true); }
    CEnumIDList(CEnumIDList&& ceidlist) { m_Enum = ceidlist.m_Enum; ceidlist.m_Enum = nullptr; }
    virtual ~CEnumIDList() { Release(); }

    bool IsEmpty() const { return (m_Enum == nullptr); }
    void Attach(const IEnumIDList* peidlist, bool bAddRef = false);
    IEnumIDList* Detach();

    CEnumIDList& operator = (const CEnumIDList& ceidlist);
    CEnumIDList& operator = (CEnumIDList&& ceidlist);

    operator IEnumIDList* () { return m_Enum; }
    void Release();

    HRESULT Next(CIDLEx& rgelt);
    HRESULT Skip(ULONG celt);
    HRESULT Reset(void);
    HRESULT Clone(CEnumIDList& ceidlist);

protected:
    IEnumIDList* m_Enum;
};

// a wrapper class for LPSHELLFOLDER
class CShellFolder
{
public:
    CShellFolder() { m_pSF = nullptr; }
    CShellFolder(const LPSHELLFOLDER psf, bool bAddRef = false) { Attach(psf, bAddRef); }
    CShellFolder(const CShellFolder& sf) { Attach(sf.m_pSF, true); }
    CShellFolder(CShellFolder&& sf) { MoveFrom(sf); }
    virtual ~CShellFolder() { Release(); }
    
    bool IsEmpty() const { return (m_pSF == nullptr); }
    void Attach(const LPSHELLFOLDER psf, bool bAddRef = false);
    LPSHELLFOLDER Detach();

    HRESULT EnumObjects(HWND hwnd, SHCONTF grfFlags, CEnumIDList& cdidlist);
    HRESULT ParseDisplayName(LPCTSTR pszDisplayName, ULONG* pchEaten, PIDLIST_RELATIVE* ppidl, ULONG* pdwAttributes);
    HRESULT ParseDisplayName(LPCTSTR pszDisplayName, PIDLIST_RELATIVE* ppidl);
    HRESULT GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, TString& name);
    HRESULT BindToObject(PCUIDLIST_RELATIVE pidl, REFIID riid, void** ppv);
    HRESULT BindToFolder(PCUIDLIST_RELATIVE pidl, CShellFolder& sf);
    HRESULT GetUIObjectOf(HWND hwndOwner, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* rgfReserved, void** ppv);
    bool ComparePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
    HRESULT GetAttributesOf(const CIDLEx& cidl, SFGAOF* rgfInOut);

    CShellFolder& operator = (const CShellFolder& sf);
    CShellFolder& operator = (CShellFolder&& sf);
    operator LPSHELLFOLDER() { return m_pSF; }
    void Release();

    static CShellFolder GetDesktopFolder();
protected:
    void MoveFrom(CShellFolder& sf);
    LPSHELLFOLDER GetShellFolder() { return m_pSF; }
protected:
    LPSHELLFOLDER m_pSF;
    bool m_bShareMode;
};

class CShMem final
{
public:
    static CShMem s_ShlMem;
    static CShMem* GetAllocator() { return &s_ShlMem; }
    void* Alloc(SIZE_T size);
    void Free(void *pv);
    SIZE_T GetSize(void* pv);
protected:
    CShMem() { Initialize(); }
    ~CShMem() { Release(); }

    bool Initialize();
    void Release();
protected:
    LPMALLOC  m_pAllocator;
};

class CIDLEx
{
public:
    CIDLEx() { m_pidl = nullptr; m_bShareMode = true; }
    CIDLEx(LPCITEMIDLIST pidl, bool bShare = false) { Attach(pidl, bShare); }
    CIDLEx(const CIDLEx& idl) { CopyFrom(idl); }
    CIDLEx(const unsigned char* pData, int size) { CreateByIdListData(pData, size); }
    CIDLEx(CIDLEx&& idl) { MoveFrom(idl); }
    virtual ~CIDLEx() { Release(); }

    bool IsEmpty() const { return (m_pidl == nullptr); }
    void Attach(LPCITEMIDLIST pidl, bool bShare = false);
    LPITEMIDLIST Detach();

    //construct by idlist raw data
    bool CreateByIdListData(const unsigned char* data, int size);
    
    //get raw idlist data
    unsigned char* GetIDListData(int& size) const;

    bool GetCopyOf(LPCITEMIDLIST pidl);
    TString GetParseName() const;
    TString GetDisplayName() const;
    
    UINT GetSize() const { return CIDLEx::GetSize(m_pidl); }


    // A pidl points to a name-space object such as a file or 
    // directory, such as "c:\windows\system\kernel32.dll"
    // This function will split the pidl into two pidls:
    // 1. the pidl of the object's root
    // 2. the pidl of the object, relative to it's root
    // Example: for the above file
    // root - is set to the pidl of "c:\windows\system"
    // obj  - is set to the pidl of "kernel" (relative to root)
    void Split(CIDLEx& root, CIDLEx& obj) const;

    // An absolute PIDL is a PIDL with the desktop as a root.  Use
    // this function to create an absolute PIDL of any other pidl
    // rooted at the folder pointed by psf.
    HRESULT MakeAbsPIDLOf(LPSHELLFOLDER psf, LPITEMIDLIST pidl);

    // IShellFolder->GetUIObjectOf works only for 1 item pidls (i.e.
    // objects that are direct descendants of the folder).
    // This following function allows you to retrieve a UI object of
    // any pidl rooted at the folder psf.
    // Example:
    // psf -> IShellFolder of "c:\windows",
    // pidl -> pidl of "system\kernel32.dll" (relative to psf)
    // 1) Will *not* work psf->GetUIObjectOf(pidl, ... )
    // 2) Will work: pidl.GetUIObjectOf(..., psf)
    HRESULT GetUIObjectOf(REFIID riid, LPVOID* ppvOut, HWND hWnd = NULL, LPSHELLFOLDER psf = nullptr);

    int GetShellIcon(UINT uFlags = SHGFI_SMALLICON) const { return CIDLEx::GetShellIcon(m_pidl, uFlags); }
    LPITEMIDLIST CopyItemID(int n = 0) const { return CIDLEx::CopyItemID(m_pidl, n); }
    LPITEMIDLIST CopyLastItemID() const;
    LPITEMIDLIST CopyItemIDList() const { return CIDLEx::CopyItemIDList(m_pidl); }
    CIDLEx& Concat(LPCITEMIDLIST pidl);


    CIDLEx& operator = (const CIDLEx& idl);
    CIDLEx& operator = (CIDLEx&& idl);
    // Concatenation using the + operator
    CIDLEx& operator + (const CIDLEx& pidl);

    operator LPITEMIDLIST& () { return m_pidl; }
    operator LPITEMIDLIST* () { return &m_pidl; }
    operator LPCITEMIDLIST() const { return m_pidl; }
    operator LPCITEMIDLIST* () const { return (LPCITEMIDLIST*)&m_pidl; }

    void Release();

    // (result = a+b)
    static void Concat(const CIDLEx& a, const CIDLEx& b, CIDLEx& result);
    static CIDLEx CIDLFromFullPath(const TString& path);
    static LPITEMIDLIST CopyItemID(LPITEMIDLIST pidl, int n);
    static LPITEMIDLIST GetNextItemID(LPCITEMIDLIST pidl);
    static UINT GetSize(LPCITEMIDLIST pidl);
    static LPITEMIDLIST CopyItemIDList(LPCITEMIDLIST pidl);
    static LPITEMIDLIST ConcatPidl(LPITEMIDLIST pidlDest, LPCITEMIDLIST pidlSrc);
    static int GetShellIcon(LPCITEMIDLIST pidl, UINT uFlags = SHGFI_SMALLICON);

protected:
    bool CopyFrom(const CIDLEx& idl);
    void MoveFrom(CIDLEx& idl);
    LPITEMIDLIST Clone(LPCITEMIDLIST pidl);

protected:
    LPITEMIDLIST  m_pidl;
    bool m_bShareMode;
};


void TransStrrefToString(LPCITEMIDLIST pidl, STRRET& strRet, TString& tString);
bool TeGetSpecialFolderLocation(HWND hwnd, int csidl, CIDLEx& cidl);
