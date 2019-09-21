#include "pch.h"
#include <algorithm>
#include "ColorHelper.h"


/*
r,g,b value should be [0,255]
*/
void RGB2HSB(int rgbR, int rgbG, int rgbB, double *hsbH, double *hsbS, double *hsbB) 
{
	int maxRgb = std::max<int>(rgbR, std::max<int>(rgbG, rgbB));
	int minRgb = std::min<int>(rgbR, std::min<int>(rgbG, rgbB));
 
	*hsbB = maxRgb / 255.0;
    *hsbS = maxRgb == 0 ? 0 : (maxRgb - minRgb) / (float) maxRgb;
 
    if(maxRgb == minRgb)
    {
        *hsbH = 0;
    }
	else if (maxRgb == rgbR && rgbG >= rgbB)
    {
		*hsbH = (rgbG - rgbB) * 60.0 / (maxRgb - minRgb) + 0.0;
	} 
    else if (maxRgb == rgbR && rgbG < rgbB)
    {
		*hsbH = (rgbG - rgbB) * 60.0 / (maxRgb - minRgb) + 360.0;
	} 
    else if (maxRgb == rgbG)
    {
		*hsbH = (rgbB - rgbR) * 60.0 / (maxRgb - minRgb) + 120.0;
	} 
    else if (maxRgb == rgbB)
    {
		*hsbH = (rgbR - rgbG) * 60.0 / (maxRgb - minRgb) + 240.0;
	}

    *hsbH = (*hsbH >= 360.0) ? 0.0 : *hsbH;
}

/*
0.0 <= h <= 360.0
0.0 <= ss <= 1.0
0.0f <= v <= 1.0
*/
void HSB2RGB(double hsbH, double hsbS, double hsbB, int *rgbR, int *rgbG, int*rgbB) 
{
    if (hsbS == 0.0)
    {
        *rgbR = (int)(hsbB * 255.0);
        *rgbG = (int)(hsbB * 255.0);
        *rgbB = (int)(hsbB * 255.0);
    }
    else
    {
        double r = 0.0, g = 0.0, b = 0.0;
        int i = (int(hsbH) / 60) % 6;
        double f = (hsbH / 60) - i;
        double p = hsbB * (1 - hsbS);
        double q = hsbB * (1 - f * hsbS);
        double t = hsbB * (1 - (1 - f) * hsbS);

        switch (i)
        {
        case 0:
            r = hsbB;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = hsbB;
            b = p;
            break;
        case 2:
            r = p;
            g = hsbB;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = hsbB;
            break;
        case 4:
            r = t;
            g = p;
            b = hsbB;
            break;
        case 5:
            r = hsbB;
            g = p;
            b = q;
            break;
        default:
            break;
        }

        *rgbR = (int)(r * 255.0);
        *rgbG = (int)(g * 255.0);
        *rgbB = (int)(b * 255.0);
    }
}

//RGB颜色亮公式:L=R*0.30+G*0.59+B*0.11,305911 公式
COLORREF LightColor(COLORREF cr, double rate)
{
    double h, s, v;
    RGB2HSB(GetRValue(cr), GetGValue(cr), GetBValue(cr), &h, &s, &v);

    v = v * (1.0 + rate);
    if (v > 1.0)
        v = 1.0;

    int r, g, b;
    HSB2RGB(h, s, v, &r, &g, &b);

    return RGB(r, g, b);
}

COLORREF DarkColor(COLORREF cr, double rate)
{
    double h, s, v;
    RGB2HSB(GetRValue(cr), GetGValue(cr), GetBValue(cr), &h, &s, &v);

    v = v * (1.0 - rate);
    if (v < 0.0)
        v = 0.0;

    int r, g, b;
    HSB2RGB(h, s, v, &r, &g, &b);

    return RGB(r, g, b);
}
