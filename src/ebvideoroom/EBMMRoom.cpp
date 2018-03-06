// EBMMRoom.cpp : Implementation of CEBMMRoom

#include "stdafx.h"
#include "EBMMRoom.h"
#include "atlsafe.h"
#include <CGCBase/cgcSmartString.h>
//#include "../include/ebstring.h"

#ifdef _DEBUG
#pragma comment(lib, "libebmmd.lib")
#else
#pragma comment(lib, "libebmm.lib")
#endif

// CEBMMRoom
static void MY_FPMMCallBack(mycp::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData)
{
	CEBMMRoom * pEBMMRoom = (CEBMMRoom*)pUserData;
	if (pEBMMRoom!=NULL)
	{
		ULONG nVideoUserData = 0;
		pEBMMRoom->m_pUserDataList.find(nUserId,nVideoUserData);
		pEBMMRoom->Fire_onVideoCallBack(nUserId, nCallBackType, lParam, wParam, nVideoUserData);
	}
}


STDMETHODIMP CEBMMRoom::Start(BSTR sAudioAddress, BSTR sVideoAddress, USHORT nBindPort)
{
	USES_CONVERSION;
	const int ret = m_pEBMM->Start(W2A(sAudioAddress),W2A(sVideoAddress),nBindPort);
	m_pEBMM->SetCallBack(MY_FPMMCallBack,this);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::get_IsStart(VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->IsStart()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::Stop(void)
{
	m_pEBMM->Stop();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::Register(LONGLONG nRoomId, LONGLONG nSourceId, LONGLONG nParam, VARIANT_BOOL bIsGroup)
{
	// TODO: Add your implementation code here
	const bool ret = m_pEBMM->Register(nRoomId, nSourceId, nParam, bIsGroup==VARIANT_TRUE?true:false);
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::get_RoomId(LONGLONG* pVal)
{
	*pVal = m_pEBMM->GetRoomId();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_SourceId(LONGLONG* pVal)
{
	*pVal = m_pEBMM->GetSourceId();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::UnRegister(void)
{
	m_pEBMM->UnRegister();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::AddAudioSink(LONGLONG nDestId)
{
	const bool ret = m_pEBMM->AddAudioSink(nDestId);
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::get_IsAudioSink(LONGLONG nDestId,VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->IsAudioSink(nDestId)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::AddVideoSink(LONGLONG nDestId, ULONG hVideoWndParent)
{
	const bool ret = m_pEBMM->AddVideoSink(nDestId,(HWND)hVideoWndParent);
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::get_IsVideoSink(LONGLONG nDestId,VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->IsVideoSink(nDestId)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::DelAudioSink(LONGLONG nDestId)
{
	m_pEBMM->DelAudioSink(nDestId);
	m_pUserDataList.remove(nDestId);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::DelVideoSink(LONGLONG nDestId)
{
	m_pEBMM->DelVideoSink(nDestId);
	m_pUserDataList.remove(nDestId);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::DelDestSink(LONGLONG nDestId)
{
	m_pEBMM->DelVideoSink(nDestId);
	m_pUserDataList.remove(nDestId);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::DelAllSink(void)
{
	m_pEBMM->DelAllSink();
	m_pUserDataList.clear();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_AudioP2PStatus(SHORT* pVal)
{
	*pVal = m_pEBMM->GetAudioP2PStatus();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_VideoP2PStatus(SHORT* pVal)
{
	*pVal = m_pEBMM->GetVideoP2PStatus();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_VideoDeviceList(VARIANT* pVal)
{
	// TODO: Add your implementation code here
	std::vector<tstring> pDeviceList;
	Cebmm::GetVideoDevices(pDeviceList);
	if (pDeviceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pDeviceList.size(); i++)
	{
		m_sa.Add(CComVariant(tstring(pDeviceList[i]).CopyBSTR()));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_VideoDeviceSize(SHORT* pVal)
{
	*pVal = m_pEBMM->GetVideoDeviceSize();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::InitVideoCapture(SHORT nVideoDeviceIndex, SHORT nVideoQuality)
{
	const bool ret = m_pEBMM->InitVideoCapture(nVideoDeviceIndex,nVideoQuality);
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::InitAudioCapture(void)
{
	const bool ret = m_pEBMM->InitAudioCapture();
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::StartVideoCapture(ULONG hLocalVideoWndParent)
{
	m_pEBMM->StartVideoCapture((HWND)hLocalVideoWndParent);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::StopVideoCapture(void)
{
	m_pEBMM->StopVideoCapture();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::StartAudioCapture(void)
{
	m_pEBMM->StartAudioCapture();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::StopAudioCapture(void)
{
	m_pEBMM->StopAudioCapture();
	return S_OK;
}

STDMETHODIMP CEBMMRoom::StartVideoGraph(LONGLONG nUserId)
{
	const bool ret = m_pEBMM->StartVideoGraph(nUserId);
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::MoveVideoWindow(LONGLONG nUserId, LONG x, LONG y, LONG width, LONG height)
{
	const bool ret = m_pEBMM->MoveVideoWindow(nUserId, x, y, width, height);
	return ret?S_OK:S_FALSE;
}
STDMETHODIMP CEBMMRoom::ShowVideoWindow(LONGLONG nUserId, VARIANT_BOOL bShow)
{
	const bool ret = m_pEBMM->ShowVideoWindow(nUserId, bShow==VARIANT_TRUE?true:false);
	return ret?S_OK:S_FALSE;
}
STDMETHODIMP CEBMMRoom::SaveVideoBitmapFile(LONGLONG nUserId, BSTR sSaveToFile)
{
	USES_CONVERSION;
	const bool ret = m_pEBMM->SaveVideoBitmapFile(nUserId, W2A(sSaveToFile));
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBMMRoom::get_LocalVideoSend(VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetLocalVideoSend()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_LocalVideoSend(VARIANT_BOOL newVal)
{
	m_pEBMM->SetLocalVideoSend(newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_LocalAudioSend(VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetLocalAudioSend()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_LocalAudioSend(VARIANT_BOOL newVal)
{
	m_pEBMM->SetLocalAudioSend(newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_LocalVideoPlay(VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetLocalVideoPlay()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_LocalVideoPlay(VARIANT_BOOL newVal)
{
	m_pEBMM->SetLocalVideoPlay(newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_LocalAudioPlay(VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetLocalAudioPlay()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_LocalAudioPlay(VARIANT_BOOL newVal)
{
	m_pEBMM->SetLocalAudioPlay(newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_SinkVideoPlay(LONGLONG nUserId, VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetSinkVideoPlay(nUserId)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_SinkVideoPlay(LONGLONG nUserId, VARIANT_BOOL newVal)
{
	m_pEBMM->SetSinkVideoPlay(nUserId, newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::get_SinkAudioPlay(LONGLONG nUserId, VARIANT_BOOL* pVal)
{
	*pVal = m_pEBMM->GetSinkAudioPlay(nUserId)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBMMRoom::put_SinkAudioPlay(LONGLONG nUserId, VARIANT_BOOL newVal)
{
	m_pEBMM->SetSinkAudioPlay(nUserId, newVal==VARIANT_TRUE?true:false);
	return S_OK;
}

STDMETHODIMP CEBMMRoom::SetUserData(LONGLONG nUserId, ULONG nUserData)
{
	if (nUserData==0)
		m_pUserDataList.remove(nUserId);
	else
		m_pUserDataList.insert(nUserId,nUserData);
	return S_OK;
}

