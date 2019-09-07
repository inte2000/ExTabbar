#pragma once

//message for com hook and explorer
extern unsigned int WM_REGISTERDRAGDROP;
extern unsigned int WM_NEWTREECONTROL;
extern unsigned int WM_BROWSEOBJECT;
extern unsigned int WM_HEADERINALLVIEWS;
extern unsigned int WM_LISTREFRESHED;
extern unsigned int WM_ISITEMSVIEW;
extern unsigned int WM_ACTIVATESEL;
extern unsigned int WM_BREADCRUMBDPA;
extern unsigned int WM_CHECKPULSE;

BOOL RegisterPrivateMessage();