// EBVRoom.cpp : Implementation of CEBVRoom

#include "stdafx.h"
#include "EBVRoom.h"
#include "atlsafe.h"
//
//#ifdef _DEBUG
//#pragma comment(lib, "videoroomd.lib")
//#else
//#pragma comment(lib, "videoroom.lib")
//#endif
//
//// CEBVRoom
//
//void ProcessVideoStream(vr::bigint nId, vr::VIDEOFRAME *pFrame, void *lParam, DWORD udata)
//{
//	//if (udata == 0) return;
//	try
//	{
//		//static bool theMessageBox = false;
//		//if (!theMessageBox)
//		//{
//		//	theMessageBox = true;
//		//	char lpsztext[50];
//		//	sprintf(lpsztext,"ProcessVideoStream %d",nId);
//		//	MessageBoxA(NULL,lpsztext,"",MB_OK);
//		//}
//		((CEBVRoom*)udata)->ProcessVideoStream2(nId,pFrame->data,pFrame->len,lParam);
//	}catch(...)
//	{
//	}
//}
//void CEBVRoom::ProcessAudioStream(LONGLONG nUserId,char *pdata, int length, void* uData)
//{
//	try
//	{
//		CStreamData* pStreamData = (CStreamData*)uData;
//		pStreamData->m_pOwner->ProcessAudioStream2(pStreamData->m_nUserId,(BYTE*)pdata,(ULONG)length,pStreamData->m_nUserData);
//	}catch(...)
//	{
//	}
//}
//
//STDMETHODIMP CEBVRoom::VR_GetVideoDevices(VARIANT* pVal)
//{
//	std::vector<std::string> pDeviceList;
//	Cvideoroom::VR_GetVideoDevices(pDeviceList);
//	if (pDeviceList.empty())
//	{
//		return S_FALSE;
//	}
//	USES_CONVERSION;
//	CComSafeArray<VARIANT> m_sa;
//	for (size_t i=0; i<pDeviceList.size(); i++)
//	{
//		m_sa.Add(CComVariant(A2W(pDeviceList[i].c_str())));
//	}
//	VariantInit(pVal);
//	CComVariant var(m_sa);
//	var.Detach(pVal);
//	return S_OK;
//}
//
//
//STDMETHODIMP CEBVRoom::VR_Init(LONG nLocalId)
//{
//	if (m_videoroom != NULL) return S_FALSE;
//	m_videoroom = new Cvideoroom();
//	int ret = m_videoroom->VR_Init(nLocalId,ProcessVideoStream);
//	return ret==0?S_OK:E_FAIL;
//}
//STDMETHODIMP CEBVRoom::VR_UnInit(void)
//{
//	if (m_videoroom == NULL) return S_FALSE;
//	m_videoroom->VR_UnInit();
//	delete m_videoroom;
//	m_videoroom = NULL;
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_SetVideoServer(BSTR sAddress, USHORT nPort)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	USES_CONVERSION;
//	m_videoroom->VR_SetVideoServer(W2A(sAddress),nPort);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_SetAudioServer(BSTR sAddress, USHORT nPort)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	USES_CONVERSION;
//	m_videoroom->VR_SetAudioServer(W2A(sAddress),nPort);
//	return S_OK;
//}
//
//
//STDMETHODIMP CEBVRoom::VR_InitVideoCapture(USHORT nVideoIndex, USHORT nVideoQuality)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	//std::vector<std::string> pOutDevices;
//	//Cvideoroom::VR_GetVideoDevices(pOutDevices);
//	bool ret = m_videoroom->VR_InitVideoCapture(nVideoIndex,nVideoQuality);
//	return ret==true?S_OK:E_FAIL;
//}
//
//STDMETHODIMP CEBVRoom::VR_InitAudioCapture(void)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	bool ret = m_videoroom->VR_InitAudioCapture();
//	return ret==true?S_OK:E_FAIL;
//}
//
//STDMETHODIMP CEBVRoom::get_VR_VideoType(USHORT* pVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	*pVal = m_videoroom->VR_GetVideoType();
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::get_VR_VideoSend(VARIANT_BOOL* pVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	*pVal = m_videoroom->VR_GetVideoSend()?VARIANT_TRUE:VARIANT_FALSE;
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::put_VR_VideoSend(VARIANT_BOOL newVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_SetVideoSend(newVal?true:false);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::get_VR_AudioSend(VARIANT_BOOL* pVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	*pVal = m_videoroom->VR_GetAudioSend()?VARIANT_TRUE:VARIANT_FALSE;
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::put_VR_AudioSend(VARIANT_BOOL newVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_SetAudioSend(newVal?true:false);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::put_VR_StartVideoCapture(VARIANT_BOOL newVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	if (newVal)
//		m_videoroom->VR_StartVideoCapture();
//	else
//		m_videoroom->VR_StopVideoCapture();
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::put_VR_StartAudioCapture(VARIANT_BOOL newVal)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	if (newVal)
//		m_videoroom->VR_StartAudioCapture();
//	else
//		m_videoroom->VR_StopAudioCapture();
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_AddMeLooks(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_AddMeLooks(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_DelMeLook(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_DelMeLook(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_AddLookMes(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_AddLookMes(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_DelLookMes(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_DelLookMes(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_AddVideoStream(LONGLONG nUserId, ULONG nUserData, VARIANT_BOOL bLocalVideo)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	CStreamData::pointer pStreamData;
//	if (!m_pVideoData.find(nUserId,pStreamData))
//	{
//		pStreamData = CStreamData::create(this,nUserId,nUserData);
//		m_pVideoData.insert(nUserId,pStreamData);
//	}else
//	{
//		pStreamData->m_nUserData = nUserData;
//	}
//	m_videoroom->VR_AddVideoStream(nUserId,(ULONG)this,bLocalVideo?true:false);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_DelVideoStream(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_DelVideoStream(nUserId);
//	m_pVideoData.remove(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_SetLocalAudiolStream(LONGLONG nUserId, ULONG nUserData)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	CStreamData::pointer pStreamData;
//	if (!m_pAudioData.find(nUserId,pStreamData))
//	{
//		pStreamData = CStreamData::create(this,nUserId,nUserData);
//		m_pAudioData.insert(nUserId,pStreamData);
//	}else
//	{
//		pStreamData->m_nUserData = nUserData;
//	}
//	m_videoroom->VR_SetLocalAudiolStream(ProcessAudioStream,pStreamData.get());
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_AddAudioStream(LONGLONG nUserId, ULONG nUserData)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	CStreamData::pointer pStreamData;
//	if (!m_pAudioData.find(nUserId,pStreamData))
//	{
//		pStreamData = CStreamData::create(this,nUserId,nUserData);
//		m_pAudioData.insert(nUserId,pStreamData);
//	}else
//	{
//		pStreamData->m_nUserData = nUserData;
//	}
//	m_videoroom->VR_AddAudioStream(nUserId,ProcessAudioStream,pStreamData.get());
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_DelAudioStream(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_DelAudioStream(nUserId);
//	m_pAudioData.remove(nUserId);	// 这个必须放在后面，否则ProcessAudioStream会有异常
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_PauseAudioStream(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_PauseAudioStream(nUserId);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVRoom::VR_PlayAudioStream(LONGLONG nUserId)
//{
//	if (m_videoroom == NULL) return E_FAIL;
//	m_videoroom->VR_PlayAudioStream(nUserId);
//	return S_OK;
//}
