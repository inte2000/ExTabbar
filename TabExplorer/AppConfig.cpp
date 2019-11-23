#include "pch.h"
#include "AppConfig.h"


//global options
BOOL g_bHideRapidAccess = TRUE; //隐藏快速访问节点
BOOL g_bAddUpButton = TRUE; //给浏览栏增加一个UP按钮（win7和win8需要，win10已经恢复了UP按钮）
BOOL g_bFixFolderScroll = FALSE; //only for win 7
BOOL g_bFolderAutoNavigate = TRUE;
/*vista只有的版本中，explorer的树控件增加了一种自动隐藏节点+/-标记的特性，当explorer
失去焦点时，这些三角形符号会慢慢消失 */
BOOL g_bNoFadeInOutExpand = FALSE;
BOOL g_bTreeViewAutoScroll = FALSE;
BOOL g_bTreeViewSimpleMode = FALSE;
int g_nTreeViewIndent = 0;
int g_nUpButtonIconSize = 24;
BOOL g_bForceRefreshWin7 = TRUE;
BOOL g_bShowFreespaceInStatusBar = TRUE;
BOOL g_bNoWindowResizing = FALSE;

BOOL g_bUsingLargeButton = FALSE;
int g_nSmallIconSize = 16;
int g_nLargeIconSize = 24;
BOOL g_bToolbarListMode = TRUE; //工具栏按钮上显示文本
BOOL g_bToolbarSameSize = FALSE;
BOOL g_bEnableSettings = TRUE; //显示“设置”按钮
BOOL g_bBandNewLine = FALSE;
BOOL g_bTabFixWidth = TRUE;
BOOL g_bTabNewButton = TRUE;
BOOL g_bTabCloseButton = TRUE;
BOOL g_bTabAutoHideButtons = TRUE;
BOOL g_bForceSysListView = TRUE;
BOOL g_bSwitchNewTab = TRUE; //立即切换到新打开的tab标签
BOOL g_bNewTabInsertBegin = FALSE; //新标签插入在所有标签最前面

int g_nDragImageTransparent = 200;

ATL::CString g_strShellNewMenuText = _T("新建");
COLORREF g_TeStatusBkColor = RGB(200, 191, 231);
