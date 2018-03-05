// dllmain.h : Declaration of module class.

class CebcontrolModule : public CAtlDllModuleT< CebcontrolModule >
{
public :
	DECLARE_LIBID(LIBID_ebcontrolLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EBCONTROL, "{45F301F0-EED3-41A8-A8ED-3308D045A603}")
};

extern class CebcontrolModule _AtlModule;
