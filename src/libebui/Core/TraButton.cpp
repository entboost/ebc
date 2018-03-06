
#include "stdafx.h"
#include "TraButton.h"

#define BUTTON_MSG_RE_INVALIDATE (WM_USER+110)

CTraButton::CTraButton()
{
	m_btninfoNor.pImg = m_btninfoHot.pImg = m_btninfoPre.pImg = m_btninfoDis.pImg = NULL;//底图
	m_bHot = false;//是否为Hot
	m_bPress = false;//是否按下
	m_bSelected = false;//是否选择
	m_bAutoSize = true;//自适应
	m_bAutoFocus = false;
	m_nTextLeft = 0;
	m_nTextTop = 0;
	m_crNorText = RGB(30,30,30);
	m_crHotText = RGB(30,30,30);

	m_bDrawPanel = false;
	m_crNorPanel = 0;
	m_crHotPanel = 0;
	m_crPrePanel = 0;
	m_crDisPanel = 0;
	m_bCreateRgn = true;

	m_bDrawClosePic = false;
	m_crClosePicNor = 0;
	m_crClosePicHot = 0;
	m_crClosePicPre = 0;
	m_crClosePicDis = 0;
	m_nClosePicWidth = 1;
	m_bDrawNewPic = false;
	m_crNewPicNor = 0;
	m_crNewPicHot = 0;
	m_crNewPicPre = 0;
	m_crNewPicDis = 0;
	m_nNewPicWidth = 2;
	m_nNewPicHeight = 8;
	m_nDrawTriangleType = 0;
	m_crTrianglePicNor = 0;
	m_crTrianglePicHot = 0;
	m_crTrianglePicPre = 0;
	m_crTrianglePicDis = 0;
	m_nTrianglePicWidth = 8;
	m_nTrianglePicHeight = 8;
	m_nDrawArrowType = 0;
	m_bDrawArrowLine = false;
	m_nArrowPenWidth = 2;
	m_crArrowPicNor = 0;
	m_crArrowPicHot = 0;
	m_crArrowPicPre = 0;
	m_crArrowPicDis = 0;
	m_nArrowPicWidth = 5;

	m_bDestroyDelete = false;
	m_bGrayImage = false;
	m_pImage = NULL;

	m_bTextHotMove = true;
	//m_pWndFocus = NULL;
}

CTraButton::~CTraButton()
{
	if (m_btninfoNor.pImg != NULL)
	{
		delete m_btninfoNor.pImg;
		m_btninfoNor.pImg = NULL;
	}
	if (m_btninfoHot.pImg != NULL)
	{
		delete m_btninfoHot.pImg; 
		m_btninfoHot.pImg = NULL;
	}
	if (m_btninfoPre.pImg != NULL)
	{
		delete m_btninfoPre.pImg;
		m_btninfoPre.pImg = NULL;
	}
	if (m_btninfoDis.pImg!= NULL)
	{
		delete m_btninfoDis.pImg;
		m_btninfoDis.pImg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CTraButton, CButton)
	//{{AFX_MSG_MAP(CTraButton)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	//ON_WM_TIMER()
	ON_MESSAGE(BUTTON_MSG_RE_INVALIDATE,OnMsgReInvalidate)
END_MESSAGE_MAP()


//作用：载入背景图片
void CTraButton::Load(UINT IDBkGroup, int width, int height, const CString& resourceType)
{
	CImage orgImg;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(IDBkGroup),resourceType);
	if (hRsrc == NULL)
	{
		DWORD dwError = GetLastError();
		return;
	}

	//讲资源加载到内存中
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return;

	//为流申请资源
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	//加载流
	orgImg.Load(pstm);

	//释放资源
	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	Load(orgImg,width,height,resourceType);
	//if (resourceType == _T("PNG"))
	//{
	//	if (orgImg.GetBPP() == 32)
	//	{
	//		//PNG透明
	//		for(int i = 0; i < orgImg.GetWidth(); i++)  
	//		{  
	//			for(int j = 0; j < orgImg.GetHeight(); j++)  
	//			{  
	//				unsigned char* pucColor = reinterpret_cast<unsigned char *>(orgImg.GetPixelAddress(i , j));  
	//				pucColor[0] = pucColor[0] * pucColor[3] / 255;  
	//				pucColor[1] = pucColor[1] * pucColor[3] / 255;  
	//				pucColor[2] = pucColor[2] * pucColor[3] / 255;  
	//			}  
	//		}
	//	}
	//}

	//if (width == -1)
	//{
	//	width = orgImg.GetWidth()/4;
	//	height = orgImg.GetHeight();
	//}
	//if (width==0 && height==0)
	//{
	//	width=orgImg.GetHeight();//方形
	//}
	//if (height==0)
	//{
	//	height = orgImg.GetHeight();
	//}

	//m_btninfoNor.nWidth = width;
	//m_btninfoNor.nHeight = height;
	//m_btninfoHot.nWidth = width;
	//m_btninfoHot.nHeight = height;
	//m_btninfoPre.nWidth = width;
	//m_btninfoPre.nHeight = height;
	//m_btninfoDis.nWidth = width;
	//m_btninfoDis.nHeight = height;
	//CImage** imgs[]={&m_btninfoNor.pImg,&m_btninfoHot.pImg,&m_btninfoPre.pImg,&m_btninfoDis.pImg};
	//int posX=0;
	//for (int i=0;i<4 && posX<=(orgImg.GetWidth()-width);i++,posX+=width)
	//{
	//	CImage* pMap=new CImage();
	//	if (*imgs[i] != NULL)
	//	{
	//		delete *imgs[i];
	//		*imgs[i] = NULL;
	//	}
	//	*imgs[i]=pMap;
 //
	//	if (resourceType == _T("PNG"))
	//	{
	//		BOOL bStat = FALSE;
	//		if (orgImg.GetBPP() == 32)
	//		{
	//			bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL, CImage::createAlphaChannel);  
	//		}else
	//		{
	//			bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
	//		}	
	//		ASSERT(bStat); 
	//	}else
	//	{
	//		BOOL bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
	//		ASSERT(bStat);  
	//	}
	//
	//	CImageDC imageDC(*pMap);
	//	orgImg.Draw(imageDC,0,0,width,height,posX,0,width,height);
	//}
}
void CTraButton::Load(const CString& sFileName, int width, int height, const CString& resourceType)
{
	CImage orgImg;
	orgImg.Load(sFileName);
	Load(orgImg,width,height,resourceType);
}

void CTraButton::Load(CImage& orgImg, int width, int height, const CString& resourceType)
{
	if (resourceType == _T("PNG"))
	{
		if (orgImg.GetBPP() == 32)
		{
			//PNG透明
			for(int i = 0; i < orgImg.GetWidth(); i++)  
			{  
				for(int j = 0; j < orgImg.GetHeight(); j++)  
				{  
					unsigned char* pucColor = reinterpret_cast<unsigned char *>(orgImg.GetPixelAddress(i , j));  
					pucColor[0] = pucColor[0] * pucColor[3] / 255;  
					pucColor[1] = pucColor[1] * pucColor[3] / 255;  
					pucColor[2] = pucColor[2] * pucColor[3] / 255;  
				}  
			}
		}
	}

	if (width == -1)
	{
		//const int n = max(1,(orgImg.GetWidth()/orgImg.GetHeight()));
		width = orgImg.GetWidth()/4;
		height = orgImg.GetHeight();
	}
	if (width==0 && height==0)
	{
		width=orgImg.GetHeight();//方形
	}
	if (height==0)
	{
		height = orgImg.GetHeight();
	}

	m_btninfoNor.nWidth = width;
	m_btninfoNor.nHeight = height;
	m_btninfoHot.nWidth = width;
	m_btninfoHot.nHeight = height;
	m_btninfoPre.nWidth = width;
	m_btninfoPre.nHeight = height;
	m_btninfoDis.nWidth = width;
	m_btninfoDis.nHeight = height;
	CImage** imgs[]={&m_btninfoNor.pImg,&m_btninfoHot.pImg,&m_btninfoPre.pImg,&m_btninfoDis.pImg};
	int posX=0;
	for (int i=0;i<4;i++,posX+=width)
	//for (int i=0;i<4 && posX<=(orgImg.GetWidth()-width);i++,posX+=width)
	{
		CImage* pMap=new CImage();
		if (*imgs[i] != NULL)
		{
			delete *imgs[i];
			*imgs[i] = NULL;
		}
		*imgs[i]=pMap;
 
		if (resourceType == _T("PNG"))
		{
			BOOL bStat = FALSE;
			if (orgImg.GetBPP() == 32)
			{
				bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL, CImage::createAlphaChannel);  
			}else
			{
				bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
			}	
			ASSERT(bStat); 
		}else
		{
			BOOL bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
			ASSERT(bStat);  
		}
	
		CImageDC imageDC(*pMap);
		if (orgImg.GetWidth()>=(posX+width))
			orgImg.Draw(imageDC,0,0,width,height,posX,0,width,height);
		else
			orgImg.Draw(imageDC,0,0,width,height,0,0,width,height);
	}
}

//重绘
void CTraButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;
	CString strText = _T("");
	GetWindowText(strText);

	//绘图
	if(lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		DrawBK(*pDC, m_btninfoDis.pImg, TRA_BTN_DIS);
	}
	else if(lpDrawItemStruct->itemState & ODS_SELECTED
		//|| lpDrawItemStruct->itemState & ODS_CHECKED	// 没用
		||(m_bHot && m_bPress) )
	{
		DrawBK(*pDC, m_btninfoPre.pImg, TRA_BTN_PRE);
	}
	else if (m_bSelected)	// add by hd
	{
		DrawBK(*pDC, m_btninfoPre.pImg, TRA_BTN_PRE);
	}
	else if(m_bHot)
	{
		DrawBK(*pDC, m_btninfoHot.pImg, TRA_BTN_HOT);
	}
	else
	{
		DrawBK(*pDC, m_btninfoNor.pImg, TRA_BTN_NOR);
	}
	//写字
	CString strTemp(strText);
	strTemp.Remove(' ');
	if (!strTemp.IsEmpty())
	{
		if(lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			DrawBtnText(*pDC,strText,0,0, TRA_BTN_DIS);
		}
		else if(lpDrawItemStruct->itemState & ODS_SELECTED
			||(m_bHot && m_bPress) )
		{
			if (m_bPress || (!m_bSelected && m_bTextHotMove))
				DrawBtnText(*pDC, strText,1,1, TRA_BTN_PRE);
			else
				DrawBtnText(*pDC, strText,0,0, TRA_BTN_PRE);
		}
		else if(m_bHot)
		{
			if (!m_bSelected && m_bTextHotMove)
				DrawBtnText(*pDC, strText,0,-1, TRA_BTN_HOT);
			else
				DrawBtnText(*pDC, strText,0,0, TRA_BTN_HOT);
		}
		else
		{
			DrawBtnText(*pDC, strText,0,0, TRA_BTN_NOR);
		}

	}
}

//写字
void CTraButton::DrawBtnText(HDC dc, const CString& strText, int nMoveX, int nMoveY, TRA_BTNSTATE btnstate)
{
	CSize sizeText = CDC::FromHandle(dc)->GetTextExtent(strText);
	CRect rect;
	GetClientRect(&rect);

	rect.DeflateRect(m_nTextLeft+nMoveX,m_nTextTop+nMoveY,0,0);
	CDC::FromHandle(dc)->SetBkMode(TRANSPARENT);

	if (btnstate == TRA_BTN_NOR)
	{
		CDC::FromHandle(dc)->SetTextColor(m_crNorText);
	}else if (btnstate == TRA_BTN_HOT)
	{
		CDC::FromHandle(dc)->SetTextColor(m_crHotText);
	}else if (btnstate == TRA_BTN_PRE)
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(30,30,30));
	}else
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(100,100,100));
	}
	CDC::FromHandle(dc)->DrawText(strText, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER); 
}

const Gdiplus::ColorMatrix theColorMatrix =
{0.3, 0.3, 0.3, 0.0, 0.0,
    0.59, 0.59, 0.59, 0.0, 0.0,
    0.11, 0.11, 0.11, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 1.0};

//绘图
void CTraButton::DrawBK(HDC dc, CImage* img, TRA_BTNSTATE btnstate)
{
	//if (img==NULL)
	//	return;

	CRect rc;
	GetClientRect(&rc);
	CRect tmpRect;
	int nX = 0;
	int nY = 0;
	int nW = 0;
	int nH = 0;

	if (m_bAutoSize == true)
	{
		tmpRect.SetRect(0,0,rc.Width(), rc.Height());
	}else
	{
		if(btnstate == TRA_BTN_NOR)
		{
			nW = img!=NULL?m_btninfoNor.nWidth:rc.Width();
			nH = img!=NULL?m_btninfoNor.nHeight:rc.Height();
		}else if (btnstate == TRA_BTN_HOT)
		{
			nW = img!=NULL?m_btninfoHot.nWidth:rc.Width();
			nH = img!=NULL?m_btninfoHot.nHeight:rc.Height();
		}else if (btnstate == TRA_BTN_PRE)
		{
			nW = img!=NULL?m_btninfoPre.nWidth:rc.Width();
			nH = img!=NULL?m_btninfoPre.nHeight:rc.Height();
		}else
		{
			nW = img!=NULL?m_btninfoDis.nWidth:rc.Width();
			nH = img!=NULL?m_btninfoDis.nHeight:rc.Height();
		}

		nX = (rc.Width()-nW)/2;
		nY = (rc.Height()-nH)/2;
		tmpRect.SetRect(nX,nY,nW+nX, nH+nY);
	}

	if (img)
		img->Draw(dc,tmpRect);
	else if (m_bDrawPanel)
	{
		COLORREF crPanel = m_crNorPanel;
		if(btnstate == TRA_BTN_NOR)
		{
			crPanel = m_crNorPanel;
		}else if (btnstate == TRA_BTN_HOT)
		{
			crPanel = m_crHotPanel;
		}else if (btnstate == TRA_BTN_PRE)
		{
			crPanel = m_crPrePanel;
		}else
		{
			crPanel = m_crDisPanel;
		}

		if (crPanel!=-1)
		{
			CBrush pBKBrush;
			pBKBrush.CreateSolidBrush(crPanel);
			if (m_bCreateRgn)
			{
				int nx = 1;
				int ny = 1;
				int dx = 3;
				int dy = 3;
				CRgn m_rgn;
				m_rgn.CreateRoundRectRgn(0, 0, tmpRect.Width()+nx, tmpRect.Height()+ny, dx, dy);//这里如果ny> =dy就木有下面的圆角啦 
				::FillRgn(dc, (HRGN)m_rgn.GetSafeHandle(), (HBRUSH)pBKBrush.GetSafeHandle());
			}else
			{
				::FillRect(dc,&tmpRect,(HBRUSH)pBKBrush.GetSafeHandle());
			}
		}
	}
	if (m_bDrawClosePic)
	{
		COLORREF crPanel = m_crClosePicNor;
		if(btnstate == TRA_BTN_NOR)
		{
			crPanel = m_crClosePicNor;
		}else if (btnstate == TRA_BTN_HOT)
		{
			crPanel = m_crClosePicHot;
		}else if (btnstate == TRA_BTN_PRE)
		{
			crPanel = m_crClosePicPre;
		}else
		{
			crPanel = m_crClosePicDis;
		}

		//const int nSize = 20;
		//CRect rect;
		//rect.right = tmpRect.right;
		//rect.left = rect.right - nSize;
		//rect.top = (tmpRect.Height()-nSize)/2;
		//rect.bottom = rect.top + nSize;

		HPEN hPenNew = CreatePen(PS_SOLID,m_nClosePicWidth,crPanel);
		HPEN hPenOld = (HPEN)SelectObject(dc,hPenNew);
		::MoveToEx(dc,rc.left+2,rc.top+2,NULL);
		::LineTo(dc,rc.right-m_nClosePicWidth-1,rc.bottom-m_nClosePicWidth-1);
		::MoveToEx(dc,rc.right-m_nClosePicWidth-1,rc.top+2,NULL);
		::LineTo(dc,rc.left+2,rc.bottom-m_nClosePicWidth-1);
		SelectObject(dc,hPenOld);
		DeleteObject(hPenNew);

		//CBrush pBKBrush1;
		//pBKBrush1.CreateSolidBrush(RGB(27,27,27));
		//::SelectObject(dc,pBKBrush1.GetSafeHandle());
		//::Ellipse(dc,rect.left,rect.top,rect.right,rect.bottom);
	}
	if (m_bDrawNewPic)
	{
		// 
		COLORREF crPanel = m_crNewPicNor;
		if(btnstate == TRA_BTN_NOR)
		{
			crPanel = m_crNewPicNor;
		}else if (btnstate == TRA_BTN_HOT)
		{
			crPanel = m_crNewPicHot;
		}else if (btnstate == TRA_BTN_PRE)
		{
			crPanel = m_crNewPicPre;
		}else
		{
			crPanel = m_crNewPicDis;
		}

		HPEN hPenNew = CreatePen(PS_SOLID,m_nNewPicWidth,crPanel);
		HPEN hPenOld = (HPEN)SelectObject(dc,hPenNew);
		::MoveToEx(dc,(rc.Width()-m_nNewPicWidth)/2,(rc.Height()-m_nNewPicHeight)/2,NULL);			// 上
		::LineTo(dc,(rc.Width()-m_nNewPicWidth)/2,(rc.Height()-m_nNewPicHeight)/2+m_nNewPicHeight);	// 下
		::MoveToEx(dc,(rc.Width()-m_nNewPicHeight)/2-1,(rc.Height()-m_nNewPicWidth)/2+1,NULL);			// 左
		::LineTo(dc,(rc.Width()-m_nNewPicHeight)/2-1+m_nNewPicHeight,(rc.Height()-m_nNewPicWidth)/2+1);	// 右
		::LineTo(dc,(rc.Width()-m_nNewPicHeight)/2-1,(rc.Height()-m_nNewPicWidth)/2+1);	// *左
		SelectObject(dc,hPenOld);
		DeleteObject(hPenNew);
	}

	if (m_nDrawTriangleType>0)
	{
		COLORREF crPanel = m_crTrianglePicNor;
		if(btnstate == TRA_BTN_NOR)
		{
			crPanel = m_crTrianglePicNor;
		}else if (btnstate == TRA_BTN_HOT)
		{
			crPanel = m_crTrianglePicHot;
		}else if (btnstate == TRA_BTN_PRE)
		{
			crPanel = m_crTrianglePicPre;
		}else
		{
			crPanel = m_crTrianglePicDis;
		}
		CBrush pBKBrush;
		pBKBrush.CreateSolidBrush(crPanel);

		HBRUSH hOldObj = (HBRUSH)SelectObject(dc,pBKBrush.GetSafeHandle()); 
		BeginPath(dc);
		switch(m_nDrawTriangleType)
		{
		case 1:	// up
			{
				::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight,NULL);				// 左下
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);	// 右下
				::LineTo(dc,rc.Width()/2,(rc.Height()-m_nTrianglePicHeight)/2);																	// 中上
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);						// 左下
			}break;
		case 2:	// down
			{
				::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2,NULL);				// 左上
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2);	// 右上
				::LineTo(dc,rc.Width()/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);						// 中下
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2);						// 左上
			}break;
		case 3:	// left
			{
				::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,rc.Height()/2,NULL);															// 左中
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2);						// 右上
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);	// 右下
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,rc.Height()/2);																	// 左中
			}break;
		case 4:	// right
			{
				::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2,NULL);				// 左上
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);	// 左下
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,rc.Height()/2);							// 右中
				::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2);						// 左上
				//::LineTo(dc,(rc.Width()-m_nTrianglePicHeight)/2,(rc.Height()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth);	// *左下
			}break;
		default:
			break;
		}
		::EndPath(dc);
		::FillPath(dc);
		SelectObject(dc,hOldObj); 
	}
	if (m_nDrawArrowType>0)
	{
		COLORREF crPanel = m_crArrowPicNor;
		if(btnstate == TRA_BTN_NOR)
		{
			crPanel = m_crArrowPicNor;
		}else if (btnstate == TRA_BTN_HOT)
		{
			crPanel = m_crArrowPicHot;
		}else if (btnstate == TRA_BTN_PRE)
		{
			crPanel = m_crArrowPicPre;
		}else
		{
			crPanel = m_crArrowPicDis;
		}

		HPEN hPenNew = CreatePen(PS_SOLID,m_nArrowPenWidth,crPanel);
		HPEN hPenOld = (HPEN)SelectObject(dc,hPenNew);
		switch(m_nDrawArrowType)
		{
		//case 1:	// up
		//	{
		//		::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight,NULL);				// 左下
		//		::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);	// 右下
		//		::LineTo(dc,rc.Width()/2,(rc.Height()-m_nTrianglePicHeight)/2);																	// 中上
		//		::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);						// 左下
		//	}break;
		//case 2:	// down
		//	{
		//		::MoveToEx(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2,NULL);				// 左上
		//		::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2+m_nTrianglePicWidth,(rc.Height()-m_nTrianglePicHeight)/2);	// 右上
		//		::LineTo(dc,rc.Width()/2,(rc.Height()-m_nTrianglePicHeight)/2+m_nTrianglePicHeight);						// 中下
		//		::LineTo(dc,(rc.Width()-m_nTrianglePicWidth)/2,(rc.Height()-m_nTrianglePicHeight)/2);						// 左上
		//	}break;
		case 3:	// left
			{
				if (m_bDrawArrowLine)
				{
					::MoveToEx(dc,(rc.Width()-m_nArrowPicWidth)/2-2-m_nArrowPenWidth,rc.Height()/2-m_nArrowPicWidth,NULL);				// 左上
					::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2-2-m_nArrowPenWidth,rc.Height()/2+m_nArrowPicWidth);						// 左下
					::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2-2-m_nArrowPenWidth,rc.Height()/2-m_nArrowPicWidth);						// *左上
				}
				::MoveToEx(dc,(rc.Width()-m_nArrowPicWidth)/2+m_nArrowPicWidth,rc.Height()/2-m_nArrowPicWidth,NULL);					// 右上
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2,rc.Height()/2);																// 左中
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2+m_nArrowPicWidth,rc.Height()/2+m_nArrowPicWidth);							// 右下
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2,rc.Height()/2);																// *左中
			}break;
		case 4:	// right
			{
				if (m_bDrawArrowLine)
				{
					::MoveToEx(dc,(rc.Width()-m_nArrowPicWidth)/2+2+m_nArrowPenWidth,rc.Height()/2-m_nArrowPicWidth,NULL);				// 右上
					::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2+2+m_nArrowPenWidth,rc.Height()/2+m_nArrowPicWidth);						// 右下
					::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2+2+m_nArrowPenWidth,rc.Height()/2-m_nArrowPicWidth);						// *右上
				}
				::MoveToEx(dc,(rc.Width()-m_nArrowPicWidth)/2-m_nArrowPicWidth,rc.Height()/2-m_nArrowPicWidth,NULL);						// 左上
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2,rc.Height()/2);																	// 右中
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2-m_nArrowPicWidth,rc.Height()/2+m_nArrowPicWidth);								// 左下
				::LineTo(dc,(rc.Width()-m_nArrowPicWidth)/2,rc.Height()/2);																	// *右中
			}break;
		default:
			break;
		}
		SelectObject(dc,hPenOld);
		DeleteObject(hPenNew);
	}

	if (m_pImage!=NULL)
	{
		Gdiplus::Graphics graphics(dc);
		if (m_bGrayImage)
		{
			Gdiplus::ImageAttributes pImageAttributes;
			pImageAttributes.SetColorMatrix(&theColorMatrix,ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
			graphics.DrawImage(m_pImage,m_rcImage,0,0, m_pImage->GetWidth(), m_pImage->GetHeight(),Gdiplus::UnitPixel,&pImageAttributes);
		}else
		{
			graphics.DrawImage(m_pImage, m_rcImage);
		}
	}
}


//鼠标移动
void CTraButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	//if (m_bHot)
	//{
	//	CRect rect;
	//	GetClientRect(rect);
	//	if(!rect.PtInRect(point))	
	//	{
	//		m_bHot = false;
	//		ReleaseCapture();
	//		this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
	//		return;
	//	}
	//}else
	//{
	//	m_bHot = true;
	//	SetCapture();
	//	this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
	//}

	CRect rect;
	GetClientRect(rect);
	if(rect.PtInRect(point))	
	{
		if(!(nFlags & MK_LBUTTON))
			m_bPress = false;
		if(GetCapture() != this)
		{			
			SetCapture();
		}
		if (m_bHot)
		{
			//
		}else
		{
			m_bHot = true;
			if (m_bAutoFocus)
				SetFocus();
			this->PostMessage(BUTTON_MSG_RE_INVALIDATE,0,0);
			//Invalidate();
			//CRect rcTemp;
			//this->GetWindowRect(&rcTemp);
			//GetParent()->ScreenToClient(&rcTemp);
			//GetParent()->InvalidateRect(rcTemp, TRUE);
		}
	}
	else
	{
		ReleaseCapture();
		m_bHot = false;
		this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
	}
	CButton::OnMouseMove(nFlags, point);
}

//鼠标抬起
void CTraButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	// add by hd
	RECT rc;
	GetClientRect(&rc);
	BOOL bIn = ::PtInRect(&rc, point);
	if (bIn)
	{
		UINT uStyle = (UINT)::GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
		if ((uStyle == BS_RADIOBUTTON) || (uStyle == BS_CHECKBOX) || (uStyle == BS_AUTOCHECKBOX) || (uStyle == BS_AUTORADIOBUTTON))
			m_bSelected = !m_bSelected;
		else
			m_bSelected = false;
	}

	if(m_bPress)
	{
		m_bPress = false;	
		this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
	}
	if (m_bHot)
	{
		m_bHot = false;
		//this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
	}
	CButton::OnLButtonUp(nFlags, point);
}

//鼠标落下
void CTraButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	CRect rcTemp;
	this->GetWindowRect(&rcTemp);
	GetParent()->ScreenToClient(&rcTemp);
	GetParent()->InvalidateRect(rcTemp, TRUE);
	m_bPress = true;	
	CButton::OnLButtonDown(nFlags, point);
}

//背景
BOOL CTraButton::OnEraseBkgnd(CDC* pDC) 
{
	if(GetButtonStyle() & BS_OWNERDRAW)
		return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}

//自适应背景图
void CTraButton::SetAutoSize(bool bAutoSize)
{
	m_bAutoSize = bAutoSize;
}

void CTraButton::MovePoint(int x, int y, int nWidth, int nHeight)
{
	if (this->GetSafeHwnd() != NULL)// && m_btninfoNor.pImg != NULL)
	{
		if (nWidth == 0 && m_btninfoNor.pImg != NULL)
			nWidth = m_btninfoNor.nWidth;
		if (nHeight == 0 && m_btninfoNor.pImg != NULL)
			nHeight = m_btninfoNor.nHeight;
		this->MoveWindow(x,y,nWidth,nHeight);
	}
}

//按钮风格
void CTraButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);//设置自绘
	CButton::PreSubclassWindow();
}

void CTraButton::SetToolTipText(LPCTSTR s)
{
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this))	//first assignment
			if (m_tooltip.AddTool(this, s))
				m_tooltip.Activate(TRUE);
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}
}
void CTraButton::SetToolTipColor(COLORREF crText, COLORREF crBkgnd)
{
    if(m_tooltip.m_hWnd != NULL)
    {
		m_tooltip.SetTipTextColor(crText);
        m_tooltip.SetTipBkColor(crBkgnd);
    }
}
void CTraButton::SetDrawPanel(bool bDrawPanel, COLORREF crNor, COLORREF crHot, COLORREF crPre, COLORREF crDis)
{
	m_bDrawPanel = bDrawPanel;
	m_crNorPanel = crNor;
	m_crHotPanel = crHot==-1?m_crNorPanel:crHot;
	m_crPrePanel = crPre==-1?m_crNorPanel:crPre;
	m_crDisPanel = crDis==-1?m_crNorPanel:crDis;
}
void CTraButton::SetDrawClosePic(bool bDrawClose, COLORREF crNor, COLORREF crHot, COLORREF crPre, COLORREF crDis, int nWidth)
{
	m_bDrawClosePic = bDrawClose;
	m_crClosePicNor = crNor;
	m_crClosePicHot = crHot==-1?m_crClosePicNor:crHot;
	m_crClosePicPre = crPre==-1?m_crClosePicNor:crPre;
	m_crClosePicDis = crDis==-1?m_crClosePicNor:crDis;
	m_nClosePicWidth = nWidth;
}
void CTraButton::SetDrawNewPic(bool bDrawNew, COLORREF crNor, COLORREF crHot, COLORREF crPre, COLORREF crDis, int nWidth, int nHeight)
{
	m_bDrawNewPic = bDrawNew;
	m_crNewPicNor = crNor;
	m_crNewPicHot = crHot==-1?m_crNewPicNor:crHot;
	m_crNewPicPre = crPre==-1?m_crNewPicNor:crPre;
	m_crNewPicDis = crDis==-1?m_crNewPicNor:crDis;
	m_nNewPicWidth = nWidth;
	m_nNewPicHeight = nHeight;
}

void CTraButton::SetDrawTrianglePic(int nTriangleType, COLORREF crNor, COLORREF crHot, COLORREF crPre, COLORREF crDis, int nWidth, int nHeight)
{
	m_nDrawTriangleType = nTriangleType;
	m_crTrianglePicNor = crNor;
	m_crTrianglePicHot = crHot==-1?m_crTrianglePicNor:crHot;
	m_crTrianglePicPre = crPre==-1?m_crTrianglePicNor:crPre;
	m_crTrianglePicDis = crDis==-1?m_crTrianglePicNor:crDis;
	m_nTrianglePicWidth = nWidth;
	m_nTrianglePicHeight = nHeight;
}
void CTraButton::SetDrawArrowPic(int nArrowType, bool bDrawLine, int nPenWidth, COLORREF crNor, COLORREF crHot, COLORREF crPre, COLORREF crDis, int nWidth)
{
	m_nDrawArrowType = nArrowType;
	m_bDrawArrowLine = bDrawLine;
	m_nArrowPenWidth = nPenWidth;
	m_crArrowPicNor = crNor;
	m_crArrowPicHot = crHot==-1?m_crArrowPicNor:crHot;
	m_crArrowPicPre = crPre==-1?m_crArrowPicNor:crPre;
	m_crArrowPicDis = crDis==-1?m_crArrowPicNor:crDis;
	m_nArrowPicWidth = nWidth;
}

void CTraButton::SetDrawImage(bool bDestroyDelete, bool bGrayImage, Gdiplus::Image * pImage, const Gdiplus::Rect& rect)
{
	m_bGrayImage = bGrayImage;
	m_rcImage = rect;
	if (m_pImage==pImage)
	{
		return;
	}
	Gdiplus::Image* pImageOld = m_pImage;
	m_pImage = pImage;
	this->Invalidate();
	if (m_bDestroyDelete && pImageOld!=NULL)
	{
		delete pImageOld;
	}
	m_bDestroyDelete = bDestroyDelete;
}
void CTraButton::SetDrawImage(bool bGrayImage)
{
	m_bGrayImage = bGrayImage;
	if (m_pImage!=NULL)
	{
		this->Invalidate();
	}
}

void CTraButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
// This function will create a MSG structure, fill it in a pass it to
// the ToolTip control, m_ttip.  Note that we ensure the point is in window
// coordinates (relative to the control's window).
	if(NULL != m_tooltip.m_hWnd){
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}

void CTraButton::SetChecked(bool b)
{
	m_bSelected = b;
	//UpdateState();
}

bool CTraButton::GetChecked() const
{ 
	return m_bSelected;
}

LRESULT CTraButton::OnMsgReInvalidate(WPARAM wParam, LPARAM lParam)
{
	if (wParam==1 && this->GetFocus()->GetSafeHwnd()==this->GetSafeHwnd())
	{
		GetParent()->SetFocus();	// 实现按钮失去焦点，正常刷新背景
	}
	Invalidate();
	CRect rcTemp;
	this->GetWindowRect(&rcTemp);
	GetParent()->ScreenToClient(&rcTemp);
	GetParent()->InvalidateRect(rcTemp, TRUE);
	return 0;
}

void CTraButton::OnTimer(UINT_PTR nIDEvent)
{
	CButton::OnTimer(nIDEvent);
}
void CTraButton::OnDestroy()
{
	if (m_tooltip.GetSafeHwnd()!=NULL)
	{
		m_tooltip.Activate(FALSE);
		m_tooltip.DelTool(this);
		m_tooltip.DestroyWindow();
	}
	CButton::OnDestroy();
	if (m_pImage!=NULL)
	{
		if (m_bDestroyDelete)
			delete m_pImage;
		m_pImage = NULL;
	}
}
