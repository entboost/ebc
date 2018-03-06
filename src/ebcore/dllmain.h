// dllmain.h : Declaration of module class.

class CebcoreModule : public CAtlDllModuleT< CebcoreModule >
{
public :
	DECLARE_LIBID(LIBID_ebcoreLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EBCORE, "{84F900D4-37D8-4FF4-AB68-D1D77CA0B80A}")
};

extern class CebcoreModule _AtlModule;
