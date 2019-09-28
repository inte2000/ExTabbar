#pragma once

#define CF_PRV_TETAB_DRAG        CF_PRIVATEFIRST + 1

typedef struct tagTeTabDragDrop
{
    HWND hSourceWnd;
    int nDragItem;
    WCHAR wcUrl[MAX_PATH]; //parsename
    unsigned char idlData[512];
    int idlSize;
}TeTabDragDrop;

