// EBVRoom.h : Declaration of the CEBVRoom

#pragma once
#include "resource.h"       // main symbols

#include "ebvideoroom_i.h"
#include "_IEBVRoomEvents_CP.h"
#include "videoroom.h"
#include "boost/shared_ptr.hpp"
#include "stl/lockmap.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif
//
//class CEBVRoom;
//class CStreamData
//{
//public:
//	typedef boost::shared_ptr<CStreamData> pointer;
//	static CStreamData::pointer create(CEBVRoom* pOwner, LONGLONG nUserId, ULONG nUserData)
//	{
//		return CStreamData::pointer(new CStreamData(pOwner, nUserId, nUserData));
//	}
//	CEBVRoom *	m_pOwner;
//	LONGLONG	m_nUserId;
//	ULONG		m_nUserData;
//	CStreamData(void)
//		: m_pOwner(NULL)
//		, m_nUserId(0)
//		, m_nUserData(0)
//	{
//	}
//	CStreamData(CEBVRoom* pOwner, LONGLONG nUserId, ULONG nUserData)
//		: m_pOwner(pOwner)
//		, m_nUserId(nUserId)
//		, m_nUserData(nUserData)
//	{
//	}
//};
//
//
//// CEBVRoom
//
//class ATL_NO_VTABLE CEBVRoom :
//	public CComObjectRootEx<CComSingleThreadModel>,
//	public CComCoClass<CEBVRoom, &CLSID_EBVRoom>,
//	public IConnectionPointContainerImpl<CEBVRoom>,
//	public CProxy_IEBVRoomEvents<CEBVRoom>,
//	public IDispatchImpl<IEBVRoom, &IID_IEBVRoom, &LIBID_ebvideoroomLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
//{
//public:
//	Cvideoroom * m_videoroom;
//	CEBVRoom()
//		: m_videoroom(NULL)
//	{
//	}
//
//DECLARE_REGISTRY_RESOURCEID(IDR_EBVROOM)
//
//
//BEGIN_COM_MAP(CEBVRoom)
//	COM_INTERFACE_ENTRY(IEBVRoom)
//	COM_INTERFACE_ENTRY(IDispatch)
//	COM_INTERFACE_ENTRY(IConnectionPointContainer)
//END_COM_MAP()
//
//BEGIN_CONNECTION_POINT_MAP(CEBVRoom)
//	CONNECTION_POINT_ENTRY(__uuidof(_IEBVRoomEvents))
//END_CONNECTION_POINT_MAP()
//
//CLockMap<LONGLONG,CStreamData::pointer> m_pVideoData;	// userid->
//CLockMap<LONGLONG,CStreamData::pointer> m_pAudioData;	// userid->
//
//void ProcessVideoStream2(LONGLONG nUserId, BYTE* pData, DWORD nSize, void *lParam)
//{
//	CStreamData::pointer pStreamData;
//	if (!m_pVideoData.find(nUserId,pStreamData))
//	{
//		return;
//	}
//	Fire_onVideoStream(nUserId,pData,nSize,pStreamData->m_nUserData);
//}
//static void ProcessAudioStream(LONGLONG nUserId,char *pdata, int length, void* uData);
//void ProcessAudioStream2(LONGLONG nUserId, BYTE* pData, ULONG nSize, ULONG nUserData)
//{
//	Fire_onAudioStream(nUserId,pData,nSize,nUserData);
//}
//
//	DECLARE_PROTECT_FINAL_CONSTRUCT()
//
//	HRESULT FinalConstruct()
//	{
//		return S_OK;
//	}
//
//	void FinalRelease()
//	{
//		m_pVideoData.clear();
//		m_pAudioData.clear();
//		VR_UnInit();
//	}
//
//public:
//
//	STDMETHOD(VR_GetVideoDevices)(VARIANT* pVal);
//	STDMETHOD(VR_Init)(LONG nLocalId);
//	STDMETHOD(VR_UnInit)(void);
//	STDMETHOD(VR_SetVideoServer)(BSTR sAddress, USHORT nPort);
//	STDMETHOD(VR_SetAudioServer)(BSTR sAddress, USHORT nPort);
//	STDMETHOD(VR_InitVideoCapture)(USHORT nVideoIndex, USHORT nVideoQuality);
//	STDMETHOD(VR_InitAudioCapture)(void);
//	STDMETHOD(get_VR_VideoType)(USHORT* pVal);
//	STDMETHOD(get_VR_VideoSend)(VARIANT_BOOL* pVal);
//	STDMETHOD(put_VR_VideoSend)(VARIANT_BOOL newVal);
//	STDMETHOD(get_VR_AudioSend)(VARIANT_BOOL* pVal);
//	STDMETHOD(put_VR_AudioSend)(VARIANT_BOOL newVal);
//	STDMETHOD(put_VR_StartVideoCapture)(VARIANT_BOOL newVal);
//	STDMETHOD(put_VR_StartAudioCapture)(VARIANT_BOOL newVal);
//	STDMETHOD(VR_AddMeLooks)(LONGLONG nUserId);
//	STDMETHOD(VR_DelMeLook)(LONGLONG nUserId);
//	STDMETHOD(VR_AddLookMes)(LONGLONG nUserId);
//	STDMETHOD(VR_DelLookMes)(LONGLONG nUserId);
//	STDMETHOD(VR_AddVideoStream)(LONGLONG nUserId, ULONG nUserData, VARIANT_BOOL bLocalVideo);
//	STDMETHOD(VR_DelVideoStream)(LONGLONG nUserId);
//	STDMETHOD(VR_SetLocalAudiolStream)(LONGLONG nUserId, ULONG nUserData);
//	STDMETHOD(VR_AddAudioStream)(LONGLONG nUserId, ULONG nUserData);
//	STDMETHOD(VR_DelAudioStream)(LONGLONG nUserId);
//	STDMETHOD(VR_PauseAudioStream)(LONGLONG nUserId);
//	STDMETHOD(VR_PlayAudioStream)(LONGLONG nUserId);
//};
//
//OBJECT_ENTRY_AUTO(__uuidof(EBVRoom), CEBVRoom)
