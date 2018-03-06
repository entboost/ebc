// EBRemoteDesktop.cpp : Implementation of CEBRemoteDesktop

#include "stdafx.h"
#include "EBRemoteDesktop.h"
//#include "ebstring.h"

#ifdef _DEBUG
#pragma comment(lib, "libebrdd.lib")
#else
#pragma comment(lib, "libebrd.lib")
#endif

// CEBRemoteDesktop

STDMETHODIMP CEBRemoteDesktop::RD_Init(BSTR sAddress, ULONG nBindPort)
{
	if (m_rd.get()==NULL)
		m_rd = Cebrd::create();

	USES_CONVERSION_EX;
	return m_rd->Start(BSTR2A_EX(sAddress), nBindPort)==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBRemoteDesktop::RD_IsInited(VARIANT_BOOL * pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->IsStart()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBRemoteDesktop::RD_UnInit(void)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->Stop();
	m_rd.reset();
	return S_OK;
}

STDMETHODIMP CEBRemoteDesktop::RD_Register(ULONGLONG nRoomId, ULONGLONG nSourceId, ULONGLONG nParam, VARIANT_BOOL bIsGroup)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	return m_rd->Register(nRoomId, nSourceId, nParam, bIsGroup==VARIANT_TRUE?true:false)?S_OK:S_FALSE;
}
STDMETHODIMP CEBRemoteDesktop::get_RD_RoomId(ULONGLONG* pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->GetRoomId();
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::get_RD_SourceId(ULONGLONG* pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->GetSourceId();
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::RD_UnRegister(void)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->UnRegister();
	return S_OK;
}

STDMETHODIMP CEBRemoteDesktop::RD_AddSink(ULONGLONG nDestId, ULONG nParentWnd)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	return m_rd->AddSink(nDestId, (HWND)nParentWnd)?S_OK:S_FALSE;
}
STDMETHODIMP CEBRemoteDesktop::RD_MoveSinkWindow(ULONGLONG nDestId, LONG x, LONG y, LONG width, LONG height, VARIANT_BOOL bRepaint)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	return m_rd->MoveSinkWindow(nDestId, x, y, width, height, bRepaint==VARIANT_TRUE?TRUE:FALSE)?S_OK:S_FALSE;
}
STDMETHODIMP CEBRemoteDesktop::get_RD_ControlSinkDesktop(ULONGLONG nDestId, LONG* pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->ControlSinkDesktop(nDestId);
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::put_RD_ControlSinkDesktop(ULONGLONG nDestId, LONG nMouseMoveFPS)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->ControlSinkDesktop(nDestId,nMouseMoveFPS);
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::RD_DelSink(ULONGLONG nDestId)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->DelSink(nDestId);
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::RD_DelAllSink(void)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->DelAllSink();
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::RD_StartLocalDesktop(ULONG quant, ULONG fps)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	return m_rd->StartLocalDesktop(quant, fps)?S_OK:S_FALSE;
}
STDMETHODIMP CEBRemoteDesktop::get_RD_LocalDesktopFPS(ULONG* pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->GetLocalDesktopFPS();
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::get_RD_EnableControlLocalDesktop(ULONGLONG* pVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	*pVal = m_rd->EnableControlLocalDesktop();
	return S_OK;
}
STDMETHODIMP CEBRemoteDesktop::put_RD_EnableControlLocalDesktop(ULONGLONG newVal)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->EnableControlLocalDesktop(newVal);
	return S_OK;
}

STDMETHODIMP CEBRemoteDesktop::RD_StopLocalDesktop(void)
{
	if (m_rd.get()==NULL)
		return S_FALSE;
	m_rd->StopLocalDesktop();
	return S_OK;
}
