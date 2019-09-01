// xconfig.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "XMLProfile.h"
#include "xconfig.h"

CXmlConfig::CXmlConfig()
{
    m_xmlProfile = new CXmlProfile();
}

CXmlConfig::CXmlConfig(LPCTSTR fileName)
{
    m_xmlProfile = new CXmlProfile();

    if(m_xmlProfile != NULL)
    {
        m_xmlProfile->Load(fileName, CXmlProfile::Ansi);
    }
}

CXmlConfig::~CXmlConfig()
{
    if(m_xmlProfile != NULL)
    {
        m_xmlProfile->DeleteContents();
        delete m_xmlProfile;
        m_xmlProfile = NULL;
    }
}

BOOL CXmlConfig::LoadConfig(LPCTSTR fileName)
{
    if(m_xmlProfile == NULL)
        return FALSE;

    return m_xmlProfile->Load(fileName, CXmlProfile::Ansi);
}

BOOL CXmlConfig::SaveConfig(LPCTSTR fileName)
{
    if(m_xmlProfile == NULL)
        return FALSE;

    if(fileName != NULL)
    {
        return m_xmlProfile->Save(fileName, CXmlProfile::Ansi);
    }
    else
    {
        return m_xmlProfile->Save(CXmlProfile::Ansi);
    }
}

BOOL CXmlConfig::IsValidate()
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->Validate(); 
}

BOOL CXmlConfig::IsFileOpen()
{ 
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->IsFileOpen(); 
}

UINT CXmlConfig::GetConfigCount(LPCTSTR lpszSectionName)
{
    if(m_xmlProfile == NULL)
    {
        return 0;
    }

    return m_xmlProfile->GetNumKeys(lpszSectionName);
}

UINT CXmlConfig::GetSectionsCount()
{
    if(m_xmlProfile == NULL)
    {
        return 0;
    }

    return m_xmlProfile->GetNumSections();
}

BOOL CXmlConfig::GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes)
{
    if(m_xmlProfile == NULL)
    {
        *pBytes = 0;
        *ppData = NULL;
        return FALSE;
    }

    return m_xmlProfile->GetProfileBinary(lpszSection, lpszEntry, ppData, pBytes);
}

BOOL CXmlConfig::SetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->SetProfileBinary(lpszSection, lpszEntry, pData, nBytes);
}

UINT CXmlConfig::GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
    if(m_xmlProfile == NULL)
    {
        return nDefault;
    }
    
    return m_xmlProfile->GetProfileInt(lpszSection, lpszEntry, nDefault);
}

BOOL CXmlConfig::SetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->SetProfileInt(lpszSection, lpszEntry, nValue);
}

BOOL CXmlConfig::GetBool(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bDefault)
{
    if(m_xmlProfile == NULL)
    {
        return bDefault;
    }

    return m_xmlProfile->GetProfileBool(lpszSection, lpszEntry, bDefault);
}

BOOL CXmlConfig::SetBool(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bValue)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

	return m_xmlProfile->SetProfileBool(lpszSection, lpszEntry, bValue);
}

CString CXmlConfig::GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
    if(m_xmlProfile == NULL)
    {
        return lpszDefault;
    }

    return m_xmlProfile->GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

BOOL CXmlConfig::SetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->SetProfileString(lpszSection, lpszEntry, lpszValue);
}

BOOL CXmlConfig::IsKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->IsKey(lpszSectionName, lpszKeyName); 
}

BOOL CXmlConfig::IsSection(LPCTSTR lpszSection)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->IsSection(lpszSection); 
}

BOOL CXmlConfig::IsSectionReadOnly(LPCTSTR lpszSection)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }

	CXmlSectionListElement *pSectionElemnet = m_xmlProfile->FindSection(lpszSection);
    if(pSectionElemnet == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->GetReadOnly(pSectionElemnet);
}

void CXmlConfig::SetSectionReadOnly(LPCTSTR lpszSection, BOOL bReadOnly)
{
    if(m_xmlProfile != NULL)
    {
	    CXmlSectionListElement *pSectionElemnet = m_xmlProfile->FindSection(lpszSection);
        if(pSectionElemnet != NULL)
        {
            m_xmlProfile->SetReadOnly(pSectionElemnet, bReadOnly);
        }
    }
}

BOOL CXmlConfig::IsKeyReadOnly(LPCTSTR lpszSection, LPCTSTR lpszKeyName)
{
    if(m_xmlProfile == NULL)
    {
        return FALSE;
    }
	CXmlKeyListElement *pKeyElenment = m_xmlProfile->FindKey(lpszSection, lpszKeyName);
    if(pKeyElenment == NULL)
    {
        return FALSE;
    }

    return m_xmlProfile->GetReadOnly(pKeyElenment);
}

void CXmlConfig::SetKeyReadOnly(LPCTSTR lpszSection, LPCTSTR lpszKeyName, BOOL bReadOnly)
{
    if(m_xmlProfile != NULL)
    {
	    CXmlKeyListElement *pKeyElenment = m_xmlProfile->FindKey(lpszSection, lpszKeyName);
        if(pKeyElenment != NULL)
        {
            m_xmlProfile->SetReadOnly(pKeyElenment, bReadOnly);
        }
    }
}


#ifdef _DEBUG
void CXmlConfig::Dump()
{
    if(m_xmlProfile != NULL)
    {
        m_xmlProfile->Dump();
    }
}
#endif
