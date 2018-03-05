// LabelEx.cpp : implementation file
//

#include "stdafx.h"
#include "LabelEx.h"

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
	m_nImgWidth = 0;
	m_nImgHeight = 0;
	m_bClicked=FALSE;
	m_bOver=FALSE;
	m_bUnderLine=FALSE;
	m_bAutoUnderLine=FALSE;
	m_bBorder=FALSE;
	m_bHighLight=TRUE;
	m_bBackUnderLine=FALSE;
	m_alignText = Align_Center;

	m_crText=RGB(0,0,0);
	m_crBorder=RGB(0,0,0);
	m_crBG=RGB(255,255,255);
	m_crUnderLine=RGB(0,0,255);

	m_hHandCur = NULL;
	//m_hHandCur=AfxGetApp()->LoadCursor(IDC_HAND);
}


CLabelEx::~CLabelEx()
{
	SAFE_DELETEOBJECT(m_BGBm.Detach());
	SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
	SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
	SAFE_DELETEOBJECT(m_LabelBm.Detach());
	SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
	SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
}


BEGIN_MESSAGE_MAP(CLabelEx, CStatic)
	//{{AFX_MSG_MAP(CLabelEx)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelEx message handlers

void CLabelEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.SetTextColor(m_crText);
	dc.SetBkMode(TRANSPARENT);
	CFont * poldFont = dc.SelectObject(this->GetFont());
	///准备工作
	CRect rect;
	CDC MemDC;
	CPen BorderPen,*pOldPen,UnderLinePen;
	CBrush BGBrush,*pOldBrush;
	BITMAP bm;
	int nTextLeft=0,nTextTop=0; //文字输出的位置
	HGDIOBJ poldBmp;

	this->GetClientRect(&rect);

	MemDC.CreateCompatibleDC(&dc);
	if (MemDC.m_hDC == NULL)
	{
		dc.SelectObject(poldFont);
		return;
	}
	MemDC.SetMapMode(dc.GetMapMode());

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
	///贴背景图
	if(m_bClicked && m_ClickedBGBm.GetSafeHandle()!=NULL)
	{
		poldBmp = MemDC.SelectObject(m_ClickedBGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	else if(m_bOver && m_MouseOverBGBm.GetSafeHandle()!=NULL)//鼠标经过的时候
	{
		poldBmp = MemDC.SelectObject(m_MouseOverBGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	else if(m_BGBm.GetSafeHandle()!=NULL)
	{
		poldBmp = MemDC.SelectObject(m_BGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	///贴标签图片
	if(m_bClicked && m_ClickedLabelBm.GetSafeHandle()!=NULL)
	{
		m_ClickedLabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		poldBmp = MemDC.SelectObject(m_ClickedLabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	else if(m_bOver && m_MouseOverLabelBm.GetSafeHandle()!=NULL)
	{
		m_MouseOverLabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		poldBmp = MemDC.SelectObject(m_MouseOverLabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	else if(m_LabelBm.GetSafeHandle()!=NULL)
	{
		m_LabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		poldBmp = MemDC.SelectObject(m_LabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		MemDC.SelectObject( poldBmp );
	}
	else
	{
		nTextLeft=4;
	}
	///输出文字
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CString strText;
	this->GetWindowText(strText);
	nTextTop=rect.top+(rect.Height()-tm.tmHeight)/2;
	if (!strText.IsEmpty())
	{	
		// ???文字有点问题；
		if (m_alignText == Align_Default)
		{
			dc.TextOut(nTextLeft,nTextTop,strText);
		}else
		{
			rect.left += nTextLeft;
			UINT nFormat = DT_SINGLELINE|DT_VCENTER;
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

	MemDC.DeleteDC();
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
			RedrawWindow();
            ReleaseCapture();
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
        RedrawWindow();
        SetCapture();
		if (m_hHandCur != NULL)
			::SetCursor(m_hHandCur);
    }

	CStatic::OnMouseMove(nFlags, point);
}

void CLabelEx::SetTextColor(COLORREF crText)
{
	m_crText=crText;
}

void CLabelEx::MoveWindowPoint(int x, int y, BOOL bRepaint)
{
	if (this->GetSafeHwnd())
	{
		if (m_BGBm.operator HBITMAP())
		{
			this->MoveWindow(x, y, m_nImgWidth, m_nImgHeight);
		}else if (m_LabelBm.operator HBITMAP())
		{
			this->MoveWindow(x, y, m_nImgWidth, m_nImgHeight);
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

void CLabelEx::SetBGBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_BGBm.Detach());
	m_BGBm.Attach(ID);
	BITMAP stBitmap;
	m_BGBm.GetBitmap(&stBitmap);
	m_nImgWidth = stBitmap.bmWidth;
	m_nImgHeight = stBitmap.bmHeight;
}

void CLabelEx::SetBGBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_BGBm.Detach());
	m_BGBm.LoadBitmap(ID);
	BITMAP stBitmap;
	m_BGBm.GetBitmap(&stBitmap);
	m_nImgWidth = stBitmap.bmWidth;
	m_nImgHeight = stBitmap.bmHeight;
}

void CLabelEx::SetMouseOverBGBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
	m_MouseOverBGBm.Attach(ID);
}

void CLabelEx::SetMouseOverBGBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_MouseOverBGBm.Detach());
	m_MouseOverBGBm.LoadBitmap(ID);
}

void CLabelEx::SetClickedBGBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
	m_ClickedBGBm.Attach(ID);
}

void CLabelEx::SetClickedBGBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_ClickedBGBm.Detach());
	m_ClickedBGBm.LoadBitmap(ID);
}

void CLabelEx::SetLabelBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_LabelBm.Detach());
	m_LabelBm.Attach(ID);
	BITMAP stBitmap;
	m_LabelBm.GetBitmap(&stBitmap);
	m_nImgWidth = stBitmap.bmWidth;
	m_nImgHeight = stBitmap.bmHeight;
}

void CLabelEx::SetLabelBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_LabelBm.Detach());
	m_LabelBm.LoadBitmap(ID);
	BITMAP stBitmap;
	m_LabelBm.GetBitmap(&stBitmap);
	m_nImgWidth = stBitmap.bmWidth;
	m_nImgHeight = stBitmap.bmHeight;
}

void CLabelEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bClicked=!m_bClicked;
	RedrawWindow();
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

void CLabelEx::SetMouseOverLabelBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
	m_MouseOverLabelBm.Attach(ID);

}

void CLabelEx::SetMouseOverLabelBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_MouseOverLabelBm.Detach());
	m_MouseOverLabelBm.LoadBitmap(ID);
}

void CLabelEx::SetClickedLabelBitmap(HBITMAP ID)
{
	SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
	m_ClickedLabelBm.Attach(ID);

}

void CLabelEx::SetClickedLabelBitmap(UINT ID)
{
	SAFE_DELETEOBJECT(m_ClickedLabelBm.Detach());
	m_ClickedLabelBm.LoadBitmap(ID);

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