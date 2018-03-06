// EBVWindow.cpp : Implementation of CEBVWindow
#include "stdafx.h"
#include "EBVWindow.h"
#include "atltypes.h"

//
//// CEBVWindow
//STDMETHODIMP CEBVWindow::VW_BuildVideoWindow(SHORT nVideoType, SHORT nVideoWidth, SHORT nVideoHeight)
//{
//	if (m_pVideoWindow != NULL) return S_FALSE;
//	CRect rect;
//	this->GetClientRect(&rect);
//	m_pVideoWindow = new Cvideowindow(this->m_hWnd,nVideoType,nVideoWidth,nVideoHeight,rect.left,rect.top,rect.right,rect.bottom);
//	return S_OK;
//}
//
//STDMETHODIMP CEBVWindow::VW_BuildGraph(VARIANT_BOOL bDefaultRender)
//{
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	return m_pVideoWindow->VW_BuildGraph(bDefaultRender?true:false);
//}
//
//STDMETHODIMP CEBVWindow::VW_StartGraph(void)
//{
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	return m_pVideoWindow->VW_StartGraph();
//}
//
//STDMETHODIMP CEBVWindow::VW_StopGraph(void)
//{
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	return m_pVideoWindow->VW_StopGraph();
//}
//
//STDMETHODIMP CEBVWindow::VW_ReleaseGraph(void)
//{
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	return m_pVideoWindow->VW_ReleaseGraph();
//}
//
//STDMETHODIMP CEBVWindow::VW_DrawVideo(BYTE* pVideoData, ULONG nVideoSize)
//{
//	if (pVideoData==NULL || nVideoSize==0) return E_INVALIDARG;
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	return m_pVideoWindow->VW_DrawVideo(pVideoData,nVideoSize);
//}
//
//STDMETHODIMP CEBVWindow::VW_DrawVideo2(VARIANT* pVideoData, ULONG nVideoSize)
//{
//	if (pVideoData==NULL || pVideoData->byref == NULL|| nVideoSize==0) return E_INVALIDARG;
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	m_pVideoWindow->VW_DrawVideo(pVideoData->byref,nVideoSize);
//	return S_OK;
//}
//
//LRESULT CEBVWindow::OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//{
//	// ???
//	if (m_pVideoWindow!=NULL)
//	{
//		//m_pVideoWindow->VW_ShowVideoWindow();
//	}
//	return 0;
//}
//
//STDMETHODIMP CEBVWindow::VW_SaveBitmapFile(BSTR sSaveImageTo)
//{
//	// TODO: Add your implementation code here
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	USES_CONVERSION;
//	bool ret = m_pVideoWindow->VW_SaveBitmapFile(W2A(sSaveImageTo));
//	return ret==true?S_OK:S_FALSE;
//}
//STDMETHODIMP CEBVWindow::VW_ShowVideoWindow(VARIANT_BOOL newVal)
//{
//	// TODO: Add your implementation code here
//	if (m_pVideoWindow==NULL) return E_FAIL;
//	m_pVideoWindow->VW_ShowVideoWindow(newVal==VARIANT_TRUE?true:false);
//	return S_OK;
//}
