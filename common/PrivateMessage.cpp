#include "pch.h"
#include <Windows.h>
#include <ShObjIdl.h>
#include <Shlobj.h>
#include <tchar.h>
#include "PrivateMessage.h"
//#include <UIAutomationCore.h>

unsigned int WM_REGISTERDRAGDROP;
unsigned int WM_NEWTREECONTROL;
unsigned int WM_BROWSEOBJECT;
unsigned int WM_HEADERINALLVIEWS;
unsigned int WM_LISTREFRESHED;
unsigned int WM_ISITEMSVIEW;
unsigned int WM_ACTIVATESEL;
unsigned int WM_BREADCRUMBDPA;
unsigned int WM_CHECKPULSE;

BOOL RegisterPrivateMessage()
{
    WM_REGISTERDRAGDROP = ::RegisterWindowMessage(_T("TabExplorer_RegisterDragDrop"));
    WM_NEWTREECONTROL = ::RegisterWindowMessage(_T("TabExplorer_NewTreeControl"));
    WM_BROWSEOBJECT = ::RegisterWindowMessage(_T("TabExplorer_BrowseObject"));
    WM_HEADERINALLVIEWS = ::RegisterWindowMessage(_T("TabExplorer_HeaderInAllViews"));
    WM_LISTREFRESHED = ::RegisterWindowMessage(_T("TabExplorer_ListRefreshed"));
    WM_ISITEMSVIEW = ::RegisterWindowMessage(_T("TabExplorer_IsItemsView"));
    WM_ACTIVATESEL = ::RegisterWindowMessage(_T("TabExplorer_ActivateSelection"));
    WM_BREADCRUMBDPA = ::RegisterWindowMessage(_T("TabExplorer_BreadcrumbDPA"));
    WM_CHECKPULSE = ::RegisterWindowMessage(_T("TabExplorer_CheckPulse"));

    return TRUE;
}
