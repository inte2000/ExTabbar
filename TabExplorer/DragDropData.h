#pragma once

#define CF_PRV_TETAB_DRAG        CF_PRIVATEFIRST + 1

typedef struct tagTeTabDragDrop
{
    HWND hSourceWnd;
    int nDragItem;
    GUID DragItemId;
    WCHAR wcUrl[MAX_PATH]; //parsename
    WCHAR wcText[256];
    WCHAR wcTip[256];
    int nIconIdx;
    unsigned char idlData[512];
    int idlSize;
}TeTabDragDrop;

