// EBEmotionView.cpp : Implementation of CEBEmotionView
#include "stdafx.h"
#include "EBEmotionView.h"
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")


// CEBEmotionView
void CEBEmotionView::PositionScrollBars(void)
{
	CRect windowRect;
	this->GetClientRect(&windowRect);
	CRect vBar(	windowRect.right-const_vbar_width,
				windowRect.top,
				windowRect.right,
				windowRect.bottom);
	if ( m_pSkinVBar.m_hWnd != NULL)
	{
		m_pSkinVBar.SetWindowPos(NULL,vBar.left,vBar.top,vBar.Width(),vBar.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		if (m_pSkinVBarInterface != NULL)
			m_pSkinVBarInterface->UpdatePosition();
	}
}

STDMETHODIMP CEBEmotionView::AddEmotion(BSTR sEmotionFilePath, BSTR sDescription, ULONG nUserData)
{
	// TODO: Add your implementation code here
	// 检查文件是否存在
	//if (!PathFileExists(sEmotionFilePath))
	//{
	//	return S_FALSE;
	//}
	size_t nDrawIndex = m_pImageDrawList.size();
	int x = m_nLeftInterval + (nDrawIndex%m_nHCount)*(m_nEmotionWidth+m_nLineInterval);
	int y = m_nTopInterval + (nDrawIndex/m_nHCount)*(m_nEmotionHeight+m_nLineInterval);
	Gdiplus::Rect rect = Gdiplus::Rect(x,y,m_nEmotionWidth,m_nEmotionHeight);
	m_pImageDrawList.push_back(CEmotionInfo::create(sEmotionFilePath,sDescription,rect,nDrawIndex,nUserData));

	int nLineCount = (m_pImageDrawList.size()+m_nHCount-1)/m_nHCount;
	int nPage = m_nVCount;
	m_nScrollMax = std::max<int>(0,nLineCount-nPage);
	return S_OK;
}

STDMETHODIMP CEBEmotionView::MoveWindow(LONG x, LONG y, LONG width, LONG height, VARIANT_BOOL bRepaint)
{
	//char lpszText[100];
	//sprintf(lpszText,"0x%x",m_ctlStatic.m_hWnd);
	//MessageBoxA(NULL,lpszText,"",MB_OK);
	//if (m_ctlStatic.m_hWnd != NULL)
	//	m_ctlStatic.MoveWindow(x,y,width,height,bRepaint?TRUE:FALSE);
	//if (m_hWnd != NULL)
	//	::MoveWindow(m_hWnd,x,y,width,height,bRepaint?TRUE:FALSE);
	return S_OK;
}

LRESULT CEBEmotionView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(TIMERID_DRAW_FOCUS_IMAGE);
	m_pSkinVBarInterface.Release();
	m_pSkinVBar.DestroyWindow();
	return 0;
}

void CEBEmotionView::InvalidateFocusInfo(void)
{
	if (m_image!=NULL && m_pMouseFocusInfo.get() != NULL)
	{
		Rect layoutRect(m_pMouseFocusInfo->m_rect.GetLeft()-1,m_pMouseFocusInfo->m_rect.GetTop()-1,m_image->GetWidth()+2,m_image->GetHeight()+2);
		layoutRect.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
		CRect rect(layoutRect.GetLeft(),layoutRect.GetTop(),layoutRect.GetRight(),layoutRect.GetBottom());
		const int nIndex = m_pMouseFocusInfo->m_nIndex;
		if ((nIndex%m_nHCount)==m_nHCount-1)		// 这是最右边选项
		{
			rect.left -= (m_image->GetWidth()-m_pMouseFocusInfo->m_rect.Width);
			rect.right -= (m_image->GetWidth()-m_pMouseFocusInfo->m_rect.Width);
		}
		const int nCurrentLine = (nIndex+m_nHCount)/m_nHCount;
		if ((nCurrentLine-m_nScrollPos)==m_nVCount)		// 这是最下边选项
		{
			rect.top -= (m_image->GetHeight()-m_pMouseFocusInfo->m_rect.Height);
			rect.bottom -= (m_image->GetHeight()-m_pMouseFocusInfo->m_rect.Height);
		}
		this->InvalidateRect(&rect,FALSE);

		Rect layoutRectTt(m_pMouseFocusInfo->m_rect.GetLeft()+m_image->GetWidth(),m_pMouseFocusInfo->m_rect.GetTop()-1,100,24);
		layoutRectTt.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
		if ((nIndex%m_nHCount)==(m_nHCount-1) || (nIndex%m_nHCount)==(m_nHCount-2))		// 这是最右边选项
		{
			layoutRectTt = Rect(layoutRect.GetLeft()-100,layoutRect.GetTop()-1,100,24);
		}
		CRect rectTt(layoutRectTt.GetLeft(),layoutRectTt.GetTop(),layoutRectTt.GetRight(),layoutRectTt.GetBottom());
		this->InvalidateRect(&rectTt,FALSE);

		//if (m_image)
		//{
		//	delete m_image;
		//	m_image = NULL;
		//}
		if (m_pFrameDelay)
		{
			free(m_pFrameDelay);
			m_pFrameDelay = NULL;
		}
	}
}
void CEBEmotionView::DrawFucusInfo(HDC hDC,bool bResetImage)
{
	Gdiplus::Graphics m_graphics(hDC);
	if (bResetImage && m_pMouseFocusInfo.get()!=NULL)
	{
		if (m_pMouseFocusInfo->m_pImage==NULL)
			m_pMouseFocusInfo->m_pImage = new Gdiplus::Image(m_pMouseFocusInfo->m_sEmotionFile.c_str());
		this->SetImage(m_pMouseFocusInfo->m_pImage);
		m_pMouseFocusInfo->m_nDrawDescriptionCount = 0;
		//this->SetImage(new Gdiplus::Image(m_pMouseFocusInfo->m_sEmotionFile.c_str()));
		//if (m_image!=NULL && !m_pMouseFocusInfo->m_sDescription.empty())
		//{
		//	// 写字
		//	Gdiplus::SolidBrush m_brush1(Gdiplus::Color::Yellow);
		//	Gdiplus::SolidBrush m_brush(Gdiplus::Color::Black);
		//	RectF layoutLastLineRect(m_pMouseFocusInfo->m_rect.GetLeft()+m_image->GetWidth()+2,m_pMouseFocusInfo->m_rect.GetTop(),100,20);
		//	RectF stringLineRectTemp;
		//	m_graphics.MeasureString(m_pMouseFocusInfo->m_sDescription.c_str(), -1, m_font, layoutLastLineRect, &stringLineRectTemp);
		//	const int nIndex = m_pMouseFocusInfo->m_nIndex;
		//	if ((nIndex%m_nHCount)==m_nHCount-1)		// 这是最右边选项
		//		stringLineRectTemp.X -= (m_image->GetWidth()+stringLineRectTemp.Width);
		//	m_graphics.FillRectangle(&m_brushBackground,stringLineRectTemp);
		//	m_graphics.DrawRectangle(&m_penBorder,stringLineRectTemp);
		//	m_graphics.DrawString(m_pMouseFocusInfo->m_sDescription.c_str(),-1,m_font,PointF(layoutLastLineRect.GetLeft(),layoutLastLineRect.GetTop()),&m_brush);
		//}
	}else if (m_image == NULL)
	{
		return;
	}

	Rect layoutRect(m_pMouseFocusInfo->m_rect.GetLeft()-1,m_pMouseFocusInfo->m_rect.GetTop()-1,m_image->GetWidth()+1,m_image->GetHeight()+1);
	layoutRect.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
	const int nIndex = m_pMouseFocusInfo->m_nIndex;
	if ((nIndex%m_nHCount)==(m_nHCount-1))		// 这是最右边选项
		layoutRect.X -= (m_image->GetWidth()-m_pMouseFocusInfo->m_rect.Width);
	const int nCurrentLine = (nIndex+m_nHCount)/m_nHCount;
	if ((nCurrentLine-m_nScrollPos)==m_nVCount)		// 这是最下边选项
		layoutRect.Y -= (m_image->GetHeight()-m_pMouseFocusInfo->m_rect.Height);
	//HDC hDC = this->GetDC();
	//SetBkMode(hDC,TRANSPARENT);
	DrawGif(m_graphics,layoutRect,timeGetTime());
	//this->ReleaseDC(hDC);

	if (m_fontTt!=NULL && !m_pMouseFocusInfo->m_sDescription.empty() && (m_pMouseFocusInfo->m_nDrawDescriptionCount)<2)
	{
		// 写字
		m_pMouseFocusInfo->m_nDrawDescriptionCount++;
		RectF stringLineRectTemp;
		m_graphics.MeasureString(m_pMouseFocusInfo->m_sDescription.c_str(), -1, m_fontTt, PointF(m_pMouseFocusInfo->m_rect.GetLeft()+m_image->GetWidth()+1+2,m_pMouseFocusInfo->m_rect.GetTop()-1), &stringLineRectTemp);
		if ((nIndex%m_nHCount)==(m_nHCount-1) || (nIndex%m_nHCount)==(m_nHCount-2))		// 这是最右边选项
			stringLineRectTemp = RectF(layoutRect.GetLeft()-stringLineRectTemp.Width-1-2,m_pMouseFocusInfo->m_rect.GetTop()-1,stringLineRectTemp.Width,stringLineRectTemp.Height);
		stringLineRectTemp.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
		m_graphics.FillRectangle(&m_brushTtBg,stringLineRectTemp);
		m_graphics.DrawRectangle(&m_penBorderTt,stringLineRectTemp);
		m_graphics.DrawString(m_pMouseFocusInfo->m_sDescription.c_str(),-1,m_fontTt,PointF(stringLineRectTemp.GetLeft()+1,stringLineRectTemp.GetTop()+1),&m_brushTt);
	}

}

void CEBEmotionView::SetImage(Gdiplus::Image * pImage)
{
	//boost::mutex::scoped_lock lock(m_mutex);
	//if (m_image)
	//{
	//	delete m_image;
	//	m_image = NULL;
	//}
	if (m_pFrameDelay)
	{
		free(m_pFrameDelay);
		m_pFrameDelay = NULL;
	}
	m_image = pImage;
	m_nFrameCount = 1;
	m_nCurrentFrame = 0;
	m_dwCurrentTime = 0;
	if (m_image)
	{
		// 计算 图像帧数 多帧（比如GIF）
		UINT count = m_image->GetFrameDimensionsCount();
		GUID *pDimensionIDs = (GUID*)new GUID[count];
		m_image->GetFrameDimensionsList(pDimensionIDs,count);
		WCHAR strGuid[39];
		StringFromGUID2(pDimensionIDs[0],strGuid,39);
		m_nFrameCount = m_image->GetFrameCount(&pDimensionIDs[0]);
		delete []pDimensionIDs;
		UINT size = m_image->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pFrameDelay = (Gdiplus::PropertyItem*)malloc(size);
		m_image->GetPropertyItem(PropertyTagFrameDelay,size,m_pFrameDelay);
	}
}
void CEBEmotionView::DrawGif(Graphics& pGraphics,const Rect& layoutRect,DWORD dwNowTime)
{
	if (m_image != NULL && m_pFrameDelay != NULL)
	{
		if (m_nFrameCount > 1)
		{
			const long nDelay = ((long*)m_pFrameDelay->value)[m_nCurrentFrame]*10;	// ms
			if (m_dwCurrentTime+nDelay>dwNowTime)
				return;	// 未到显示时间
			// 设置下一帧
			m_nCurrentFrame++;
			if(m_dwCurrentTime==0 || m_nFrameCount==m_nCurrentFrame)
			{
				m_nCurrentFrame = 0;
			}
			m_dwCurrentTime = dwNowTime;
			GUID Guid = FrameDimensionTime;
			m_image->SelectActiveFrame(&Guid,m_nCurrentFrame);
		}
		// 显示图像
		pGraphics.DrawImage(m_image, layoutRect.GetLeft()+1,layoutRect.GetTop()+1);//,layoutRect.Width-2,layoutRect.Height-2);
		pGraphics.DrawRectangle(&m_penBorder,layoutRect);
	}
}
LRESULT CEBEmotionView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	DWORD nIDEvent = wParam;
	if (TIMERID_DRAW_FOCUS_IMAGE==nIDEvent)
	{
		if (this->IsWindowVisible())	// 当前界面显示才有效
		{
			POINT pos;
			::GetCursorPos(&pos);
			ScreenToClient(&pos);

			HDC hDC = this->GetDC();
			SetBkMode(hDC,TRANSPARENT);
			bool bMouseFocused = false;
			for (size_t i=0; i<m_pImageDrawList.size(); i++)
			{
				int nCurrentLine = i/m_nHCount;
				if ((nCurrentLine-m_nScrollPos)==m_nVCount) break;	// 已经画多少行
				if (nCurrentLine<m_nScrollPos) continue;							// 前面几行不画

				const CEmotionInfo::pointer& pImageDrawInfo = m_pImageDrawList[i];
				Gdiplus::Rect rectReal(pImageDrawInfo->m_rect);
				rectReal.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
				CRect rect(rectReal.GetLeft(),rectReal.GetTop(),rectReal.GetRight(),rectReal.GetBottom());
				if (rect.PtInRect(pos))
				{
					// 鼠标移动
					if (m_pMouseFocusInfo.get()!=NULL && m_pMouseFocusInfo.get()==pImageDrawInfo.get())
					{
						// 继续当前选择项
						DrawFucusInfo(hDC,false);
					}else
					{
						if (m_pMouseFocusInfo.get()!=NULL && m_pMouseFocusInfo.get()!=pImageDrawInfo.get())
						//if (m_pMouseFocusInfo.get()!=NULL && !m_pMouseFocusInfo->m_sEmotionFile.empty())
						{
							// 换一个选择
							InvalidateFocusInfo(); // 刷新之前背景
							m_pMouseFocusInfo = pImageDrawInfo;
							DrawFucusInfo(hDC,true);
						}else
						{
							// 第一个选中
							m_pMouseFocusInfo = pImageDrawInfo;
							DrawFucusInfo(hDC,true);
						}
					}
					bMouseFocused = true;
					break;
				}
			}
			if (!bMouseFocused && m_pMouseFocusInfo.get()!=NULL)
			{
				// 当前没有选项，但前面有选项
				InvalidateFocusInfo();	// 刷新之前背景
				m_pMouseFocusInfo.reset();
			}
			this->ReleaseDC(hDC);
		}
	}

	return 0;
}

STDMETHODIMP CEBEmotionView::SetBorderColor(BYTE r, BYTE g, BYTE b)
{
	m_penBorder.SetColor(Gdiplus::Color(r,g,b));
	return S_OK;
}
STDMETHODIMP CEBEmotionView::SetBorderColor2(BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_penBorder.SetColor(Gdiplus::Color(a,r,g,b));
	return S_OK;
}
STDMETHODIMP CEBEmotionView::SetBackGroundColor(BYTE r, BYTE g, BYTE b)
{
	m_brushBackground.SetColor(Gdiplus::Color(r,g,b));
	return S_OK;
}
STDMETHODIMP CEBEmotionView::SetBackGroundColor2(BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_brushBackground.SetColor(Gdiplus::Color(a,r,g,b));
	return S_OK;
}

STDMETHODIMP CEBEmotionView::GetScrollMax(LONG* pOutMax)
{
	*pOutMax = m_nScrollMax;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::GetScrollMin(LONG* pOutMin)
{
	*pOutMin = 1;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::GetScrollPos(LONG* pOutPos)
{
	*pOutPos = m_nScrollPos;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::ScrollPageUp(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == 0)
		return S_FALSE;
	int nPage = m_nVCount;
	m_nScrollPos -= (nPage-1);
	if (m_nScrollPos < 0)
	{
		m_nScrollPos = 0;
	}
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBEmotionView::ScrollPageDown(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	int nPage = m_nVCount;
	m_nScrollPos += (nPage-1);
	if (m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBEmotionView::ScrollLineUp(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == 0)
		return S_FALSE;
	m_nScrollPos -= 1;
	if (m_nScrollPos < 0)
	{
		m_nScrollPos = 0;
	}
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBEmotionView::ScrollLineDown(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	m_nScrollPos += 1;
	if (m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBEmotionView::ScrollBy(LONG nScrollTimes)
{
	// TODO: Add your implementation code here
	if (nScrollTimes > 0)
	{
		// 向下拉动
		if (m_nScrollPos == m_nScrollMax)
			return S_FALSE;
		m_nScrollPos += nScrollTimes;
		if (m_nScrollPos > m_nScrollMax)
			m_nScrollPos = m_nScrollMax;
		this->FireViewChange();
	}else if (nScrollTimes < 0)
	{
		// 向上拉动
		if (m_nScrollPos == 0)
			return S_FALSE;
		m_nScrollPos -= (nScrollTimes*-1);
		if (m_nScrollPos < 0)
		{
			m_nScrollPos = 0;
		}
		this->FireViewChange();
	}
	return S_OK;
}
STDMETHODIMP CEBEmotionView::ScrollTo(LONG nScrollPos)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == nScrollPos)
		return S_FALSE;
	else if (nScrollPos < 0 && m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	else if (nScrollPos > m_nScrollMax && m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	m_nScrollPos = nScrollPos;
	if (m_nScrollPos < 0 || m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBEmotionView::OnVBarKillFocus(void)
{
	HWND hFocus = ::GetFocus();
	if (hFocus != this->m_hWnd)
	{
		::PostMessage(::GetParent(this->m_hWnd),WM_KILLFOCUS,(WPARAM)hFocus,0);
	}
	return S_OK;
}

LRESULT CEBEmotionView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// 不刷新背景
	return TRUE;
	return 0;
}
#define WM_EV_CLICK_EMOTION		(WM_USER+0x111)

LRESULT CEBEmotionView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//bHandled = FALSE;
	//RelayEvent(uMsg, (WPARAM)wParam,MAKELPARAM(LOWORD(lParam), HIWORD(lParam)));
	CPoint pos;
	::GetCursorPos(&pos);
	ScreenToClient(&pos);

	for (size_t i=0; i<m_pImageDrawList.size(); i++)
	{
		int nCurrentLine = i/m_nHCount;
		if ((nCurrentLine-m_nScrollPos)==m_nVCount) break;	// 已经画多少行
		if (nCurrentLine<m_nScrollPos) continue;						// 前面几行不画

		const CEmotionInfo::pointer& pImageDrawInfo = m_pImageDrawList[i];
		Gdiplus::Rect rectReal(pImageDrawInfo->m_rect);
		rectReal.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
		CRect rect(rectReal.GetLeft(),rectReal.GetTop(),rectReal.GetRight(),rectReal.GetBottom());
		if (rect.PtInRect(pos))
		{
			::PostMessage(::GetParent(this->m_hWnd),WM_EV_CLICK_EMOTION,pImageDrawInfo->m_dwUserData,0);
			//this->GetParent()->PostMessage(WM_EV_CLICK_EMOTION,pImageDrawInfo.m_dwUserData);
			break;
		}
	}
	return 0;
}


LRESULT CEBEmotionView::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	short zDelta = HIWORD(wParam);
	int nScrollTimes = (-1*zDelta);
	ScrollBy(nScrollTimes);

	if (m_pSkinVBarInterface != NULL)
		m_pSkinVBarInterface->UpdatePosition();

	return 0;
}

STDMETHODIMP CEBEmotionView::get_EmotionWidth(USHORT* pVal)
{
	*pVal = m_nEmotionWidth;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_EmotionWidth(USHORT newVal)
{
	m_nEmotionWidth = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_EmotionHeight(USHORT* pVal)
{
	*pVal = m_nEmotionHeight;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_EmotionHeight(USHORT newVal)
{
	m_nEmotionHeight = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_TopInterval(USHORT* pVal)
{
	*pVal = m_nTopInterval;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_TopInterval(USHORT newVal)
{
	m_nTopInterval = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_LeftInterval(USHORT* pVal)
{
	*pVal = m_nLeftInterval;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_LeftInterval(USHORT newVal)
{
	m_nLeftInterval = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_LineInterval(USHORT* pVal)
{
	*pVal = m_nLineInterval;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_LineInterval(USHORT newVal)
{
	m_nLineInterval = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_HCount(USHORT* pVal)
{
	*pVal = m_nHCount;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_HCount(USHORT newVal)
{
	m_nHCount = newVal;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::get_VCount(USHORT* pVal)
{
	*pVal = m_nVCount;
	return S_OK;
}

STDMETHODIMP CEBEmotionView::put_VCount(USHORT newVal)
{
	m_nVCount = newVal;
	return S_OK;
}

LRESULT CEBEmotionView::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	HWND hFocus = ::GetFocus();
	if (hFocus != this->m_pSkinVBar.m_hWnd)
	{
		::PostMessage(::GetParent(this->m_hWnd),WM_KILLFOCUS,(WPARAM)hFocus,0);
	}
	return 0;
}

STDMETHODIMP CEBEmotionView::get_EmotionCount(ULONG* pVal)
{
	*pVal = m_pImageDrawList.size();
	return S_OK;
}
//
//LRESULT CEBEmotionView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
//{
//	 TODO: Add your message handler code here and/or call default
//	if (m_wndToolTip.m_hWnd)
//	{
//		POINT pt;
//		static uId;
//		pt.x = LOWORD(lParam);
//		pt.y = HIWORD(lParam);
//
//		TOOLINFO ti;
//		ti.cbSize = sizeof(TOOLINFO);
//		ti.hwnd   = m_hTheWnd;
//		ti.uId    = 1;
//		ti.rect.left   = pt.x;
//		ti.rect.right  = pt.x+1;
//		ti.rect.top    = pt.y;
//		ti.rect.bottom = pt.y+1;
//		m_wndToolTip.SendMessage(TTM_NEWTOOLRECT,   0, (LPARAM)&ti);
//
//		RelayEvent(uMsg, wParam, lParam);
//	}
//	return 0;
//}
