#pragma once


class CNavigatedPoint final
{
public:
    TString strTitle;
    TString strUrl;

    bool operator == (const CNavigatedPoint& l1)
    {
        if (l1.strTitle != strTitle)
            return false;
        if (l1.strUrl != strUrl)
            return false;

        return true;
    }
};

