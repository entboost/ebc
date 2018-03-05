// EBImage.cpp : Implementation of CEBImage
#include "stdafx.h"
#include "EBImage.h"
#ifdef USES_THREAD_DRAW
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#endif

// CEBImage

STDMETHODIMP CEBImage::LoadImageEx(BSTR strImageFile,LONG hParentHwnd)
{
	// TODO: Add your implementation code here
	//boost::mutex::scoped_lock lock(m_mutex);
	m_sImageFile = strImageFile;
	m_hParentHwnd = (HWND)hParentHwnd;
	if (m_image)
	{
		delete m_image;
		m_image = NULL;
	}
	if (m_pFrameDelay)
	{
		free(m_pFrameDelay);
		m_pFrameDelay = NULL;
	}
	m_image = new Gdiplus::Image(strImageFile);
	//m_image = new Gdiplus::Bitmap(strImageFile,PixelFormat24bppRGB);
	m_sizePicture.cx = m_image->GetWidth();
	m_sizePicture.cy = m_image->GetHeight();

	SIZEL szlPixels;
	szlPixels.cx = m_sizePicture.cx;
	szlPixels.cy = m_sizePicture.cy;
	AtlPixelToHiMetric(&szlPixels, &m_sizeExtent);
	// IOleObjectImpl
	//SetExtent(DVASPECT_CONTENT, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	// update control sizing...
	//m_rcPos.right= m_rcPos.left + m_sizePicture.cx;
	//m_rcPos.bottom= m_rcPos.top + m_sizePicture.cy;
	//if (m_spInPlaceSite != NULL) {
	//	// needed for IE to accept the resizing
	//	m_spInPlaceSite->OnPosRectChange(&m_rcPos);
	//}
	//SetFocus();

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
#ifdef USES_THREAD_DRAW
	m_bKilled = false;
	if (m_pDrawGifThread==NULL)
		m_pDrawGifThread = new boost::thread(boost::bind(draw_gif_thread_svr, (void*)this));
#endif
	return S_OK;
}

#ifdef USES_THREAD_DRAW
void CEBImage::draw_gif_thread_svr(void* lparam)
{
	CEBImage * pSvr = (CEBImage*)lparam;
	pSvr->OnDrawGif();
}
void CEBImage::OnDrawGif(void)
{
	while(!m_bKilled)
	{
		Sleep(100);
		DrawGif(timeGetTime());
	}
}
#endif

void CEBImage::DrawGif(DWORD dwNowTime)
{
	if (m_image != NULL && m_nFrameCount > 1 && m_pFrameDelay != NULL)
	{
		const long nDelay = ((long*)m_pFrameDelay->value)[m_nCurrentFrame]*10;	// ms
		if (m_dwCurrentTime+nDelay>dwNowTime)
			return;	// 未到显示时间
		{
			m_dwCurrentTime = dwNowTime;
			// 设置下一帧
			GUID Guid = FrameDimensionTime;
			m_nCurrentFrame++;
			if(m_nFrameCount==m_nCurrentFrame)
			{
				m_nCurrentFrame = 0;
			}
			m_image->SelectActiveFrame(&Guid,m_nCurrentFrame);
		}
		// 使用这个，外面必须设置背景明透明，所以没有采用
		//if (m_hParentHwnd)
		//	::InvalidateRect(m_hParentHwnd,&m_rcPos,FALSE);
		// 使用这个，外面不需要设置背景透明
		// 但使用内部线程有时会挂死，外部不会挂死 需要多测试
		this->FireViewChange();
	}
}


STDMETHODIMP CEBImage::get_ShowImage(VARIANT_BOOL* pVal)
{
	*pVal = m_bShowImage;
	return S_OK;
}

STDMETHODIMP CEBImage::put_ShowImage(VARIANT_BOOL newVal)
{
	m_bShowImage = newVal;
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBImage::ShowNextFrame(LONG nNowTime)
{
	// TODO: Add your implementation code here
	DrawGif(nNowTime);
	return S_OK;
}

STDMETHODIMP CEBImage::GetImagePath(BSTR* pOutImagePath)
{
	// TODO: Add your implementation code here
	*pOutImagePath = ::SysAllocString(m_sImageFile.c_str());
	return S_OK;
}

STDMETHODIMP CEBImage::SetColorKey(LONG colorLow, LONG colorHigh)
{
	if (m_pImageAttributes == NULL)
	{
		m_pImageAttributes = new Gdiplus::ImageAttributes();
	}
	Gdiplus::Color pColorLow(GetRValue(colorLow),GetGValue(colorLow),GetBValue(colorLow));
	Gdiplus::Color pColorHigh(GetRValue(colorHigh),GetGValue(colorHigh),GetBValue(colorHigh));
	m_pImageAttributes->SetColorKey(pColorLow,pColorHigh);
	return S_OK;
}

STDMETHODIMP CEBImage::ClearColorKey(void)
{
	// TODO: Add your implementation code here
	if (m_pImageAttributes != NULL)
	{
		m_pImageAttributes->ClearColorKey();
		// ?
		//delete m_pImageAttributes;
		//m_pImageAttributes = NULL;
	}
	return S_OK;
}
