// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XCONFIG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XCONFIG_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef XCONFIG_EXPORTS
#define XCONFIG_API __declspec(dllexport)
#else
#define XCONFIG_API __declspec(dllimport)
#endif


class CXmlProfile;

// This class is exported from the xconfig.dll
class XCONFIG_API CXmlConfig 
{
public:
	CXmlConfig();
	CXmlConfig(LPCTSTR fileName);
    ~CXmlConfig();

	BOOL LoadConfig(LPCTSTR fileName);
	BOOL SaveConfig(LPCTSTR fileName = NULL);
    BOOL IsValidate();
    BOOL IsFileOpen();
	UINT GetConfigCount(LPCTSTR lpszSectionName);
	UINT GetSectionsCount();

    //config get and set interface
	BOOL GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes);
	BOOL SetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);

	UINT GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
	BOOL SetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	BOOL GetBool(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bDefault);
	BOOL SetBool(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bValue);

	CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	BOOL SetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);

	BOOL IsKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);
	BOOL IsSection(LPCTSTR lpszSection);

	BOOL IsSectionReadOnly(LPCTSTR lpszSection);
	void SetSectionReadOnly(LPCTSTR lpszSection, BOOL bReadOnly);
	BOOL IsKeyReadOnly(LPCTSTR lpszSection, LPCTSTR lpszKeyName);
	void SetKeyReadOnly(LPCTSTR lpszSection, LPCTSTR lpszKeyName, BOOL bReadOnly);

    // TODO: add your methods here.
#ifdef _DEBUG
    void Dump();
#endif

protected:
    CXmlProfile *m_xmlProfile;
};

