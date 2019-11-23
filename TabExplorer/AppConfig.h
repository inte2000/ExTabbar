#pragma once

//global options
extern BOOL g_bHideRapidAccess; //隐藏快速访问节点
extern BOOL g_bAddUpButton; //给浏览栏增加一个UP按钮（win7和win8需要，win10已经恢复了UP按钮）
extern BOOL g_bFixFolderScroll; //only for win 7
extern BOOL g_bFolderAutoNavigate;
/*vista只有的版本中，explorer的树控件增加了一种自动隐藏节点+/-标记的特性，当explorer
失去焦点时，这些三角形符号会慢慢消失 */
extern BOOL g_bNoFadeInOutExpand;
extern BOOL g_bTreeViewAutoScroll;
extern BOOL g_bTreeViewSimpleMode;
extern int g_nTreeViewIndent;
extern int g_nUpButtonIconSize;
extern BOOL g_bForceRefreshWin7;
extern BOOL g_bShowFreespaceInStatusBar;
extern BOOL g_bNoWindowResizing;

extern BOOL g_bUsingLargeButton;
extern int g_nSmallIconSize;
extern int g_nLargeIconSize;
extern BOOL g_bToolbarListMode; //工具栏按钮上显示文本
extern BOOL g_bToolbarSameSize;
extern BOOL g_bEnableSettings; //显示“设置”按钮
extern BOOL g_bBandNewLine;
extern BOOL g_bTabFixWidth;
extern BOOL g_bTabNewButton;
extern BOOL g_bTabCloseButton;
extern BOOL g_bTabAutoHideButtons;
extern BOOL g_bForceSysListView;
extern BOOL g_bSwitchNewTab; //立即切换到新打开的tab标签
extern BOOL g_bNewTabInsertBegin; //新标签插入在所有标签最前面

extern int g_nDragImageTransparent;

extern ATL::CString g_strShellNewMenuText;
extern COLORREF g_TeStatusBkColor;
