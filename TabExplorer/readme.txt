选定一个目录并打开一个文件：

WinExec("explorer   /n,/select,   c:\\1.txt",   SW_SHOWNORMAL   );
  

ShellExecute(NULL, _T("open"), _T("Explorer"), _T("/select, C:\\1.txt"), NULL, SW_SHOW);

HRESULT GetShellFolderViewDual(ITEMIDLIST* pidl, IShellFolderViewDual** ppIShellFolderViewDual)
{
IWebBrowserApp* pIWebBrowserApp;
IDispatch* pDoc;
HWND hWnd;
HRESULT hr;
HINSTANCE ghSHDOCVW; HRESULT (WINAPI*gpfSHGetIDispatchForFolder)(ITEMIDLIST* pidl, IWebBrowserApp** ppIWebBrowserApp); *ppIShellFolderViewDual = NULL; ghSHDOCVW = LoadLibrary(_T("SHDOCVW.DLL"));
if (ghSHDOCVW == NULL)
return FALSE; pIWebBrowserApp=NULL;
gpfSHGetIDispatchForFolder =
(HRESULT (WINAPI*)(ITEMIDLIST*, IWebBrowserApp**)) GetProcAddress(ghSHDOCVW, "SHGetIDispatchForFolder");
if (gpfSHGetIDispatchForFolder == NULL)
return FALSE;
if (SUCCEEDED(gpfSHGetIDispatchForFolder(pidl, &pIWebBrowserApp)))
{
if (SUCCEEDED(pIWebBrowserApp->get_HWND((long*)&hWnd)))
{
SetForegroundWindow(hWnd);
ShowWindow(hWnd, SW_SHOWNORMAL);
} if (SUCCEEDED(hr = pIWebBrowserApp->get_Document(&pDoc)))
{
pDoc->QueryInterface(IID_IShellFolderViewDual, (void**) ppIShellFolderViewDual);
pDoc->Release();
} pIWebBrowserApp->Release();
}
FreeLibrary(ghSHDOCVW); return TRUE;
}BOOL OpenFolderAndSelectItems(ITEMIDLIST *pidlFolder)
{
ITEMIDLIST *pidl, *pidl2;
ITEMIDLIST* pIdlFile;
USHORT cb;
IShellFolderViewDual* pIShellFolderViewDual; HRESULT (WINAPI *gpfSHOpenFolderAndSelectItems)(LPCITEMIDLIST *pidlFolder, UINT cidl, LPCITEMIDLIST *apidl, DWORD dwFlags);
HINSTANCE ghShell32; /// 只有WinXp及以上及系统才支持SHOpenFolderAndSelectItems() API
/// 那其它系统该怎么实现这个功能呢？只能采用其它的方法来处理
/// 首先用XP跟踪到SHOpenFolderAndSelectItems()API中，看它是如何处理的，再用同样的方法去实现
/// 其它系统的这个功能使用工具 VC6 .net 2003 MSDN Ollydbg v1.10中文版 ghShell32 = LoadLibrary(_T("Shell32.DLL"));
if (ghShell32 == NULL)
return FALSE; gpfSHOpenFolderAndSelectItems =
(HRESULT (WINAPI*)(LPCITEMIDLIST*, UINT, LPCITEMIDLIST*, DWORD)) GetProcAddress(ghShell32, "SHOpenFolderAndSelectItems");
if (gpfSHOpenFolderAndSelectItems != NULL)
{
/// 可以获得SHOpenFolderAndSelectItems()函数的API地址
if (SUCCEEDED(gpfSHOpenFolderAndSelectItems((LPCITEMIDLIST*)pidlFolder,0,(LPCITEMIDLIST*)NULL,0)))
{
///直接调用系统的功能
FreeLibrary(ghShell32);
return TRUE;
}
FreeLibrary(ghShell32);
return FALSE;
}
FreeLibrary(ghShell32); /// 当操作系统不支持SHOpenFolderAndSelectItems()函数的API时的处理，
/// 自已动手写一个与系统功能相同的代码
pidl = pidlFolder;
pIdlFile = pidl;
/// 找出目标文件中文件名的偏移量
while (cb = pIdlFile->mkid.cb)
{
pidl2 = pIdlFile;
pIdlFile = (ITEMIDLIST*)((BYTE*)pIdlFile + cb);
} cb = pidl2->mkid.cb;
pidl2->mkid.cb = 0; /// 打开目标文件所在的文件夹
if (SUCCEEDED(GetShellFolderViewDual(pidl, &pIShellFolderViewDual)))
{
pidl2->mkid.cb = cb;
// 0 Deselect the item.
// 1 Select the item.
// 3 Put the item in edit mode.
// 4 Deselect all but the specified item.
// 8 Ensure the item is displayed in the view.
// 0x10 Give the item the focus.
COleVariant bszFile(pidl2);
if(pIShellFolderViewDual != NULL)
{
/// 选中相应的选项
pIShellFolderViewDual->SelectItem(bszFile, 0x1d);
pIShellFolderViewDual->Release();
}
return TRUE;
}
return FALSE;
}void FindTarget(CString str)
{
HRESULT hres;
IShellLink *psl;
ITEMIDLIST *pidl;
IPersistFile *ppf; // Get a pointer to the IShellLink interface.
hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
IID_IShellLink, (LPVOID*)&psl);
if (SUCCEEDED(hres))
{
// 设置目标文件
psl->SetPath(str);
/// 获得目标文件的ITEMIDLIST
psl->GetIDList(&pidl); // Get a pointer to the IPersistFile interface.
hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
if (SUCCEEDED(hres))
{
WCHAR wsz[MAX_PATH];
#ifdef _UNICODE
wcscpy(wsz, str);
#else
// Ensure that the string is Unicode.
MultiByteToWideChar(CP_ACP, 0, str, -1, wsz, MAX_PATH);
#endif // Load the shortcut.
hres = ppf->Load(wsz, STGM_READ);
if (SUCCEEDED(hres))
{
/// 获得快捷方式的ITEMIDLIST
psl->GetIDList(&pidl);
}
ppf->Release();
} /// 打开文件夹并选中项目
OpenFolderAndSelectItems(pidl);
psl->Release();
}
}


创建和自动化Internet Explorer和资源管理器窗口：

我在很久之前就开始用程序自动化Shell窗口——主要对象是IE窗口。有时浏览器控件或者MFC类CHTMLView可以满足我的需要，但是很多时候我需要从头嵌入浏览器控件并且尽可能模拟IE的行为，例如实现IDocHostUIHandler来启用自动完成功能。一个很自然的替代方案是直接操作IE窗口。
创建新的Internet Explorer窗口

最简单的方法是调用Windows API ShellExecute (Ex)，Paul DiLascia在他的C++ Q&A专栏文章"Browser Detection Revisited, Toolbar Info, IUnknown with COM and MFC"里面有一段示例代码:

/// As I've shown in many programs... ShellExecute(0, _T("open"), pszMyHTMLFile, 0, 0, SW_SHOWNORMAL);
但是，这样没法控制新的窗口，而且在用户关闭程序之后会留下一个IE窗口。为了扫我自己的门前雪，我需要找到我创建的窗口，并且控制它。

我的下一个尝试是创建和控制一个InternetExplorer对象，并且在必要时关掉它。微软知识库中有这么一篇文章"How To Automate Internet Explorer to POST Form Data" 基本上描述的就是我想要的，除了最后的关闭窗口。嗯，简单的调用IWebBrowser2::Quit就可以做到这一点

// create a new IE instance and show it //CComQIPtr m_pWebBrowser2; m_pWebBrowser2.CoCreateInstance(CLSID_InternetExplorer); HRESULT hr; hr = m_pWebBrowser2->put_StatusBar(VARIANT_TRUE); hr = m_pWebBrowser2->put_ToolBar(VARIANT_TRUE); hr = m_pWebBrowser2->put_MenuBar(VARIANT_TRUE); hr = m_pWebBrowser2->put_Visible(VARIANT_TRUE); if(!::PathIsURL(m_strFileToFind)) m_strFileToFind=_T("http://blog.joycode.com/jiangsheng"); COleVariant vaURL( ( LPCTSTR) m_strFileToFind); m_pWebBrowser2->Navigate2( &vaURL, COleVariant( (long) 0, VT_I4), COleVariant((LPCTSTR)NULL, VT_BSTR), COleSafeArray(), COleVariant((LPCTSTR)NULL, VT_BSTR) ); void CAutomationDlg::OnDestroy() { //close the IE window created by this program before exit if(m_pWebBrowser2) { if(m_bOwnIE) { m_pWebBrowser2->Quit(); m_bOwnIE=FALSE; } UnadvisesinkIE(); m_pWebBrowser2=(LPUNKNOWN)NULL; } CDialog::OnDestroy(); }
还有一个问题。要是用户在我的WM_TIMER处理函数中操作窗口之前关掉了新的IE窗口怎么办？可以IWebBrowser2接口控制的IE对象现在不再存在了。幸亏微软考虑到了这一点，程序不会崩溃，但是最好还是能够知道什么时候它会关闭，这样我可以避免意外发生。
处理Internet Explorer的事件

Internet Explorer对象在退出时会触发DWebBrowserEvents2::OnQuit事件。这是一个理想的释放控制的时机。因为对象要被销毁，所以我同时也停止监视对象的事件

if(m_pWebBrowser2) { UnadvisesinkIE(); m_pWebBrowser2=(LPUNKNOWN)NULL; }
连接到当前的Internet Explorer窗口

虽然我不在乎我会控制到哪个IE窗口，但是既然微软知识库里面有"如何连接到一个Internet Explorer的实例"这样一篇文章，我假定一些人会觉得"如何连接到当前的Internet Explorer实例"这样一篇文章比较有用.

这样的话，什么是“当前的Internet Explorer实例”？实际上，它就是最后一个活动的IE窗口。因为Windows会把活动的窗口移动到z-order的顶部，所以它会保留在所有IE窗口的z-order的最高处。因此我需要做的就是找到哪个IE窗口具有最高的z-order值。这样我需要先判断哪个窗口是IE窗口。在一些和Spy++有关的调查之后，我假定IE窗口具有一个共同的窗口类"IEFrame"，然后编写了一个函数来获得Shell窗口的窗口类：

//shell windows object will list both IE and Explorer windows //use their window class names to identify them. CString CAutomationDlg::GetWindowClassName(IWebBrowser2* pwb) { TCHAR szClassName[_MAX_PATH]; ZeroMemory( szClassName, _MAX_PATH * sizeof( TCHAR)); HWND hwnd=NULL; if (pwb) { LONG_PTR lwnd=NULL; pwb->get_HWND(&lwnd); hwnd=reinterpret_cast(lwnd); ::GetClassName( hwnd, szClassName, _MAX_PATH); } return szClassName; }
剩下的问题就很简单了：沿Z轴枚举顶层窗口，找到第一个Shell窗口列表中的具有窗口类"IEFrame"的第一个实例。之后我操作了一下IE的DHTML文档对象模型(也称为DOM，它只在IE窗口触发最后一个DocumentComplete事件只后有效)来确认成功连接到窗口。

void CAutomationDlg::DocumentComplete(IDispatch *pDisp, VARIANT *URL) { //HTML DOM is available AFTER the DocumentComplete event is fired. //For more information, please visit KB article //"How To Determine When a Page Is Done Loading in WebBrowser Control" //http://support.microsoft.com/kb/q180366/ CComQIPtr pWBUK(m_pWebBrowser2); CComQIPtr pSenderUK( pDisp); USES_CONVERSION; TRACE( _T( "Page downloading complete:/r/n")); CComBSTR bstrName; m_pWebBrowser2->get_LocationName(&bstrName); CComBSTR bstrURL; m_pWebBrowser2->get_LocationURL(&bstrURL); TRACE( _T( "Name:[ %s ]/r/nURL: [ %s ]/r/n"), OLE2T(bstrName), OLE2T(bstrURL)); if (pWBUK== pSenderUK) { CComQIPtr pHTMLDocDisp; m_pWebBrowser2->get_Document(&pHTMLDocDisp); CComQIPtr pHTMLDoc(pHTMLDocDisp); CComQIPtr ecAll; CComPtr pTagLineDisp; if(pHTMLDoc) { CComBSTR bstrNewTitle(_T("Sheng Jiang's Automation Test")); pHTMLDoc->put_title(bstrNewTitle); pHTMLDoc->get_all(&ecAll); } if(ecAll) { ecAll->item(COleVariant(_T("tagline")),COleVariant((long)0),&pTagLineDisp); } CComQIPtr eTagLine(pTagLineDisp); if(eTagLine) { eTagLine->put_innerText( CComBSTR(_T("Command what is yours, conquer what is not. --Kane"))); } } }
现在控制的窗口和IE打开文件时选择的一样了。
副产品: 连接到当前的Windows Explorer窗口

在研究ShellWindows对象的shell窗口列表时，我获得一个副产品：看起来Windows Explorer窗口也有共同的窗口类名。这样同样的机制在把窗口类从"IEFrame"改成"ExploreWClass"之后对Windows Explorer窗口也适用。因为没有DHTML DOM可供操作，我通知Windows Explorer 窗口打开一个现存路径，来标志我接管了这个窗口。

//show the folder bar COleVariant clsIDFolderBar(_T("{EFA24E64-B078-11d0-89E4-00C04FC9E26E}")); COleVariant FolderBarShow(VARIANT_TRUE,VT_BOOL); COleVariant dummy; if(m_pWebBrowser2) m_pWebBrowser2->ShowBrowserBar(&clsIDFolderBar,&FolderBarShow,&dummy); //browse to a given folder CComQIPtr psp(m_pWebBrowser2); CComPtr psb; if(psp) psp->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(LPVOID*)&psb); if(psb) { USES_CONVERSION; LPITEMIDLIST pidl=NULL; SFGAOF sfgao; SHParseDisplayName (T2OLE(m_strFileToFind),NULL,&pidl,0, &sfgao); if(pidl==NULL) ::SHGetSpecialFolderLocation(m_hWnd,CSIDL_DRIVES,&pidl); m_pidlToNavigate=NULL; if(pidl) { //if the start address is a folder, then browse it. //otherwise browse to its parent folder, and select it in the folder view. LPCITEMIDLIST pidlChild=NULL; CComPtr psf; HRESULT hr = SHBindToParent(pidl, IID_IShellFolder, (LPVOID*)&psf, &pidlChild); if (SUCCEEDED(hr)){ SFGAOF rgfInOut=SFGAO_FOLDER; hr=psf->GetAttributesOf(1,&pidlChild,&rgfInOut); if (SUCCEEDED(hr)){ m_pidlToNavigate=ILClone(pidl); if(rgfInOut&SFGAO_FOLDER){//this is a folder psb->BrowseObject(pidl,SBSP_SAMEBROWSER); } else { //this is a file, browse to the parent folder LPITEMIDLIST pidlParent=ILClone(pidl); ::ILRemoveLastID(pidlParent); psb->BrowseObject( pidlParent, SBSP_SAMEBROWSER); ILFree(pidlParent); } } } //clean up ILFree(pidl); } }:
这代码有点长，因为我想区别对待文件和文件夹。如果你调用IShellBrowser::BrowseObject并且给这个方法传递一个文件pidl，那么Windows Explorer会提示你是否打开这个文件，就像在资源管理器的地址栏中输入路径之后按回车一样。我想模拟"Explorer.exe /select"的行为，在文件夹视图中选择指定的文件，所以我在DocumentComplete事件处理函数中加入了一些代码：

if(m_pidlToNavigate) { //If the start address is a file, browse to the parent folder //and then select it CComQIPtr psp(m_pWebBrowser2); CComPtr psb; CComPtr psv; if(psp) psp->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(LPVOID*)&psb); if(psb) psb->QueryActiveShellView(&psv); if(psv) { LPCITEMIDLIST pidlChild=NULL; CComPtr psf; SFGAOF rgfInOut=SHCIDS_ALLFIELDS; HRESULT hr = SHBindToParent(m_pidlToNavigate, IID_IShellFolder, (LPVOID*)&psf, &pidlChild); if (SUCCEEDED(hr)){ hr=psf->GetAttributesOf(1,&pidlChild,&rgfInOut); if (SUCCEEDED(hr)){ if((rgfInOut&SFGAO_FOLDER)==0){ //a file, select it hr=psv->SelectItem(ILFindLastID(m_pidlToNavigate) ,SVSI_SELECT|SVSI_ENSUREVISIBLE|SVSI_FOCUSED| SVSI_POSITIONITEM); } } } } //clean up ILFree(m_pidlToNavigate); m_pidlToNavigate=NULL; }
创建Explorer窗口

解决了这么多问题，可以衣锦还乡了。既然我可以以和当前的Internet Explorer窗口基本相同的方式连接到当前的Windows Explorer窗口，那么我是否可以以和创建Internet Explorer窗口基本相同的方式创建Windows Explorer窗口？遗憾的是，这不可行。不存在Windows Explorer对应的类ID来创建一个COM对象。虽然我仍旧可以创建IE窗口，浏览到文件夹，显示文件夹侧边栏，使得它看起来就像一个Windows Explorer窗口，但是我不能改变窗口类"IEFrame"，因此较难把它和其他的显示HTML网页和活动文档的IE窗口区分开来。

好吧，既然我不能以COM的方式来创建它，我还可以尝试用传统的方式。我可以创建一个explorer.exe进程之后查找其主窗口，就像Paul DiLascia 在他的文章"Get the Main Window, Get EXE Name"中演示的那样，并且发送未文档化的消息WM_GETISHELLBROWSER来获得窗口的IShellBrowser接口:

//start the new process STARTUPINFO si; PROCESS_INFORMATION pi; ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) ); // Start the child process. if( !CreateProcess( NULL, // No module name (use command line). _T("explorer.exe"), // Command line. NULL, // Process handle not inheritable. NULL, // Thread handle not inheritable. FALSE, // Set handle inheritance to FALSE. 0, // No creation flags. NULL, // Use parent's environment block. NULL, // Use parent's starting directory. &si, // Pointer to STARTUPINFO structure. &pi ) // Pointer to PROCESS_INFORMATION structure. ) //wait a graceful time //so the window is created and is ready to answer messages. ::WaitForInputIdle(pi.hProcess,1000); //m_hExplorerProcess=(DWORD)pi.hProcess; EnumWindows(EnumWindowsProc,(LPARAM)this); BOOL CALLBACK CAutomationDlg::EnumWindowsProc(HWND hwnd,LPARAM lParam) { CAutomationDlg* pdlg=(CAutomationDlg*)lParam; DWORD pidwin; GetWindowThreadProcessId(hwnd, &pidwin); if (pidwin==pdlg->m_hExplorerProcess) { IShellBrowser* psb=(IShellBrowser*)::SendMessage(hwnd,WM_USER+7,0,0); CComQIPtr pwb(psb); return FALSE; } return TRUE; }
啊喔，这在我的计算机上也没有效果。怎么回事？在我的资源管理器的文件夹选项中，“在同一窗口中打开每一个文件夹”被选中，所以新的Windows Explorer窗口被创建在现有的Windows Explorer进程中。看起来这是条死胡同。

等一下，我手头还有另一个ShellWindows对象，它可以给我一个Shell窗口的列表，包含每一个Windows Explorer窗口和每个窗口对应的IWebBrowser2接口，这是到IShellBrowser接口的入口。.现在我需要获得两份shell窗口列表，创建explorer.exe进程之前和之后各一份，之后要比较它们来找到新的shell窗口：

m_pShellWindows.CoCreateInstance(CLSID_ShellWindows); if(m_pShellWindows) { //get the list of running IE windows //using the ShellWindows collection //For more information, please visit //http://support.microsoft.com/kb/176792 long lCount=0; m_pShellWindows->get_Count(&lCount); for(long i=0;i pdispShellWindow; m_pShellWindows->Item(COleVariant(i),&pdispShellWindow); if(pdispShellWindow) { m_listShellWindows.AddTail(new CComQIPtrIDispatch(pdispShellWindow)); } } } //enumerate through the new shell window list long lCount=0; m_pShellWindows->get_Count(&lCount); for(long i=0;i//search the new window //using the ShellWindows collection //For more information, please visit //http://support.microsoft.com/kb/176792 BOOL bFound=FALSE; CComPtr pdispShellWindow; m_pShellWindows->Item(COleVariant(i),&pdispShellWindow); //search it in the old shell window list POSITION pos=m_listShellWindows.GetHeadPosition(); while(pos) { CComQIPtrIDispatch* pDispatch=m_listShellWindows.GetNext(pos); if(pDispatch&&pdispShellWindow.p==pDispatch->p) { bFound=TRUE;break; } } if(!bFound)//new window found { //attach to it m_pWebBrowser2=pdispShellWindow; m_bOwnIE=TRUE; //sink for the Quit and DocumentComplete events AdviseSinkIE(); NavigateToSamplePage(FALSE); } }
等一下，你的"创建explorer.exe进程之后"是什么意思?一秒钟之后？还是两秒钟？实际上，一个WindowRegistered事件会被ShellWindows 对象触发，所以我在事件处理中加入一些代码：.

//sink DShellWindowsEvents events LPUNKNOWN pUnkSink = GetIDispatch(FALSE); m_pShellWindows.CoCreateInstance(CLSID_ShellWindows); AfxConnectionAdvise((LPUNKNOWN)m_pShellWindows, DIID_DShellWindowsEvents,pUnkSink,FALSE,&m_dwCookieShellWindows); void CAutomationDlg::WindowRegistered(long lCookie) { //ok, a new shell window is created if(m_pShellWindows) { //enumerate through the new shell window list long lCount=0; m_pShellWindows->get_Count(&lCount); for(long i=0;i//search the new window //using the ShellWindows collection //For more information, please visit //http://support.microsoft.com/kb/176792 BOOL bFound=FALSE; CComPtr pdispShellWindow; m_pShellWindows->Item(COleVariant(i),&pdispShellWindow); //search it in the old shell window list POSITION pos=m_listShellWindows.GetHeadPosition(); while(pos) { CComQIPtrIDispatch* pDispatch=m_listShellWindows.GetNext(pos); if(pDispatch&&pdispShellWindow.p==pDispatch->p) { bFound=TRUE;break; } } if(!bFound)//new window { //attach to it m_pWebBrowser2=pdispShellWindow; m_bOwnIE=TRUE; //sink for the Quit and DocumentComplete events AdviseSinkIE(); NavigateToSamplePage(FALSE); } } //clean up if(m_dwCookieShellWindows!= 0) { LPUNKNOWN pUnkSink = GetIDispatch(FALSE); AfxConnectionUnadvise((LPUNKNOWN)m_pShellWindows, DIID_DShellWindowsEvents, pUnkSink, FALSE, m_dwCookieShellWindows); m_dwCookieShellWindows= 0; } POSITION pos=m_listShellWindows.GetHeadPosition(); while(pos) { CComQIPtrIDispatch* pDispatch=m_listShellWindows.GetNext(pos); delete pDispatch; } m_listShellWindows.RemoveAll(); m_pShellWindows=(LPUNKNOWN)NULL; } }
为什么不用Browser Helper Objects?

因为新的窗口在进程外，所以跨进程列集COM调用很慢。如果你的自动化操作包含很多的COM调用，那么你可能要把代码本地化，例如编写一个浏览器辅助对象(BHO)。但是，BHO会被每一个Windows Explorer和Internet Explorer的实例加载，而且我不想拖慢整个系统来让它们扫瓦上霜。一些人倒是使用了这个技术连接到当前的Internet Explorer窗口.
已知问题

ShellWindows对象在explorer.exe process被终止或者尚未启动时不可访问。BHO在这种情况下可以作为替代方案。
结论

这里有一大堆让人迷糊的代码，而且可能还有你不熟悉的COM和Windows API函数混合调用。希望你会觉得本文有用，并且不会被我的代码搞得头昏脑胀。自动化Internet Explorer和Windows Explorers窗口可以节省你模拟系统默认行为的时间，并且给用户提供一个熟悉的界面。
参考

    Browser Detection Revisited, Toolbar Info, IUnknown with COM and MFC
    Connecting to Running Instances of Internet Explorer
    Connecting to a running instance of Internet Explorer
    Get the Main Window, Get EXE Name
    How to connect to a running instance of Internet Explorer
    How to Connect to IExplorer in Citrix Environments
    PRB: IShellFolder::CreateViewObject() causes access violation
    Querying information from an Explorer window
    ShellWindows Object (Windows Explorer and Controls)



    ATL与网页进行事件交互:
    以前我们为了编写一个漂亮的程序，需要用VC进行客户区非客户区重绘，需要维护图片载入、GDI绘制、消息处理、重载各种控件……，虽然很多人都是这么做的，包括我最近做的项目中，仍然使用GDI在做很多界面上的事情。但是，目前似乎程序结合Web页面，共同去做些事情仿佛越来越流行。当然，界面编程还是离不开GDI或GDI+，我这里只是记录一些我认为未来在写软件、做项目中可能会使用的一些技术——当然，这种技术已经很多人都在用了，而且肯定用的比我好，希望玩儿的比较溜的多包涵文中的错误和不足。

从今天开始，我计划用1、2个月的时间陆续写一些关于使用ATL/WTL和网页交互的文字，包括使用COM来操作网页元素，以及对网页元素事件的响应。 

无论是用MFC/ATL/WTL，无论是Insert一个Microsoft Web Browser的ActiveX控件，还是动态创建了一个Browser实例，甚至是你要直接操作Windows系统的IE，我们首先要做的事情就是获得IE Browser的接口。

我们首先来编写一个WTL的示例程序开始说明。

（未完待续）

2008年1月16日
最近一段时间实在是太忙了，刚刚把新产品的需求分析弄完。下一个产品的需求分析马上又要来了。郁闷。
现在编码的工作基本都交代出去了，自己已经没有多少时间编码了，很多新的技术和细节我已经不擅长了，不知道这是好是坏。不过最近项目管理的事情倒是学到了不少。

废话少说，我还是尽量抽空把这个文章写完。

使用WTL向导做一个标准的对话框程序出来，记得要把Enable ActiveX Control Hosting的选项勾上。然后丢一个Microsoft Web Browser的ActiveX控件上去，ID比方说是IDC_EXPLORER。

想让对话框能够接收WebBrowser控件的事件，需要为对话框增加一些继承类。首先我们定义这样一个东东：
#define WebBrowser IDispEventImpl<IDC_EXPLORER, CMainDlg, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>

然后添加到对话框的继承列表中，如下：
class CMainDlg : public CAxDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
        public CMessageFilter, public CIdleHandler, public WebBrowser

到stdafx.h中添加这几个必要的头文件：
#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>
OK，我们希望处理的是对话框上那个WebBrowser控件的事件，因此，需要维护一个该控件的成员变量：
CComPtr<IWebBrowser2> m_pWeb2;
接下来，选择我们希望处理的网页事件。首先添加消息响应宏，然后添加两个事件，如下：
BEGIN_SINK_MAP(CMainDlg)
        SINK_ENTRY_EX(IDC_EXPLORER, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete) 
        SINK_ENTRY_EX(IDC_EXPLORER, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
    END_SINK_MAP()
我们上面希望处理两个事件：OnBeforeNavigate2和OnDocumentComplete。下面是这两个事件的实现体：
void __stdcall OnDocumentComplete(IDispatch *, VARIANT* )
{
    MessageBox(_T("Current document complete!"),_T("Info"),MB_OK|MB_ICONEXCLAMATION);
}

void __stdcall CMainDlg::OnBeforeNavigate2 (
    IDispatch* pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName,
    VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel )
{
    MessageBox(_T("Before navigate!"),_T("Info"),MB_OK|MB_ICONEXCLAMATION);
};

下面我们让对话框启动时，让嵌入的Web Browser ActiveX控件直接打开一个网页，在OnInitDialog中添加下面的代码：
CAxWindow AxWin = GetDlgItem(IDC_EXPLORER);
AxWin.QueryControl(&m_pWeb2);

if (m_pWeb2)
{
    WebBrowser::DispEventAdvise(m_pWeb2);
    m_pWeb2->Navigate(CComBSTR("http://blog.csdn.net/epubcn/"),NULL,NULL,NULL,NULL);
}
记得在OnDestroy中释放网页事件监控：
if (m_pWeb2)
{
    WebBrowser::DispEventUnadvise(m_pWeb2);
    m_pWeb2.Release();
}

OK，现在编译运行一下是否会弹出预期的消息对话框。

注：如果运行时发生ATLASSERT(!InlineIsEqualGUID(*m_plibid,GUID_NULL) && "Did you forget to pass the LIBID to CComModule::Init?");错误，请将工程设置中的“ATL使用”改为“动态链接到ATL”即可。

上面是对网页事件的响应。接下来让我们来处理网页上某个元素的事件。比如一个BUTTON的单击、超级链接的点击、一个复选框按钮是否被选中、文本输入框内容的改变和获取等。




















