// EBMMRoom.h : Declaration of the CEBMMRoom

#pragma once
#include "resource.h"       // main symbols

#include "ebvideoroom_i.h"
#include "_IEBMMRoomEvents_CP.h"
#include <CGCBase/cgcobject.h>
using namespace mycp;
#include "ebmm.h"
//#include "boost/shared_ptr.hpp"
#include "stl/lockmap.h"
using namespace entboost;


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CEBMMRoom

class ATL_NO_VTABLE CEBMMRoom :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEBMMRoom, &CLSID_EBMMRoom>,
	public IConnectionPointContainerImpl<CEBMMRoom>,
	public CProxy_IEBMMRoomEvents<CEBMMRoom>,
	public IDispatchImpl<IEBMMRoom, &IID_IEBMMRoom, &LIBID_ebvideoroomLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	Cebmm::pointer m_pEBMM;
	CEBMMRoom()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EBMMROOM)


BEGIN_COM_MAP(CEBMMRoom)
	COM_INTERFACE_ENTRY(IEBMMRoom)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CEBMMRoom)
	CONNECTION_POINT_ENTRY(__uuidof(_IEBMMRoomEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_pEBMM = Cebmm::create();
		return S_OK;
	}

	void FinalRelease()
	{
		m_pEBMM.reset();
	}
public:
	CLockMap<mycp::bigint,ULONG> m_pUserDataList;

	STDMETHOD(Start)(BSTR sAudioAddress, BSTR sVideoAddress, USHORT nBindPort);
	STDMETHOD(get_IsStart)(VARIANT_BOOL* pVal);
	STDMETHOD(Stop)(void);
	STDMETHOD(Register)(LONGLONG nRoomId, LONGLONG nSourceId, LONGLONG nParam, VARIANT_BOOL bIsGroup);
	STDMETHOD(get_RoomId)(LONGLONG* pVal);
	STDMETHOD(get_SourceId)(LONGLONG* pVal);
	STDMETHOD(UnRegister)(void);
	STDMETHOD(AddAudioSink)(LONGLONG nDestId);
	STDMETHOD(get_IsAudioSink)(LONGLONG nDestId,VARIANT_BOOL* pVal);
	STDMETHOD(AddVideoSink)(LONGLONG nDestId, ULONG hVideoWndParent);
	STDMETHOD(get_IsVideoSink)(LONGLONG nDestId,VARIANT_BOOL* pVal);
	STDMETHOD(DelAudioSink)(LONGLONG nDestId);
	STDMETHOD(DelVideoSink)(LONGLONG nDestId);
	STDMETHOD(DelDestSink)(LONGLONG nDestId);
	STDMETHOD(DelAllSink)(void);
	STDMETHOD(get_AudioP2PStatus)(SHORT* pVal);
	STDMETHOD(get_VideoP2PStatus)(SHORT* pVal);
	STDMETHOD(get_VideoDeviceList)(VARIANT* pVal);
	STDMETHOD(get_VideoDeviceSize)(SHORT* pVal);
	STDMETHOD(InitVideoCapture)(SHORT nVideoDeviceIndex, SHORT nVideoQuality);
	STDMETHOD(InitAudioCapture)(void);
	STDMETHOD(StartVideoCapture)(ULONG hLocalVideoWndParent);
	STDMETHOD(StopVideoCapture)(void);
	STDMETHOD(StartAudioCapture)(void);
	STDMETHOD(StopAudioCapture)(void);
	STDMETHOD(StartVideoGraph)(LONGLONG nUserId);
	STDMETHOD(MoveVideoWindow)(LONGLONG nUserId, LONG x, LONG y, LONG width, LONG height);
	STDMETHOD(ShowVideoWindow)(LONGLONG nUserId, VARIANT_BOOL bShow);
	STDMETHOD(SaveVideoBitmapFile)(LONGLONG nUserId, BSTR sSaveToFile);
	STDMETHOD(get_LocalVideoSend)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LocalVideoSend)(VARIANT_BOOL newVal);
	STDMETHOD(get_LocalAudioSend)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LocalAudioSend)(VARIANT_BOOL newVal);
	STDMETHOD(get_LocalVideoPlay)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LocalVideoPlay)(VARIANT_BOOL newVal);
	STDMETHOD(get_LocalAudioPlay)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LocalAudioPlay)(VARIANT_BOOL newVal);
	STDMETHOD(get_SinkVideoPlay)(LONGLONG nUserId, VARIANT_BOOL* pVal);
	STDMETHOD(put_SinkVideoPlay)(LONGLONG nUserId, VARIANT_BOOL newVal);
	STDMETHOD(get_SinkAudioPlay)(LONGLONG nUserId, VARIANT_BOOL* pVal);
	STDMETHOD(put_SinkAudioPlay)(LONGLONG nUserId, VARIANT_BOOL newVal);
	STDMETHOD(SetUserData)(LONGLONG nUserId, ULONG nUserData);
};

OBJECT_ENTRY_AUTO(__uuidof(EBMMRoom), CEBMMRoom)
