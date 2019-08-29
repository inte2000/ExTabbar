#pragma once

// a wrapper class for LPSHELLFOLDER
class CShellFolder
{
public:
    CShellFolder() { m_pSF = nullptr; }
    CShellFolder(const LPSHELLFOLDER psf, BOOL bShare = false) { Attach(psf, bShare); }
    CShellFolder(const CShellFolder& sf) { Attach(sf.m_pSF, true); }
    virtual ~CShellFolder() { Release(); }

    void Attach(const LPSHELLFOLDER psf, BOOL bShare = false);
    LPSHELLFOLDER Detach() { LPSHELLFOLDER tmp = m_pSF; m_pSF = nullptr; return tmp; }

    HRESULT ParseDisplayName(LPCTSTR pszDisplayName, ULONG* pchEaten, PIDLIST_RELATIVE* ppidl, ULONG* pdwAttributes);
    HRESULT GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, TString& name);
    HRESULT BindToObject(PCUIDLIST_RELATIVE pidl, REFIID riid, void** ppv);
    HRESULT BindToFolder(PCUIDLIST_RELATIVE pidl, CShellFolder& sf);
    HRESULT GetUIObjectOf(HWND hwndOwner, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* rgfReserved, void** ppv);

    CShellFolder& operator = (const CShellFolder& sf);
    operator LPSHELLFOLDER() { return m_pSF; }
    void Release();
protected:

    LPSHELLFOLDER GetShellFolder() { return m_pSF; }
protected:
    LPSHELLFOLDER m_pSF;
    bool m_bShareMode;
};

// special case for DesktopFolder
class CDesktopFolder final : public CShellFolder
{
protected:
    CDesktopFolder() { Initialize(); }
    virtual ~CDesktopFolder() {}

    bool Initialize();
public:
    static const CShellFolder& GetDesktopFolder() { return s_DesktopFolder; }
protected:
    static CDesktopFolder s_DesktopFolder;
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
    CIDLEx(LPITEMIDLIST pidl, bool bShare) { Attach(pidl, bShare); }
    CIDLEx(const CIDLEx& idl) { Attach(idl.m_pidl, false); }
    CIDLEx(CIDLEx&& idl);
    virtual ~CIDLEx() { Release(); }

    bool IsEmpty() const { return (m_pidl == nullptr); }
    void Attach(LPITEMIDLIST pidl, bool bShare);
    LPITEMIDLIST Detach();

    //copy set
    HRESULT Set(LPITEMIDLIST pidl) { Attach(pidl, false); }
    // Set by path: szPath relative to the folder psf.  Uses
    // the desktop by default, so you can use regular path strings
    // like "c:\windows"
    HRESULT Set(LPCTSTR szPath, LPSHELLFOLDER psf = nullptr);
    TString GetParseName() const;
    TString GetDisplayName() const;
    
    UINT GetSize() const;
    // A pidl points to a name-space object such as a file or 
    // directory, such as "c:\windows\system\kernel32.dll"
    // This function will split the pidl into two pidls:
    // 1. the pidl of the object's root
    // 2. the pidl of the object, relative to it's root
    // Example: for the above file
    // root - is set to the pidl of "c:\windows\system"
    // obj  - is set to the pidl of "kernel" (relative to root)
    void Split(CIDLEx& root, CIDLEx& obj) const;

    // Returns a pointer to the first item in the list
    LPSHITEMID GetFirstItemID() const  { return (LPSHITEMID)m_pidl; }
    // Points to the next item in the list
    void GetNextItemID(LPSHITEMID& pid) const { (LPBYTE&)pid += pid->cb; }

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

    CIDLEx& operator = (const CIDLEx& idl);
    CIDLEx& operator = (CIDLEx&& idl);
    // Concatenation using the + operator
    CIDLEx operator + (const CIDLEx& pidl) const;

    // (result = a+b)
    static void Concat(const CIDLEx& a, const CIDLEx& b, CIDLEx& result);

    operator LPITEMIDLIST& () { return m_pidl; }
    operator LPITEMIDLIST* () { return &m_pidl; }
    operator LPCITEMIDLIST() const { return m_pidl; }
    operator LPCITEMIDLIST* () const { return (LPCITEMIDLIST*)&m_pidl; }

    void Release();
protected:
    LPITEMIDLIST Clone(LPITEMIDLIST pidl);

protected:
    LPITEMIDLIST  m_pidl;
    bool m_bShareMode;
};


void TransStrrefToString(LPCITEMIDLIST pidl, STRRET& strRet, TString& tString);
