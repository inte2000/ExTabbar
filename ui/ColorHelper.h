#pragma once


void RGB2HSB(int rgbR, int rgbG, int rgbB, double* hsbH, double* hsbS, double* hsbB);
void HSB2RGB(double h, double s, double b, int* rgbR, int* rgbG, int* rgbB);


COLORREF LightColor(COLORREF cr, double rate);
COLORREF DarkColor(COLORREF cr, double rate);




