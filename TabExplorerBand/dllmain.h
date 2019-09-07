// dllmain.h : Declaration of module class.

class CTabExplorerModule : public ATL::CAtlDllModuleT< CTabExplorerModule >
{
public :
	DECLARE_LIBID(LIBID_TabExplorerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TABEXPLORER, "{3fc345a8-4fda-4fb7-8762-50de64a370fe}")
};

extern class CTabExplorerModule _AtlModule;
