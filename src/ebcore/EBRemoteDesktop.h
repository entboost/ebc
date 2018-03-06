// EBRemoteDesktop.h : Declaration of the CEBRemoteDesktop

#pragma once
#include "resource.h"       // main symbols

#include "ebcore_i.h"
#include "ebrd.h"
using namespace entboost;


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CEBRemoteDesktop

class ATL_NO_VTABLE CEBRemoteDesktop :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEBRemoteDesktop, &CLSID_EBRemoteDesktop>,
	public IDispatchImpl<IEBRemoteDesktop, &IID_IEBRemoteDesktop, &LIBID_ebcoreLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CEBRemoteDesktop()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EBREMOTEDESKTOP)


BEGIN_COM_MAP(CEBRemoteDesktop)
	COM_INTERFACE_ENTRY(IEBRemoteDesktop)
	COM_INTERFACE_ENTRY(IDispatch)
	//COM_INTERFACE_ENTRY(IProvideClassInfo)	// 支持脚本接收事件
	//COM_INTERFACE_ENTRY(IProvideClassInfo2)	// 支持脚本接收事件
END_COM_MAP()

// 实现脚本安全
BEGIN_CATEGORY_MAP(CEBRemoteDesktop)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		m_rd.reset();
	}

	Cebrd::pointer m_rd;
public:
	STDMETHOD(RD_Init)(BSTR sAddress, ULONG nBindPort);
	STDMETHOD(RD_IsInited)(VARIANT_BOOL * pVal);
	STDMETHOD(RD_UnInit)(void);

	STDMETHOD(RD_Register)(ULONGLONG nRoomId, ULONGLONG nSourceId, ULONGLONG nParam, VARIANT_BOOL bIsGroup);
	STDMETHOD(get_RD_RoomId)(ULONGLONG* pVal);
	STDMETHOD(get_RD_SourceId)(ULONGLONG* pVal);
	STDMETHOD(RD_UnRegister)(void);

	STDMETHOD(RD_AddSink)(ULONGLONG nDestId, ULONG nParentWnd);
	STDMETHOD(RD_MoveSinkWindow)(ULONGLONG nDestId, LONG x, LONG y, LONG width, LONG height, VARIANT_BOOL bRepaint);
	STDMETHOD(get_RD_ControlSinkDesktop)(ULONGLONG nDestId, LONG* pVal);
	STDMETHOD(put_RD_ControlSinkDesktop)(ULONGLONG nDestId, LONG nMouseMoveFPS);
	STDMETHOD(RD_DelSink)(ULONGLONG nDestId);
	STDMETHOD(RD_DelAllSink)(void);

	STDMETHOD(RD_StartLocalDesktop)(ULONG quant, ULONG fps);
	STDMETHOD(get_RD_LocalDesktopFPS)(ULONG* pVal);
	STDMETHOD(get_RD_EnableControlLocalDesktop)(ULONGLONG* pVal);
	STDMETHOD(put_RD_EnableControlLocalDesktop)(ULONGLONG newVal);
	STDMETHOD(RD_StopLocalDesktop)(void);

};

OBJECT_ENTRY_AUTO(__uuidof(EBRemoteDesktop), CEBRemoteDesktop)
