#include "pch.h"
#include "framework.h"
#include "SystemFunctions.h"
#include "TravelLogMgmt.h"

bool IsSpecialFolderNeedsToTravel(const TString& psth)
{
    return false;
}

bool TravelLogEntryToTravelLoc(ITravelLogEntry *logEntry, CNavigatedPoint& loc)
{
    LPWSTR wstrTmp = nullptr;
    if ((logEntry->GetURL(&wstrTmp) != S_OK) || (wstrTmp == nullptr))
        return false;

    loc.strUrl = TStringFromWStr(wstrTmp);
    ::CoTaskMemFree(wstrTmp);

    wstrTmp = nullptr;
    if ((logEntry->GetTitle(&wstrTmp) != S_OK) || (wstrTmp == nullptr))
        return false;

    loc.strTitle = TStringFromWStr(wstrTmp);
    ::CoTaskMemFree(wstrTmp);

    return true;
}

bool CTravelLogMgmt::GetTravelLog(std::vector<CNavigatedPoint>& logs, ULONG maxWanted, CNavigatedPoint& curItem) const
{
    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<IEnumTravelLogEntry> pEtle;
    HRESULT hr = m_spTravelLogStg->EnumEntries(TLEF_RELATIVE_BACK | TLEF_RELATIVE_FORE| TLEF_RELATIVE_INCLUDE_CURRENT, &pEtle);
    if ((hr != S_OK) || (!pEtle))
        return false;

    ITravelLogEntry* pLogEntry = nullptr;

    ULONG count = 0;
    ULONG Fetched = 0;
    while (SUCCEEDED(pEtle->Next(1, &pLogEntry, &Fetched)) && (pLogEntry != nullptr) && (count < maxWanted))
    {
        CNavigatedPoint loc;
        if (TravelLogEntryToTravelLoc(pLogEntry, loc))
        {
            if (std::find(logs.begin(), logs.end(), loc) == logs.end())
            {
                logs.push_back(loc);
                if (logs.size() >= maxWanted)
                    break;
            }
        }
        pLogEntry->Release();
    }

    ITravelLogEntry* pCurTle = GetCurrentLogEntry();
    if (pCurTle)
    {
        TravelLogEntryToTravelLoc(pCurTle, curItem);
        pCurTle->Release();
    }

    return true;
}

bool CTravelLogMgmt::SetTravelLog(const std::vector<CNavigatedPoint>& logs, const CNavigatedPoint& curItem, bool bTravelToCurrent)
{
    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<ITravelLogEntry> pCurTle;
    for (auto& x : logs)
    {
        CComPtr<ITravelLogEntry> pTle;
        HRESULT hr = m_spTravelLogStg->CreateEntry(x.strUrl.c_str(), x.strTitle.c_str(), NULL, TRUE, &pTle);
        if (hr == S_OK)
        {
            if (x.strUrl == curItem.strUrl)
                pCurTle = pTle;

            pTle.Release();
        }
    }

    if (bTravelToCurrent)
    {
        if (pCurTle == nullptr)
            return false;

        if (m_spTravelLogStg->TravelTo(pCurTle) != S_OK)
            return false;
    }

    return true;
}

void CTravelLogMgmt::ClearTravelLogs()
{
    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<IEnumTravelLogEntry> pEtle;
    //HRESULT hr = m_spTravelLogStg->EnumEntries(TLEF_RELATIVE_BACK | TLEF_RELATIVE_FORE | TLEF_RELATIVE_INCLUDE_CURRENT, &pEtle);
    HRESULT hr = m_spTravelLogStg->EnumEntries(TLEF_ABSOLUTE | TLEF_INCLUDE_UNINVOKEABLE, &pEtle);
    if ((hr != S_OK) || (!pEtle))
        return;

    DWORD dwCount = 0;
    m_spTravelLogStg->GetCount(TLEF_ABSOLUTE, &dwCount);
loop:
    ITravelLogEntry *pTle = nullptr;
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

        pTle->Release();
        goto loop;
    }
}

ITravelLogEntry* CTravelLogMgmt::GetCurrentLogEntry() const
{
    ITravelLogEntry *pTle = nullptr;

    ATLASSERT(m_spTravelLogStg != nullptr);

    CComPtr<IEnumTravelLogEntry> pEtle;
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
