#pragma once

#include "NavigatedPoint.h"

class CTravelLogMgmt
{
public:
    CTravelLogMgmt() {  }
    virtual ~CTravelLogMgmt() {  }
    
    bool Attach(CComPtr<ITravelLogStg>& spTravelLogStg) { m_spTravelLogStg = spTravelLogStg; return true; }
    bool GetTravelLog(std::vector<CNavigatedPoint>& logs, ULONG maxWanted, CNavigatedPoint& curItem) const;
    bool SetTravelLog(const std::vector<CNavigatedPoint>& logs, const CNavigatedPoint& curItem, bool bTravelToCurrent);
    void ClearTravelLogs();
    ITravelLogEntry* GetCurrentLogEntry() const;

protected:
    CComPtr<ITravelLogStg> m_spTravelLogStg;

};

