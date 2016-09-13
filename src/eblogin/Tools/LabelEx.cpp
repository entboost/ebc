// LabelEx.cpp : implementation file
//

#include "stdafx.h"
#include "LabelEx.h"
#include "SkinMemDC.h"

#define BUTTON_MSG_RE_INVALIDATE (WM_USER+110)
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelEx
#define SAFE_DELETEOBJECT(obj) {if (obj) DeleteObject(obj);}

CLabelEx::CLabelEx()
{
	m_bClicked=FALSE;
	m_bOver=FALSE;
	m_bUnderLine=FALSE;
	m_bAutoUnderLine=FALSE;
	m_bBorder=FALSE;
	m_bHighLight=TRUE;
	m_bBackUnderLine=FALSE;
	m_alignText = Align_Center;
	m_nTextLeft = 0;
	m_nTextTop = 0;
	m_hHoverBorder=FALSE;
	m_crHoverBorder=RGB(0,128,255);
	m_hCheckBoxHoverBorder = FALSE;
	m_crCheckBoxHoverBorder=RGB(0,128,255);

	m_crText=RGB(0,0,0);
	m_crHotText=m_crText;
	m_crBorder=RGB(0,0,0);
	m_crBG=RGB(255,255,255);
	m_crUnderLine=RGB(0,0,255);

	m_hHandCur = NULL;
	//m_hHandCur=AfxGetApp()->LoadCursor(IDC_HAND);
}


CLabelEx::~CLabelEx()
{
	//SAFE_DELETEOBJECT(m_BGBm.Detach());
	//SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
	//SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
	//SAFE_DELETEOBJECT(m_LabelBm.Detach());
	//SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
	//SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
}


BEGIN_MESSAGE_MAP(CLabelEx, CStatic)
	//{{AFX_MSG_MAP(CLabelEx)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(BUTTON_MSG_RE_INVALIDATE,OnMsgReInvalidate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelEx message handlers

void CLabelEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (m_bOver)
		dc.SetTextColor(m_crHotText);
	else
		dc.SetTextColor(m_crText);
	//dc.SetTextColor(m_crText);
	dc.SetBkMode(TRANSPARENT);
	CFont * poldFont = dc.SelectObject(this->GetFont());
	///准备工作
	CRect rect;
	CDC pMemDC;
	CPen BorderPen,*pOldPen,UnderLinePen;
	CBrush BGBrush,*pOldBrush;
	//BITMAP bm;
	int nTextLeft=0,nTextTop=0; //文字输出的位置
	//HGDIOBJ poldBmp;

	this->GetClientRect(&rect);

	pMemDC.CreateCompatibleDC(&dc);
	if (pMemDC.m_hDC == NULL)
	{
		dc.SelectObject(poldFont);
		return;
	}
	pMemDC.SetMapMode(dc.GetMapMode());
	//CSkinMemDC memDC(&dc, rect);
	//memDC.SetBkMode(TRANSPARENT);
	//memDC.FillSolidRect(rect,  RGB(74,82,107));
	//Gdiplus::Graphics graphics(dc.m_hDC);

	///画边框
	if(m_bBorder)
	{
		if (!BorderPen.CreatePen(PS_SOLID,1,m_crBorder)) return;
		if (!BGBrush.CreateSolidBrush(m_crBG)) return;
		pOldPen=dc.SelectObject(&BorderPen);
		pOldBrush=dc.SelectObject(&BGBrush);

		dc.Rectangle(&rect);

		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		rect.DeflateRect(1,1);
		BorderPen.DeleteObject();
		BGBrush.DeleteObject();
	}
	// Hover 边框
	if (m_bOver && m_hHoverBorder)
	{
		//Gdiplus::Pen penTools(Gdiplus::Color(0,128,255),1.0);
		//graphics.DrawRectangle(&penTools,Rect(0,0,rect.Width(),rect.Height()));

		BorderPen.CreatePen(PS_SOLID,1,m_crHoverBorder);
		//BGBrush.CreateSolidBrush(m_crBG);
		//BGBrush.CreateSysColorBrush(COLOR_BTNFACE);
		pOldPen=dc.SelectObject(&BorderPen);
		//pOldBrush=dc.SelectObject(&BGBrush);

		dc.Rectangle(&rect);

		dc.SelectObject(pOldPen);
		//dc.SelectObject(pOldBrush);

		rect.DeflateRect(1,1);
		BorderPen.DeleteObject();
		//BGBrush.DeleteObject();
	}else if (m_hHoverBorder)
	{
		// 清空边框
	}

	///贴背景图
	if(m_bClicked && m_pDrawBGClicked.m_pImage!=NULL)
	{
		m_pDrawBGClicked.m_pImage->Draw(dc,rect);
	}
	else if(m_bOver && m_pDrawBGHover.m_pImage!=NULL)//鼠标经过的时候
	{
		m_pDrawBGHover.m_pImage->Draw(dc,rect);
	}
	else if(m_pDrawBGNormal.m_pImage!=NULL)
	{
		m_pDrawBGNormal.m_pImage->Draw(dc,rect);
	}

	///贴标签图片
	CRect rectLabel(rect);
	rectLabel.top += (rect.Height()-m_pDrawLabelClicked.m_nHeight)/2;
	if(m_bClicked && m_pDrawLabelClicked.m_pImage!=NULL)
	{
		//rectLabel.right = rectLabel.left+m_pDrawLabelClicked.m_nWidth;
		//rectLabel.bottom = rectLabel.top+m_pDrawLabelClicked.m_nHeight;
		m_pDrawLabelClicked.m_pImage->Draw(dc,rectLabel.left,rectLabel.top);
	}
	else if(m_bOver && m_pDrawLabelHover.m_pImage!=NULL)
	{
		//rectLabel.right = rectLabel.left+m_pDrawLabelHover.m_nWidth;
		//rectLabel.bottom = rectLabel.top+m_pDrawLabelHover.m_nHeight;
		m_pDrawLabelHover.m_pImage->Draw(dc,rectLabel.left,rectLabel.top);
	}
	else if(m_pDrawLabelNormal.m_pImage!=NULL)
	{
		//rectLabel.right = rectLabel.left+m_pDrawLabelNormal.m_nWidth;
		//rectLabel.bottom = rectLabel.top+m_pDrawLabelNormal.m_nHeight;
		m_pDrawLabelNormal.m_pImage->Draw(dc,rectLabel.left,rectLabel.top);
	}
	else
	{
		nTextLeft=4;
	}
	if (m_bOver && m_hCheckBoxHoverBorder && m_pDrawLabelNormal.m_nHeight>0)
	{
		CRect rectHover;
		rectHover.left = 0;
		rectHover.top = (rect.Height()-m_pDrawLabelNormal.m_nHeight)/2;
		rectHover.right = rectHover.left + m_pDrawLabelNormal.m_nWidth-1;
		rectHover.bottom = rectHover.top + m_pDrawLabelNormal.m_nHeight-1;
		BorderPen.CreatePen(PS_SOLID,1,m_crCheckBoxHoverBorder);
		pOldPen=dc.SelectObject(&BorderPen);
		dc.MoveTo(rectHover.left,rectHover.top);
		dc.LineTo(rectHover.right,rectHover.top);
		dc.LineTo(rectHover.right,rectHover.bottom);
		dc.LineTo(rectHover.left,rectHover.bottom);
		dc.LineTo(rectHover.left,rectHover.top);
		dc.SelectObject(pOldPen);
		//rect.DeflateRect(1,1);
		BorderPen.DeleteObject();
		//BGBrush.DeleteObject();
	}

	///输出文字
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CString strText;
	this->GetWindowText(strText);
	nTextTop=rect.top+(rect.Height()-tm.tmHeight)/2;
	if (!strText.IsEmpty())
	{	
		// ??文字有点问题；
		nTextLeft += m_nTextLeft;
		if (m_alignText == Align_Default)
		{
			UINT nFormat = DT_LEFT;
			dc.DrawText(strText, &rect, nFormat);
			//dc.TextOut(nTextLeft,nTextTop+m_nTextTop,strText);
		}else
		{
			rect.left += nTextLeft;
			rect.top += m_nTextTop;
			UINT nFormat = 0;//DT_SINGLELINE|DT_VCENTER;
			if (m_alignText == Align_Left)
			{
				rect.left += 2;
				nFormat |= DT_LEFT;
			}else if (m_alignText == Align_Center)
				nFormat |= DT_CENTER;
			else if (m_alignText == Align_Right)
				nFormat |= DT_RIGHT;

			//nFormat |= DT_CENTER;
			dc.DrawText(strText, &rect, nFormat);
		}
	}

	///画下划线
	if(m_bUnderLine)
	{
		nTextLeft-=2;
		nTextTop=nTextTop+tm.tmHeight+1;
		if (!UnderLinePen.CreatePen(PS_SOLID,1,m_crUnderLine)) return;
		pOldPen=dc.SelectObject(&UnderLinePen);
		dc.MoveTo(nTextLeft,nTextTop);
		dc.LineTo(nTextLeft+tm.tmAveCharWidth*strText.GetLength(),nTextTop);
		dc.SelectObject( pOldPen );
		UnderLinePen.DeleteObject();
	}
	dc.SelectObject( poldFont );

	CStatic::OnPaint();
}

void CLabelEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	
	SetFont(GetParent()->GetFont());
	CStatic::PreSubclassWindow();
}

void CLabelEx::SetToolTipText(LPCTSTR s)
{
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this))	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(TRUE);
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}
}

void CLabelEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	if (m_bOver)        // Cursor is currently over control
    {
        CRect rect;
        GetClientRect(rect);

        if (!rect.PtInRect(point))
        {
            m_bOver = FALSE;
			//if(m_bAutoUnderLine)  ///自动下划线
			//{
			//	this->SetUnderLine(FALSE,RGB(0,0,0));
			//}
			//if(m_bHighLight)   //自动高亮
			//{
			//	///恢复原来的字体颜色
			//	this->SetTextColor(m_crBackText);
			//}
			//RedrawWindow();
            ReleaseCapture();
			this->PostMessage(BUTTON_MSG_RE_INVALIDATE,1,0);
            return;
        }
    }
    else                      // Cursor has just moved over control
    {
        m_bOver = TRUE;
		//if(m_bAutoUnderLine)
		//{
		//	this->SetUnderLine(TRUE,RGB(0,0,255));
		//}
		//if(m_bHighLight)
		//{
		//	m_crBackText=m_crText;
		//	this->SetTextColor(RGB(0,0,255));
		//}
		if(GetCapture() != this)
		{			
			SetCapture();
		}
        //SetCapture();
        //RedrawWindow();
		if (m_hHandCur != NULL)
			::SetCursor(m_hHandCur);
		this->PostMessage(BUTTON_MSG_RE_INVALIDATE,0,0);
    }

	CStatic::OnMouseMove(nFlags, point);
}

void CLabelEx::SetTextColor(COLORREF crText,COLORREF crHot)
{
	m_crText=crText;
	m_crHotText = crHot==-1?m_crText:crHot;
}

void CLabelEx::MoveWindowPoint(int x, int y, BOOL bRepaint)
{
	if (this->GetSafeHwnd())
	{
		if (m_pDrawBGNormal.m_nWidth>0 && m_pDrawBGNormal.m_nHeight>0)
		{
			this->MoveWindow(x, y, m_pDrawBGNormal.m_nWidth, m_pDrawBGNormal.m_nHeight);
		}else if (m_pDrawLabelNormal.m_nWidth>0 && m_pDrawLabelNormal.m_nHeight>0)
		{
			this->MoveWindow(x, y, m_pDrawLabelNormal.m_nWidth, m_pDrawLabelNormal.m_nHeight);
		}
	}
}

int CLabelEx::GetWidth(void) const
{
	if (this->GetSafeHwnd())
	{
		CRect rectClient;
		this->GetClientRect(&rectClient);
		return rectClient.Width();
	}
	return -1;
}

int CLabelEx::GetHeight(void) const
{
	if (this->GetSafeHwnd())
	{
		CRect rectClient;
		this->GetClientRect(&rectClient);
		return rectClient.Height();
	}
	return -1;
}

void CLabelEx::SetUnderLine(BOOL bUnderLine, COLORREF crUnderLine)
{
	m_bUnderLine=bUnderLine;
	m_crUnderLine=crUnderLine;

}

void CLabelEx::SetBorder(BOOL bBorder, COLORREF crBorder)
{
	m_bBorder=bBorder;
	m_crBorder=crBorder;
}
void CLabelEx::SetHoverBorder(BOOL bHoverBorder,COLORREF crBorder)
{
	m_hHoverBorder=bHoverBorder;
	m_crHoverBorder=crBorder;
}
void CLabelEx::SetCheckBoxHoverBorder(BOOL bHoverBorder,COLORREF crBorder)
{
	m_hCheckBoxHoverBorder=bHoverBorder;
	m_crCheckBoxHoverBorder=crBorder;
}

void CLabelEx::SetBkColor(COLORREF crBkColor)
{
	m_crBG=crBkColor;

}

void CLabelEx::EnableAutoUnderLine(BOOL bAutoUnderLine)
{
	m_bAutoUnderLine=bAutoUnderLine;
	if(m_bAutoUnderLine)   ///设置了自动下划线
	{
		m_bBackUnderLine=m_bUnderLine;//保存原来下划线的状态
		m_bUnderLine=FALSE;
	}
	else   ///取消了自动下划线
	{
		m_bUnderLine=m_bBackUnderLine;//回复原来下划线的状态
	}
}

void CLabelEx::RedrawWindow()
{
	CStatic::RedrawWindow();
	//RECT rect;
	//this->GetWindowRect(&rect);
	//GetParent()->ScreenToClient(&rect);
	//GetParent()->InvalidateRect(&rect);
}
//
//void CLabelEx::SetBGBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_BGBm.Detach());
//	m_BGBm.Attach(ID);
//}
//
//void CLabelEx::SetBGBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_BGBm.Detach());
//	m_BGBm.LoadBitmap(ID);
//}
//
//void CLabelEx::SetMouseOverBGBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
//	m_MouseOverBGBm.Attach(ID);
//}
//
//void CLabelEx::SetMouseOverBGBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
//	m_MouseOverBGBm.LoadBitmap(ID);
//}
//
//void CLabelEx::SetClickedBGBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
//	m_ClickedBGBm.Attach(ID);
//}
//
//void CLabelEx::SetClickedBGBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
//	m_ClickedBGBm.LoadBitmap(ID);
//}
//
//void CLabelEx::SetLabelBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_LabelBm.Detach());
//	m_LabelBm.Attach(ID);
//}
//
//void CLabelEx::SetLabelBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_LabelBm.Detach());
//	m_LabelBm.LoadBitmap(ID);
//}

void CLabelEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bClicked=!m_bClicked;
	//RedrawWindow();
	this->PostMessage(BUTTON_MSG_RE_INVALIDATE,0,0);
	CStatic::OnLButtonDown(nFlags, point);
}

BOOL CLabelEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hHandCur != NULL)
	{
		::SetCursor(m_hHandCur);
		return TRUE;
	}
	else
		return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CLabelEx::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
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
//
//void CLabelEx::SetMouseOverLabelBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
//	m_MouseOverLabelBm.Attach(ID);
//
//}
//
//void CLabelEx::SetMouseOverLabelBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
//	m_MouseOverLabelBm.LoadBitmap(ID);
//}
//
//void CLabelEx::SetClickedLabelBitmap(HBITMAP ID)
//{
//	SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
//	m_ClickedLabelBm.Attach(ID);
//
//}
//
//void CLabelEx::SetClickedLabelBitmap(UINT ID)
//{
//	SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
//	m_ClickedLabelBm.LoadBitmap(ID);
//
//}

bool CLabelEx::LoadBG(UINT IDBkGroup, int width, int height, int nSize,const CString& resourceType)
{
	CImage orgImg;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(IDBkGroup),resourceType);
	if (hRsrc == NULL)
		return false;

	//讲资源加载到内存中
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return false;

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
		width = orgImg.GetWidth()/nSize;
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

	m_pDrawBGNormal.m_nWidth = width;
	m_pDrawBGNormal.m_nHeight = height;
	m_pDrawBGClicked.m_nWidth = width;
	m_pDrawBGClicked.m_nHeight = height;
	m_pDrawBGHover.m_nWidth = width;
	m_pDrawBGHover.m_nHeight = height;
	CImage** imgs[]={&m_pDrawBGNormal.m_pImage,&m_pDrawBGClicked.m_pImage,&m_pDrawBGHover.m_pImage};
	int posX=0;
	for (int i=0;i<nSize && posX<=(orgImg.GetWidth()-width);i++,posX+=width)
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
		orgImg.Draw(imageDC,0,0,width,height,posX,0,width,height);
	}
	return true;
}
bool CLabelEx::LoadLabel(UINT IDLabelGroup, int width, int height, int nSize, const CString& resourceType)
{
	CImage orgImg;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(IDLabelGroup),resourceType);
	if (hRsrc == NULL)
		return false;

	//讲资源加载到内存中
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return false;

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
		width = orgImg.GetWidth()/nSize;
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

	m_pDrawLabelNormal.m_nWidth = width;
	m_pDrawLabelNormal.m_nHeight = height;
	m_pDrawLabelClicked.m_nWidth = width;
	m_pDrawLabelClicked.m_nHeight = height;
	//CString stext;
	//stext.Format(_T("%d %d"),m_pDrawLabelClicked.m_nWidth,m_pDrawLabelClicked.m_nHeight);
	//AfxMessageBox(stext);
	m_pDrawLabelHover.m_nWidth = width;
	m_pDrawLabelHover.m_nHeight = height;
	CImage** imgs[]={&m_pDrawLabelNormal.m_pImage,&m_pDrawLabelClicked.m_pImage,&m_pDrawLabelHover.m_pImage};
	int posX=0;
	for (int i=0;i<nSize && posX<=(orgImg.GetWidth()-width);i++,posX+=width)
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
		orgImg.Draw(imageDC,0,0,width,height,posX,0,width,height);
	}
	return true;
}

///自动感应字体颜色
void CLabelEx::EnableHighLight(BOOL bHighLight)
{
	m_bHighLight=bHighLight;
}
BOOL CLabelEx::OnEraseBkgnd(CDC* pDC) 
{
	//return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}
LRESULT CLabelEx::OnMsgReInvalidate(WPARAM wParam, LPARAM lParam)
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
