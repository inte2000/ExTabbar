#pragma once


class CTravelLogMgmt
{
public:
    CTravelLogMgmt() {  }
    virtual ~CTravelLogMgmt() {  }
    
    bool Attach(CComPtr<ITravelLogStg>& spTravelLogStg) { m_spTravelLogStg = spTravelLogStg; return true; }
    void ClearTravelLogs();
    void AddFakeLogs();
    CComPtr<ITravelLogEntry> GetCurrentLogEntry();

protected:
    CComPtr<ITravelLogStg> m_spTravelLogStg;

};

