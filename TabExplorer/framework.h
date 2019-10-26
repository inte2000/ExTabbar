#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlconv.h>
#include <atlstr.h>
#include <atlcoll.h>

#define _WTL_NO_CSTRING 

using namespace ATL;

#include <atlapp.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atltheme.h>
#include <atlsplit.h>


#include <shobjidl.h>
#include <exdispid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <Shldisp.h>
#include <comdef.h>
#include <Tlogstg.h>

#include <windows.h>
#include <VersionHelpers.h>

#include <vector>
#include <string>
#include <algorithm>
#include <memory>


typedef std::basic_string<TCHAR> TString;

#include "DebugLog.h"

