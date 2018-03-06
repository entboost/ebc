// dllmain.h : Declaration of module class.

class CebvideoroomModule : public CAtlDllModuleT< CebvideoroomModule >
{
public :
	DECLARE_LIBID(LIBID_ebvideoroomLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EBVIDEOROOM, "{E8A482BB-268A-405F-9E3D-6D8026E8A454}")
};

extern class CebvideoroomModule _AtlModule;
