#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "TravelLogMgmt.h"

bool IsSpecialFolderNeedsToTravel(const TString& psth)
{
    return false;
}

void CTravelLogMgmt::ClearTravelLogs()
{
    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<IEnumTravelLogEntry> pEtle;;
    HRESULT hr = m_spTravelLogStg->EnumEntries(TLEF_RELATIVE_BACK | TLEF_RELATIVE_FORE, &pEtle);
    if ((hr != S_OK) || (!pEtle))
        return;

loop:
    CComPtr<ITravelLogEntry> pTle;
    if (pEtle->Next(1, &pTle, NULL) == S_OK)
    {
        LPWSTR wstrUrl = nullptr;
        if (pTle->GetURL(&wstrUrl) == S_OK)
        {
            TString path = TStringFromWStr(wstrUrl);
            ::CoTaskMemFree(wstrUrl);
            if (!IsSpecialFolderNeedsToTravel(path))
            {
                m_spTravelLogStg->RemoveEntry(pTle);
            }
        }

        pTle.Release();
        goto loop;
    }
}

void CTravelLogMgmt::AddFakeLogs()
{
    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<ITravelLogEntry> pTle;
    m_spTravelLogStg->CreateEntry(L"D:\\", L"D:", NULL, TRUE, &pTle);

    CComPtr<ITravelLogEntry> pTle3;
    m_spTravelLogStg->CreateEntry(L"D:\\C300V1.2.3P3T6_20130308\\msg6k_pt", L"msg6k_pt", pTle, FALSE, &pTle3);

    CComPtr<ITravelLogEntry> pTle2;
    m_spTravelLogStg->CreateEntry(L"D:\\C300V1.2.3P3T6_20130308", L"C300V1.2.3P3T6_20130308", pTle3, TRUE, &pTle2);

}

CComPtr<ITravelLogEntry> CTravelLogMgmt::GetCurrentLogEntry()
{
    CComPtr<ITravelLogEntry> pTle;

    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<IEnumTravelLogEntry> pEtle;;
    HRESULT hr = m_spTravelLogStg->EnumEntries(TLEF_RELATIVE_INCLUDE_CURRENT, &pEtle);
    if (hr == S_OK)
    {
        if (pEtle->Next(1, &pTle, NULL) == S_OK)
        {
            return pTle;
        }
    }

    return pTle;
}
