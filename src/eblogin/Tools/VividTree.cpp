///////////////////////////////////////////////////////////////////////////
// VividTree.cpp : Implementation of Class VividTree
///////////////////////////////////////////////////////////////////////////
//
// Copyright(C) 2005 Jim Alsup  All rights reserved
//              email: ja.alsup@gmail.com
//
// License: This code is provided "as is" with no expressed or implied 
//          warranty. You may use, or derive works from this file without
//          any restrictions except those listed below.
//
//        - This original header must be kept in the derived work.
//
//        - If your derived work is distributed in any form, you must
//          notify the author via the email address above and provide a 
//          short description of the product and intended audience.  
//
//        - You may not sell this code or a derived version of it as part of 
//          a comercial code library. 
//
//        - Offering the author a free licensed copy of any end product 
//          using this code is not required, but does endear you with a 
//          bounty of good karma.
//
///////////////////////////////////////////////////////////////////////////

//
// This owner draw tree control allows for the creation of a tree control
// that loosely resembles the user interface used by Skype's main window.
//
// Beside demonstrating the basics of creating an ownerdrawn tree control,
// but more importantly, demonstrates gradient backgrounds fills, bitmap 
// backgrounds, flicker free drawing, and how to deal with these in a 
// scrollable and resizable dialog.
//
// The underlying gradient or bitmap image is sized according the natural
// size of the visible (on screen or not) tree data.  This can be more
// readily understood and seen by opening and collasping the tree leaf
// with a long text name.  Notice the bmp or gradient resizes to the
// actual screen content.  
//
// Version History
//
// Sept 1, 2005  1.0.0  Initial Version

#include "stdafx.h"
#include "VividTree.h"
#include ".\vividtree.h"

#define _OWNER_DRAWN 1  // Set to 0 to use Windows draw code.  
#define TIMERID_UPDATETRACK_ITEM			998
#define TIMERID_CHECK_MOUSELEAVE_ITEM		999

// Gradient Filling Helper Routine
void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad )
{
	TRIVERTEX        vert[2];
	GRADIENT_RECT    mesh;

	vert[0].x      = rect.left;
	vert[0].y      = rect.top;
	vert[0].Alpha  = 0x0000;
	vert[0].Blue   = GetBValue(col_from) << 8;
	vert[0].Green  = GetGValue(col_from) << 8;
	vert[0].Red    = GetRValue(col_from) << 8;

	vert[1].x      = rect.right;
	vert[1].y      = rect.bottom; 
	vert[1].Alpha  = 0x0000;
	vert[1].Blue   = GetBValue(col_to) << 8;
	vert[1].Green  = GetGValue(col_to) << 8;
	vert[1].Red    = GetRValue(col_to) << 8;

	mesh.UpperLeft  = 0;
	mesh.LowerRight = 1;
#if _MSC_VER >= 1300  // only VS7 and above has GradientFill as a pDC member
	pDC->GradientFill( vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#else
	GradientFill( pDC->m_hDC, vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#endif
}



// CTreeCtrlEx

IMPLEMENT_DYNAMIC(VividTree, CTreeCtrl)

VividTree::VividTree()
{
	m_hCallback = NULL;
	m_gradient_bkgd_sel = RGB( 0x80, 0xa0, 0xff );	//Blueish
	m_gradient_bkgd_from = RGB( 0xff, 0xff, 0xff );	//White
	m_gradient_bkgd_to = RGB( 0xd5, 0xd5, 0xe0 );	//Light Greyish Blue
	m_bkgd_mode = BK_MODE_FILL;//BK_MODE_GRADIENT;
	m_bmp_tiled_mode = false;
	m_gradient_horz = true;
	m_crRectangle = 0;

	//m_top_txt_offset = 0;
	//m_top_ico_offset = 0;
	m_ico_width = 16;
	m_ico_height = 16;

    //VERIFY( m_bmp_tree_closed.LoadBitmap( IDB_TREE_CLOSED ) ) ; 
    //m_bmp_tree_closed.GetSafeHandle();
    //VERIFY( m_bmp_tree_open.LoadBitmap( IDB_TREE_OPENED ) ) ; 
    //m_bmp_tree_open.GetSafeHandle();
	m_icoRoot = NULL;
	m_icoParent = NULL;
	m_icon = NULL;

	m_hTrackItem = NULL;
	m_pOnDeleteItem = NULL;
	
	m_colorSelected = RGB(255,240,181);
	m_colorTracked = RGB(255,244,202);
	m_bDrawIntervalLine = false;
	m_crIntervalLine0 = 0;
	m_crIntervalLine1 = 0;
}


VividTree::~VividTree()
{
    if (m_bmp_back_ground.GetSafeHandle())
        m_bmp_back_ground.DeleteObject();
    if (m_bmp_tree_closed.GetSafeHandle())
        m_bmp_tree_closed.DeleteObject();
    if (m_bmp_tree_open.GetSafeHandle())
        m_bmp_tree_open.DeleteObject();
}


BEGIN_MESSAGE_MAP(VividTree, CTreeCtrl)
	ON_WM_ERASEBKGND()
#if _OWNER_DRAWN
	ON_WM_PAINT()
#endif
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTvnItemexpanding)
	//ON_WM_CREATE()
	//ON_NOTIFY_REFLECT(TVN_GETINFOTIP, &VividTree::OnTvnGetInfoTip)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &VividTree::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED, &VividTree::OnTvnItemChanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &VividTree::OnTvnSelChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &VividTree::OnTvnDeleteitem)
	ON_WM_TIMER()
	//ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &VividTree::OnTvnEndlabeledit)
	//ON_NOTIFY_REFLECT(NM_RCLICK, &VividTree::OnNMRClick)
END_MESSAGE_MAP()



// CVividTree message handlers
void VividTree::Sort(HTREEITEM hItem, PFNTVCOMPARE lpfnCompare)
{
	TVSORTCB    tvs;
	tvs.hParent = hItem;
	tvs.lParam = (LPARAM)this;
	tvs.lpfnCompare = lpfnCompare;
	SortChildrenCB(&tvs);
}

void VividTree::SetTreeOpenClosedBmp(UINT uOpenedId, UINT uClosedId)
{
    if (m_bmp_tree_closed.GetSafeHandle())
        m_bmp_tree_closed.DeleteObject();
    if (m_bmp_tree_open.GetSafeHandle())
        m_bmp_tree_open.DeleteObject();

    VERIFY( m_bmp_tree_closed.LoadBitmap( uClosedId ) ) ; 
    m_bmp_tree_closed.GetSafeHandle();
    VERIFY( m_bmp_tree_open.LoadBitmap( uOpenedId ) ) ; 
    m_bmp_tree_open.GetSafeHandle();

}

BOOL VividTree::OnEraseBkgnd(CDC* pDC)
{
    // nothing to do here -- see OnPaint
	return TRUE;
}


//#include "POP.h"
#if _OWNER_DRAWN
void VividTree::OnPaint()
{
	CPaintDC dc(this);	// Device context for painting
	CDC dc_ff;			// Memory base device context for flicker free painting
	CBitmap bm_ff;		// The bitmap we paint into
	CBitmap *bm_old;
	CFont *font, *old_font;
	CFont fontDC;
	int old_mode;

	GetClientRect(&m_rect);
	SCROLLINFO scroll_info;
	// Determine window portal to draw into taking into account
	// scrolling position
	if ( GetScrollInfo( SB_HORZ, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		m_h_offset = -scroll_info.nPos;
		m_h_size = max( scroll_info.nMax+1, m_rect.Width());
	}
	else
	{
		m_h_offset = m_rect.left;
		m_h_size = m_rect.Width();
	}
	if (m_crRectangle != 0)
	{
		m_h_offset += 1;
		m_h_size -= 2;
	}
	if ( GetScrollInfo( SB_VERT, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		if ( scroll_info.nMin == 0 && scroll_info.nMax == 100) 
			scroll_info.nMax = 0;
		m_v_offset = -scroll_info.nPos * GetItemHeight();
		m_v_size = max( (scroll_info.nMax+2)*((int)GetItemHeight()+1), m_rect.Height() );
	}
	else
	{
		m_v_offset = m_rect.top;
		m_v_size = m_rect.Height();
	}

	// Create an offscreen dc to paint with (prevents flicker issues)
	dc_ff.CreateCompatibleDC( &dc );
    bm_ff.CreateCompatibleBitmap( &dc, m_rect.Width(), m_rect.Height() );
    // Select the bitmap into the off-screen DC.
    bm_old = (CBitmap *)dc_ff.SelectObject( &bm_ff );
	// Default font in the DC is not the font used by 
	// the tree control, so grab it and select it in.
	font = GetFont();
	old_font = dc_ff.SelectObject( font );
	// We're going to draw text transparently
	old_mode = dc_ff.SetBkMode( TRANSPARENT );

	DrawBackGround( &dc_ff );
	try
	{
		DrawItems( &dc_ff );
	}catch(const std::exception&)
	{
		//theApp.LogMessage("VividTree::OnPaint()... exception 111\r\n");
	}catch(...)
	{
		//theApp.LogMessage("VividTree::OnPaint()... exception 222\r\n");
	}

	if (m_crRectangle != 0)
	{
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_crRectangle);		//边框颜色
		pOldPen = dc_ff.SelectObject(&BorderPen);
		dc_ff.MoveTo(m_rect.left,m_rect.top);
		dc_ff.LineTo(m_rect.right-1,m_rect.top);
		dc_ff.LineTo(m_rect.right-1,m_rect.bottom-1);
		dc_ff.LineTo(m_rect.left,m_rect.bottom-1);
		dc_ff.LineTo(m_rect.left,m_rect.top);
		dc_ff.SelectObject(pOldPen);
	}

    // Now Blt the changes to the real device context - this prevents flicker.
    dc.BitBlt( m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &dc_ff, 0, 0, SRCCOPY);

	dc_ff.SelectObject( old_font );
	dc_ff.SetBkMode( old_mode );
    dc_ff.SelectObject( bm_old );
}
#endif


// Draw TreeCtrl Background - 
void VividTree::DrawBackGround( CDC* pDC )
{
	BkMode mode = m_bkgd_mode;

	CRect rect(m_rect);
	if (m_crRectangle != 0)
	{
		//CPen BorderPen,*pOldPen;
		//BorderPen.CreatePen(PS_SOLID,1, m_crRectangle);		//边框颜色
		//pOldPen = pDC->SelectObject(&BorderPen);
		//pDC->Rectangle(m_rect);
		//pDC->SelectObject(pOldPen);
		rect.DeflateRect(1,1);
	}
	if ( mode == BK_MODE_BMP )
	{
		if ( !m_bmp_back_ground.GetSafeHandle() )
			mode = BK_MODE_GRADIENT;
	}
	if ( mode == BK_MODE_GRADIENT )
	{
		GradientFillRect( pDC, 
			CRect( m_h_offset, m_v_offset, m_h_size + m_h_offset, m_v_size + m_v_offset ), 
			m_gradient_bkgd_from, m_gradient_bkgd_to, !m_gradient_horz );
	}
	else if ( mode == BK_MODE_FILL )
	{
		pDC->FillSolidRect( rect, pDC->GetBkColor() ); 
	}else if ( mode == BK_MODE_BMP )
	{
		BITMAP bm;
		CDC dcMem;
	      
		VERIFY(m_bmp_back_ground.GetObject(sizeof(bm), (LPVOID)&bm));
		dcMem.CreateCompatibleDC(pDC);
		//dcMem.CreateCompatibleDC(NULL);
		CBitmap* bmp_old = (CBitmap*)dcMem.SelectObject( &m_bmp_back_ground ); 
		
		if ( m_bmp_tiled_mode ) 	// BitMap Tile Mode
		{
			for ( int y = 0; y <= (m_v_size / bm.bmHeight); y++ )
			{
				for ( int x = 0; x <= (m_h_size / bm.bmWidth); x++ )
				{
					pDC->BitBlt((x*bm.bmWidth) + m_h_offset, (y*bm.bmHeight) + m_v_offset,
						bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY);
				}
			}
		}
		else  // BITMAP Stretch Mode
		{
			pDC->StretchBlt( m_h_offset, m_v_offset, m_h_size, m_v_size, 
				&dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
		}
		// CleanUp
		dcMem.SelectObject( bmp_old );
	}
	else
		ASSERT( 0 );  // Unknown BackGround Mode
}

void VividTree::SetIntervalLine(bool bDrawIntervalLine, COLORREF color0, COLORREF color1)
{
	m_bDrawIntervalLine = bDrawIntervalLine;
	m_crIntervalLine0 = color0;
	m_crIntervalLine1 = color1;
}

#ifndef TVIS_FOCUSED
	#define TVIS_FOCUSED	1
#endif

//Gdiplus::Bitmap* ConvertToGrayscale(Gdiplus::Bitmap* source)
//{
//	Gdiplus::Bitmap* bm = new Gdiplus::Bitmap(source->GetWidth(),source->GetHeight());
//	for(int y=0;y<bm->GetHeight();y++)
//	{
//		for(int x=0;x<bm->GetWidth();x++)
//		{
//			Gdiplus::Color c;
//			source->GetPixel(x,y,&c);
//			int luma = (int)(c.GetR()*0.3 + c.GetG()*0.59+ c.GetB()*0.11);
//			bm->SetPixel(x,y,Gdiplus::Color(luma));
//		}
//	}
//	return bm;
//}
const Gdiplus::ColorMatrix theColorMatrix =
{0.299, 0.299, 0.299, 0.0, 0.0,
    0.587, 0.587, 0.587, 0.0, 0.0,
    0.114, 0.114, 0.114, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 1.0};
//const Gdiplus::ColorMatrix theColorMatrix =
//{0.3, 0.3, 0.3, 0.0, 0.0,
//    0.59, 0.59, 0.59, 0.0, 0.0,
//    0.11, 0.11, 0.11, 0.0, 0.0,
//    0.0, 0.0, 0.0, 1.0, 0.0,
//	0.0, 0.0, 0.0, 0.0, 1.0};

//
//Gdiplus::Image* ConvertToGrayscale2(Gdiplus::Image* source)
//{
//	Gdiplus::Image * pImage = source->Clone();
//	Gdiplus::Graphics graphics(source);
//	Gdiplus::ImageAttributes pImageAttributes;
//	pImageAttributes.SetColorMatrix(&theColorMatrix,ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
//	graphics.DrawImage(pImage,Gdiplus::Rect(0,0,source->GetWidth(),source->GetHeight()),0,0, source->GetWidth(),source->GetHeight(),Gdiplus::UnitPixel,&pImageAttributes);
//	return pImage;
//}

//bool GrayScaleByLockData(Bitmap * bm)
//{
//	int width = bm->GetWidth();
//	int height = bm->GetHeight();
//	Rect rect(0,0,width,height);
//	BitmapData data;
//	Status status;
//	status = bm->LockBits(&rect,ImageLockModeWrite,bm->GetPixelFormat(),&data);
//	if (status!=0)
//	{
//		bm->UnlockBits(&data);
//		return false;
//	}
//	byte * p = (byte*)data.Scan0;
//	int offset = data.Stride-width*3;
//	int i=0,j=0;
//	BYTE r,g,b;
//	for (i=0;i<height;i++)
//	{
//		for (j=0;j<width;j++)
//		{
//			b=p[0];
//			g=p[1];
//			r=p[2];
//			p[0]=p[1]=p[2]=(BYTE)(0.299*r+0.587*g+0.114*b);
//			p+=3;
//		}
//		p+=offset;
//	}
//	bm->UnlockBits(&data);
//	return true;
//}
// Draw TreeCtrl Items
void VividTree::DrawItems( CDC *pDC )
{
	// draw items
	HTREEITEM show_item, parent;
	CRect rc_item;
	CString name;
	COLORREF color;
	DWORD tree_style;
	BITMAP bm;
	CDC dc_mem;
	CBitmap *button;
	int count = 0;
	int state;
	bool selected;
	bool has_children;
	bool isrootitem = false;
	bool tracked = false;

	show_item = GetFirstVisibleItem();
	if ( show_item == NULL )
		return;

	dc_mem.CreateCompatibleDC(pDC);
	//dc_mem.CreateCompatibleDC(NULL);	// 会弹错误窗口
	color = pDC->GetTextColor();
	tree_style = ::GetWindowLong( m_hWnd, GWL_STYLE ); 

	Gdiplus::Graphics graphics(pDC->m_hDC);	// for Image
	CImageList m_ilTB;
	m_ilTB.Create(m_ico_width, m_ico_height, ILC_MASK|ILC_COLOR, 1, 0);
	do
	{
		tracked = show_item == m_hTrackItem;
		state = GetItemState( show_item, TVIF_STATE );
		parent = GetParentItem( show_item );
		has_children = ItemHasChildren( show_item ) || parent == NULL;
		selected = (state & TVIS_SELECTED) && ((this == GetFocus()) || (tree_style & TVS_SHOWSELALWAYS));
		//if (!selected)
		//	selected = state & TVIS_FOCUSED;
		isrootitem = has_children&&parent == NULL;//GetRootItem()==show_item;

		if ( GetItemRect( show_item, rc_item, TRUE ) )
		{
			if (m_crRectangle != 0)
			{
				if (rc_item.left==0)
					rc_item.left += 1;
				if (rc_item.top == 0)
					rc_item.top += 1;
				if (rc_item.right == m_rect.right)
					rc_item.right -= 1;
				if (rc_item.bottom == m_rect.bottom)
					rc_item.bottom -= 1;
			}
			int xoffset=0;
			if (/* has_children  || */selected || tracked)
			{
				CRect rect;
				rect.top = rc_item.top;
				rect.bottom = rc_item.bottom;
				rect.right = m_h_size + m_h_offset;
				//if ( !has_children )
				//	rect.left = rc_item.left + m_h_offset;
				//else
					rect.left = m_h_offset;
				
				// 255,255,217 比较淡
				// 251,239,193 深一点
				const COLORREF m_colorDefault = selected?m_colorSelected:m_colorTracked;
				//const COLORREF m_colorDefault = selected?RGB(251,239,193):RGB(255,255,217);
				//const COLORREF m_clrSelectBorder = selected?RGB(243,202,139):RGB(251,239,193);
				CPen BorderPen,*pOldPen;
				BorderPen.CreatePen(PS_SOLID,1, m_colorDefault);	// m_clrSelectBorder 边框颜色
				CBrush BKBrush,*pOldBrush;
				BKBrush.CreateSolidBrush( m_colorDefault );			//背景颜色
				pOldBrush = pDC->SelectObject(&BKBrush);
				//pDC->FillSolidRect(rect, m_colorDefault);
				pOldPen = pDC->SelectObject(&BorderPen);
				pDC->Rectangle(&rect);
				pDC->SelectObject(pOldPen);
				pDC->SelectObject(pOldBrush);

				pDC->SetTextColor( RGB( 0, 0, 255 ) );
			}else
			{
				if (m_bDrawIntervalLine)
				{
					CRect rect;
					rect.top = rc_item.top;
					rect.bottom = rc_item.bottom;
					rect.right = m_h_size + m_h_offset;
					//if ( !has_children )
					//	rect.left = rc_item.left + m_h_offset;
					//else
						rect.left = m_h_offset;

					const COLORREF colorLine = (count%2)==0?m_crIntervalLine0:m_crIntervalLine1;
					CPen BorderPen,*pOldPen;
					BorderPen.CreatePen(PS_SOLID,1, colorLine);	//边框颜色
					CBrush BKBrush,*pOldBrush;
					BKBrush.CreateSolidBrush( colorLine );		//背景颜色
					pOldBrush = pDC->SelectObject(&BKBrush);
					pOldPen = pDC->SelectObject(&BorderPen);
					pDC->Rectangle(&rect);
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}

				//if (has_children)
				//{
				//	CRect rect;
				//	rect.top = rc_item.top;
				//	rect.bottom = rc_item.bottom;
				//	rect.right = m_h_size + m_h_offset;
				//	rect.left = m_h_offset;

				//	COLORREF colorBg = RGB(250,250,250);
				//	CPen BorderPen,*pOldPen;
				//	BorderPen.CreatePen(PS_SOLID,1, colorBg);	//边框颜色
				//	CBrush BKBrush,*pOldBrush;
				//	BKBrush.CreateSolidBrush( colorBg );	//选中后的颜色
				//	pOldBrush = pDC->SelectObject(&BKBrush);
				//	//pDC->FillSolidRect(rect, m_colorDefault);
				//	pOldPen = pDC->SelectObject(&BorderPen);
				//	pDC->Rectangle(&rect);
				//	pDC->SelectObject(pOldPen);
				//	pDC->SelectObject(pOldBrush);
				//}

				//if (count%2==1)
				//{
				//	CRect rect;
				//	rect.top = rc_item.top;
				//	rect.bottom = rc_item.bottom;
				//	rect.right = m_h_size + m_h_offset;
				//	rect.left = rc_item.left;

				//	COLORREF colorBg = RGB(250,250,250);
				//	CPen BorderPen,*pOldPen;
				//	BorderPen.CreatePen(PS_SOLID,1, colorBg);	//边框颜色
				//	CBrush BKBrush,*pOldBrush;
				//	BKBrush.CreateSolidBrush( colorBg );	//选中后的颜色
				//	pOldBrush = pDC->SelectObject(&BKBrush);
				//	//pDC->FillSolidRect(rect, m_colorDefault);
				//	pOldPen = pDC->SelectObject(&BorderPen);
				//	pDC->Rectangle(&rect);
				//	pDC->SelectObject(pOldPen);
				//	pDC->SelectObject(pOldBrush);
				//}
			}
			count++;
			CBitmap * pBitmapOpen = NULL;
			CBitmap * pBitmapClosed = NULL;
			const bool bDrawOpenClose = m_hCallback==NULL?has_children:m_hCallback->GetItemDrawOpenClose(*this,show_item,&pBitmapOpen,&pBitmapClosed);
			if ( bDrawOpenClose )
			{
				// Draw an Open/Close button
				if ( state & TVIS_EXPANDED )
					button = pBitmapOpen!=NULL?pBitmapOpen:&m_bmp_tree_open;
				else
					button = pBitmapClosed!=NULL?pBitmapClosed:&m_bmp_tree_closed;
				if (button!=NULL && button->GetSafeHandle()!=NULL)		// add by hd
				{
					VERIFY(button->GetObject(sizeof(bm), (LPVOID)&bm));
					CBitmap *bmp_old = (CBitmap*)dc_mem.SelectObject(button); 
					pDC->BitBlt( rc_item.left+xoffset, rc_item.top+(rc_item.Height()-bm.bmHeight)/2, bm.bmWidth, bm.bmHeight, 
						//pDC->BitBlt( rc_item.left - bm.bmWidth - 2, rc_item.top, bm.bmWidth, bm.bmHeight, 
						&dc_mem, 0, 0, SRCAND );
					// CleanUp
					dc_mem.SelectObject( bmp_old );
					xoffset += (bm.bmWidth+2);
				}
			}

			//if ( !has_children )
			{
				//Gdiplus::Bitmap pImage;
				//pImage.dr

				const int top_ico_offset = (rc_item.Height()-m_ico_height)/2;

				// lookup the ICON instance (if any) and draw it
				Gdiplus::Image * pImage1 = NULL;
				Gdiplus::Image * pImage2 = NULL;
				int nState = 1;
				bool bRet = m_hCallback==NULL?false:m_hCallback->GetItemImage(*this,show_item,pImage1,pImage2,nState);
				if (bRet)
				{
					xoffset += 1;
					assert (pImage1 != NULL);
					Gdiplus::Rect destRect(rc_item.left+xoffset, rc_item.top+top_ico_offset, m_ico_width, m_ico_height);
					if (nState == 1)
					{
						graphics.DrawImage(pImage1,destRect,0,0, pImage1->GetWidth()+1, pImage1->GetHeight()+1,Gdiplus::UnitPixel);
						if (pImage2 != NULL)
						{
							destRect.X += (m_ico_width-pImage2->GetWidth());
							destRect.Y += (m_ico_height-pImage2->GetHeight());
							destRect.Width = pImage2->GetWidth();
							destRect.Height = pImage2->GetHeight();
							graphics.DrawImage(pImage2,destRect,0,0, pImage2->GetWidth(), pImage2->GetHeight(),Gdiplus::UnitPixel);
						}
					}else
					{
						// 显示灰色
						//Gdiplus::Graphics graphics2(pImage1);	// for Image
						//graphics2.DrawImage(pImage1,destRect,0,0, pImage1->GetWidth(), pImage1->GetHeight(),Gdiplus::UnitPixel);
						//Bitmap * pBitmap = new Bitmap(pImage1->GetWidth(),pImage1->GetHeight(),&graphics2);
						////if (GrayScaleByLockData(pBitmap))
						//{
						//	graphics.DrawImage(pBitmap,destRect,0,0, pImage1->GetWidth()+1, pImage1->GetHeight()+1,Gdiplus::UnitPixel);
						//	delete pBitmap;
						//}
						Gdiplus::ImageAttributes pImageAttributes;
						pImageAttributes.SetColorMatrix(&theColorMatrix,ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
						graphics.DrawImage(pImage1,destRect,0,0, pImage1->GetWidth()+1, pImage1->GetHeight()+1,Gdiplus::UnitPixel,&pImageAttributes);
					}
					delete pImage1;
					if (pImage2 != NULL)
					{
						delete pImage2;
					}
					xoffset += (m_ico_width+3);
				}else
				{
					HICON icon1 = NULL;
					HICON icon2 = NULL;
					int nState = 1;
					bRet = m_hCallback==NULL?NULL:m_hCallback->GetItemIcon(*this,show_item,icon1,icon2,nState);
					if (!bRet)
						icon1 = GetItemIcon( show_item, has_children);
					if ( icon1 != NULL )
					{
						xoffset += 1;
						//HICON hMinIcon = NULL;
						//if (m_hCallback == NULL || m_hCallback->GetItemState(*this,show_item, hMinIcon) == 1)
						if (nState == 1)
						{
							// 正常显示
							DrawIconEx( pDC->m_hDC, rc_item.left+xoffset, rc_item.top+top_ico_offset, icon1, m_ico_width, m_ico_height,0,0, DI_NORMAL );
							if (icon2 != NULL)
							{
								m_ilTB.Add(icon2);
								m_ilTB.Draw(pDC, 0, CPoint(rc_item.left+xoffset, rc_item.top+top_ico_offset),ILD_NORMAL);
								m_ilTB.Remove(0);
							}
						}else
						{
							// 显示灰色
							m_ilTB.Add(icon1);
							m_ilTB.Draw(pDC, 0, CPoint(rc_item.left+xoffset, rc_item.top+top_ico_offset),ILD_NORMAL);
							m_ilTB.Remove(0);
						}
						xoffset += (m_ico_width+3);
					}
				}
			}
			name = GetItemText( show_item );
			rc_item.DeflateRect( xoffset,1,(xoffset*-1)-2,1 );
			//rc_item.DeflateRect( xoffset,1+m_top_txt_offset,(xoffset*-1)-2,1 );

			const int nFind = name.Find(_T("\n"));
			CString name1;
			CString name2;
			CRect rect1 = rc_item;
			CRect rect2 = rc_item;
			if (nFind > 0)
			{
				// 分二行显示
				name1 = name.Left(nFind);
				name2 = name.Mid(nFind+1);
				rect1.bottom = rect1.top + rc_item.Height()/2;
				rect2.top = rect1.bottom+1;
			}else
			{
				// 一行
				rc_item.top += (rc_item.Height()-16)/2;
			}

			COLORREF colorItem = m_hCallback==NULL?color:m_hCallback->GetItemTextColor(*this,show_item,color);
			//rc_item.DeflateRect( m_ico_width+2,1+m_top_txt_offset,(m_ico_width*-1)-2,1 );
			if ( selected )
			{
				// 白亮，更不好
				//if ( !has_children  )
				//	pDC->SetTextColor( GetSysColor(COLOR_HIGHLIGHTTEXT) );
				pDC->SetTextColor( colorItem );
				COLORREF col = pDC->GetBkColor();
				pDC->SetBkColor( GetSysColor(COLOR_HIGHLIGHT) );
				if (nFind > 0)	// 分二行显示
				{
					pDC->DrawText( name1, rect1, DT_LEFT );
					const bool nFindCoor = name2.Left(4)==_T("CLR=")?true:false;
					const int nFind = nFindCoor?name2.Find(";",4):-1;
					if (nFindCoor && nFind>0)
					{
						const COLORREF colorName = atol(name2.Mid(4,nFind-4));
						name2 = name2.Mid(nFind+1);
						pDC->SetTextColor( colorName );
					}
					pDC->DrawText( name2, rect2, DT_LEFT );
				}else
				{
					pDC->DrawText( name, rc_item, DT_LEFT|DT_VCENTER );
				}
				pDC->SetTextColor( color );
				pDC->SetBkColor( col );
			}
			else
			{
				pDC->SetTextColor( colorItem );
				if (nFind > 0)	// 分二行显示
				{
					pDC->DrawText( name1, rect1, DT_LEFT );
					const bool nFindCoor = name2.Left(4)==_T("CLR=")?true:false;
					const int nFind = nFindCoor?name2.Find(";",4):-1;
					if (nFindCoor && nFind>0)
					{
						const COLORREF colorName = atol(name2.Mid(4,nFind-4));
						name2 = name2.Mid(nFind+1);
						pDC->SetTextColor( colorName );
					}
					pDC->DrawText( name2, rect2, DT_LEFT );
				}else
				{
					pDC->DrawText( name, rc_item, DT_LEFT|DT_VCENTER );
				}
				pDC->SetTextColor( color );
			}
			//if ( state & TVIS_BOLD )
			//	pDC->SelectObject( font );
		}
	} while ( (show_item = GetNextVisibleItem( show_item )) != NULL );
}



void VividTree::SetBitmapID( UINT id )
{
	// delete any existing bitmap
    if (m_bmp_back_ground.GetSafeHandle())
        m_bmp_back_ground.DeleteObject();
	// add in the new bitmap
    VERIFY( m_bmp_back_ground.LoadBitmap( id ) ) ; 
    m_bmp_back_ground.GetSafeHandle();
}



// Determine if a referenced item is visible within the control window
bool VividTree::ItemIsVisible( HTREEITEM item )
{
	HTREEITEM scan_item;
	scan_item = GetFirstVisibleItem();
	while ( scan_item != NULL )
	{
		if ( item == scan_item )
			return true;
		scan_item = GetNextVisibleItem( scan_item );
	}
	return false;
}


// For a given tree node return an ICON for display on the left side.
// This default implementation only returns one icon.
// This function is virtual and meant to be overriden by deriving a class
// from VividTree and supplying your own icon images. 
HICON VividTree::GetItemIcon( HTREEITEM item, bool hadchild)		
{
	//if (GetRootItem() == item)
	if (GetParentItem(item) == NULL && hadchild)
	{
		return m_icoRoot;
	}else if (hadchild)
	{
		return m_icoParent;
	}
	return m_icon;  // default implementation - overridable
}


// If the background is a bitmap, and a tree is expanded/collapsed we
// need to redraw the entire background because windows moves the bitmap
// up (on collapse) destroying the position of the background.
void VividTree::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if ( GetBkMode() == VividTree::BK_MODE_BMP && ItemIsVisible( pNMTreeView->itemNew.hItem ) )
		Invalidate();  // redraw everything
	*pResult = 0;
}

void VividTree::OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//m_hTrackItem = pGetInfoTip->hItem;
	//Invalidate();  // redraw everything
}

void VividTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//LPNMTVCUSTOMDRAW pDraw=(LPNMTVCUSTOMDRAW)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//DWORD dwDrawStage=pDraw->dwDrawStage;
	//UINT  uItemState =pDraw->uItemState;
	//*pResult=CDRF_NOTIFYITEMDRAW;
	////	
	//if((dwDrawStage & CDDS_ITEM) && (uItemState & CDIS_SELECTED))
	//{  
	//	HTREEITEM hItem=(HTREEITEM) pDraw->dwItemSpec;
	//	CDC* pdc=CDC::FromHandle(pDraw->hdc);
	//	COLORREF clr = RGB(123,0,0);
	//	pdc->SetBkColor(clr);
	//	*pResult |= CDRF_SKIPDEFAULT;
	//}

}

void VividTree::OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_hTrackItem = NULL;
	Invalidate();  // redraw everything
}
void VividTree::OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (!this->IsOnDeleteItem(pNMTreeView->itemNew.hItem))
	//if (!m_pOnDeleteList.exist(pNMTreeView->itemNew.hItem))
		this->GetParent()->PostMessage(WM_ITEM_SEL_CHANGED, (WPARAM)pNMTreeView->itemNew.hItem, (LPARAM)this);
}
void VividTree::UpdateTrackItem(void)
{
	try
	{
		HTREEITEM hPrevTrackItem = m_hTrackItem;
		CRect rect;
		POINT pt;
		TVHITTESTINFO pHitTestInfo;
		GetCursorPos(&pt);            //鼠标位置
		::GetWindowRect(m_hWnd,&rect); //窗口位置 
		pHitTestInfo.pt.x = pt.x - rect.left;  //鼠标相对x位置
		pHitTestInfo.pt.y = pt.y - rect.top;   //鼠标相对y位置
		HTREEITEM hTrackItem = HitTest(&pHitTestInfo);     //停留测试
		BOOL bChanged = m_hTrackItem!=hTrackItem;
		m_hTrackItem = hTrackItem;
		if (bChanged)
		{
			Invalidate();  // redraw everything
			this->GetParent()->PostMessage(WM_ITEM_TRACK_HOT, (WPARAM)m_hTrackItem, (LPARAM)this);

			if (hPrevTrackItem==NULL && m_hTrackItem!=NULL)
			{
				SetTimer(TIMERID_CHECK_MOUSELEAVE_ITEM,500,NULL);
			}
		}
		if (hPrevTrackItem!=NULL && m_hTrackItem==NULL)
		{
			KillTimer(TIMERID_CHECK_MOUSELEAVE_ITEM);
		}
	}catch(const std::exception&)
	{
		//theApp.LogMessage("OnTreeItemTrackHot... exception 111\r\n");
	}catch(...)
	{
		//theApp.LogMessage("OnTreeItemTrackHot... exception 222\r\n");
	}
}

void VividTree::OnMouseMove(UINT nFlags, CPoint point)
{
	UpdateTrackItem();
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void VividTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	POINT pt;
	TVHITTESTINFO pHitTestInfo;
	GetCursorPos(&pt);            //鼠标位置
	::GetWindowRect(m_hWnd,&rect); //窗口位置 
	pHitTestInfo.pt.x = pt.x - rect.left;  //鼠标相对x位置
	pHitTestInfo.pt.y = pt.y - rect.top;   //鼠标相对y位置
	HTREEITEM hDblClkItem = HitTest(&pHitTestInfo);     //停留测试
	if (hDblClkItem)
	{
		this->GetParent()->PostMessage(WM_ITEM_DOUBLE_CLICK, (WPARAM)hDblClkItem, (LPARAM)this);
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void VividTree::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	//m_pOnDeleteList.insert(pNMTreeView->itemOld.hItem,true,false);
	m_pOnDeleteItem = pNMTreeView->itemOld.hItem;
	if (m_hTrackItem != NULL && m_hTrackItem == pNMTreeView->itemOld.hItem)
	{
		SetTimer(TIMERID_UPDATETRACK_ITEM, 20, NULL);
	}
}

void VividTree::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_UPDATETRACK_ITEM == nIDEvent)
	{
		KillTimer(nIDEvent);
		UpdateTrackItem();
	}else if (TIMERID_CHECK_MOUSELEAVE_ITEM == nIDEvent)
	{
		if (m_hTrackItem != NULL)
		{
			CRect rect;
			GetClientRect(rect);
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);
			if(!rect.PtInRect(pos))	
			{
				KillTimer(TIMERID_CHECK_MOUSELEAVE_ITEM);
				m_hTrackItem = NULL;
				Invalidate();  // redraw everything
				this->GetParent()->PostMessage(WM_ITEM_TRACK_HOT, (WPARAM)m_hTrackItem, (LPARAM)this);
			}
		}
	}
	CTreeCtrl::OnTimer(nIDEvent);
}
//
//void VividTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//	HTREEITEM hEditItem = pTVDispInfo->item.hItem;
//	this->GetParent()->PostMessage(WM_ITEM_END_EDIT, (WPARAM)hEditItem, (LPARAM)this);
//}

//void VividTree::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//
//	CRect rect;
//	POINT pt;
//	TVHITTESTINFO pHitTestInfo;
//	GetCursorPos(&pt);            //鼠标位置
//	::GetWindowRect(this->m_hWnd, &rect); //窗口位置 
//	pHitTestInfo.pt.x = pt.x - rect.left;  //鼠标相对x位置
//	pHitTestInfo.pt.y = pt.y - rect.top;   //鼠标相对y位置
//	HTREEITEM hTrackItem = HitTest(&pHitTestInfo);     //停留测试
//	if (hTrackItem!=NULL)
//	{
//		//SelectItem(hTrackItem);
//	}
//}

HTREEITEM VividTree::SelectHitTest(void)
{
	HTREEITEM hSelected = this->GetSelectedItem();
	CRect rect;
	POINT pt;
	TVHITTESTINFO pHitTestInfo;
	GetCursorPos(&pt);            //鼠标位置
	::GetWindowRect(this->m_hWnd, &rect); //窗口位置 
	pHitTestInfo.pt.x = pt.x - rect.left;  //鼠标相对x位置
	pHitTestInfo.pt.y = pt.y - rect.top;   //鼠标相对y位置
	HTREEITEM hHitTest = HitTest(&pHitTestInfo);     //停留测试
	if (hHitTest!=NULL && hSelected!=hHitTest)
	{
		this->SelectItem(hHitTest);
		//this->Select(hHitTest,TVGN_CARET);
	}
	return hHitTest;
}
