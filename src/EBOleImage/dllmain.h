// dllmain.h : Declaration of module class.

class CEBOleImageModule : public CAtlDllModuleT< CEBOleImageModule >
{
public :
	DECLARE_LIBID(LIBID_EBOleImageLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EBOLEIMAGE, "{03F3B783-053F-4614-AE42-68FF490CE902}")
};

extern class CEBOleImageModule _AtlModule;
