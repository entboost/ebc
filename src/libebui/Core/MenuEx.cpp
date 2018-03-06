// MenuEx.cpp: implementation of the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuEx.h"

#define USES_CHECKED 0	// 0/1

#if (USES_CHECKED)
#include "resource.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern HINSTANCE g_hResourceHandle;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC( CMenuEx, CMenu )
CMenuEx::CMenuEx():m_szImage(16,15)
{
	//m_colMenu =::GetSysColor(COLOR_MENU);
	m_clrMenu = RGB(239,243,255);
//	m_colMenu = RGB(255,0,0);
	m_clrText =::GetSysColor(COLOR_MENUTEXT);
	m_clrTextSelected =::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_clrLeft = RGB(210,210,210);
	m_clrSelectBorder = RGB(49,106,197);
	m_clrSelectRect = RGB(193,210,238);

	m_bHasImageLeft =FALSE;
	m_nSeparator = 10;	//sparator的默认高度
//#ifdef SUN_4
#if (USES_CHECKED)
	m_szCheck[0] = 0;
	m_szCheck[1] = 0;
	LoadString( NULL , IDS_MENUEX_CHECK , m_szCheck , sizeof(m_szCheck)/sizeof(WCHAR) );
#endif
}

CMenuEx::~CMenuEx()
{
	m_ImageList.DeleteImageList();

	if(m_bHasImageLeft)
		m_bmpImageLeft.DeleteObject();

	int i;
	//释放所有的子菜单
	for( i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		delete pSubMenu;
	}
	
	//释放所有的菜单项
	for(i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		MENUITEM *pMenuItem = m_MenuItemArr.GetAt(i);
		delete pMenuItem;
	}
	
	//销毁菜单
	DestroyMenu();
}
/////////////////////////////////////////////////
//当菜单项为不可用时绘制灰色的文本
void CMenuEx::GrayString(CDC *pDC, const CString &str, const CRect rect)
{
	CRect	rt(rect);
	//int nMode =pDC->SetBkMode(TRANSPARENT);

	rt.left +=1;
	rt.top +=1;
		
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str,&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);

	rt.left -=1;
	rt.top -=1;
	pDC->SetTextColor(RGB(127,127,127));
	pDC->DrawText(str,&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);

	//pDC->SetBkMode(nMode);
}
/////////////////////////////////////////////////
//绘制菜单项位图
void CMenuEx::DrawMenuItemImage(CDC *pDC, CRect &rect, BOOL bSelected, BOOL bChecked,
								BOOL bGrayed, BOOL bHasImage,LPMENUITEM lpItem)
{
	CRect	rt(rect.left ,rect.top ,rect.left + m_szImage.cx + 4,
									rect.top + m_szImage.cy + 4);

	if(bChecked)
	{	
		if(bGrayed)
		{	
			//菜单不可用
#if (USES_CHECKED)
//#ifdef SUN_4
			GrayString(pDC,m_szCheck ,rt);
//#else
//			GrayString(pDC,"√",rt);
#endif
		}
		else
		{
			if(bSelected)
			{
				//菜单选中
				//当该项被选中仅多绘制一个立体矩形
				//pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}

			rt.InflateRect(-2,-2);
			
			//画出"√"
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_clrText);
//#ifdef SUN_4
#if (USES_CHECKED)
			pDC->DrawText( m_szCheck ,&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
//#else
//			pDC->DrawText("√",&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
#endif
		}
		
		rect.left +=m_szImage.cx + 4 +2 ;
		
		return ;
	}
	
	//std::map<UINT, HICON>::iterator pIter = m_mapIcons.find(lpItem->uID);
	//if (pIter == m_mapIcons.end())
	//	return;

	HICON hIcon = lpItem->hIcons->GetIconByCommandId(lpItem->uID);
	if(hIcon)
	//if(bHasImage)
	{
		CPoint pt(rt.left+2 , rt.top+2 );
		if(bSelected)
		{
			//pt.x = rt.left + 1;
			pt.y = rt.top + 1;
		}
		//UINT	uStyle = ILD_IMAGE;	//CImageList::Draw()绘制位图的风格
		UINT	uStyle =ILD_TRANSPARENT;	//CImageList::Draw()绘制位图的风格
		if(bGrayed)
		{
			//uStyle |=ILD_BLEND50;	//菜单不可用所以位图较暗
		}
		else
		{
			if(bSelected)
			{
				//当该项被选中仅多绘制一个立体矩形
				//pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}
		}
//		pDC->FillSolidRect(rect.left + 2,rect.top + 2,m_szImage.cx,m_szImage.cy,0);
		//lpItem->pImageList->Draw(pDC,lpItem->uIndex,pt,uStyle);	//在菜单项中绘制位图

		//HICON hIcon = ::AfxGetApp()->LoadIcon(lpItem->uIndex);
		::DrawIconEx(pDC->m_hDC, pt.x, pt.y, hIcon,
			16, 16, 0, NULL,
			DI_NORMAL);


		//调整可绘制矩形的大小
		//4：位图外接矩形比位图大4
		//2：菜单文本与位图外接矩形的间隔为2
		//rect.left  +=m_szImage.cx + 4 + 2;
	}
}
/////////////////////////////////////////////////
//绘制菜单项文本
//参数：rect:立体矩形的RECT
//		rtText:菜单文本的RECT
void CMenuEx::TextMenu(CDC *pDC, CRect rect,CRect rcText,BOOL bSelected, BOOL bGrayed, LPMENUITEM lpItem)
{
	//选中状态的菜单项要先画出立体矩形
	
	if(bSelected)
	{
		CRect rcItem = rect;
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_clrSelectBorder);	//边框颜色
		CBrush BKBrush,*pOldBrush;
		BKBrush.CreateSolidBrush( m_clrSelectRect );	//选中后的颜色
		pOldPen = pDC->SelectObject(&BorderPen);
		pOldBrush = pDC->SelectObject(&BKBrush);
		
		pDC->Rectangle(&rcItem);
		
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}

	if(bGrayed)
	{
		//GrayString(pDC,lpItem->strText,rtText);
		CPoint pt(rcText.left,rcText.top);
		CSize si(rcText.Width(),rcText.Height());
		pDC->DrawState(pt, si, lpItem->strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		//pDC->DrawText(lpItem->strText,rtText,DT_LEFT | DT_EXPANDTABS | DT_VCENTER | DT_GRAY);
	}
	else
	{
		pDC->DrawText(lpItem->strText,rcText,DT_LEFT | DT_EXPANDTABS | DT_VCENTER);
	}
}

void CMenuEx::DrawImageLeft(CDC *pDC, CRect &rect,LPMENUITEM lpItem)
{
	if(!m_bHasImageLeft || lpItem->uPositionImageLeft == -1)
		return ;
	
	CDC  memDC;
	if(!memDC.CreateCompatibleDC(pDC)) return;
	CBitmap	*oldBmp =(CBitmap *) memDC.SelectObject(&m_bmpImageLeft);

	int cy;	//设定该菜单项应从哪画起

	//cy = m_szImageLeft.cy - rect.Height() * lpItem->uPositionImageLeft;
	cy = 0;
	pDC->BitBlt(rect.left ,rect.top ,m_szImageLeft.cx ,rect.Height(),&memDC,0,cy,SRCCOPY);

	memDC.SelectObject(oldBmp);
	memDC.DeleteDC();

	rect.left +=m_szImageLeft.cx+1;
}

void CMenuEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC	dc;
	LPMENUITEM lpItem;
	CRect rect(lpDIS->rcItem);
	dc.Attach(lpDIS->hDC);
	lpItem =(LPMENUITEM)lpDIS->itemData;

	//被选中的话,使用被选中的文本颜色
	if(lpDIS->itemState & ODS_SELECTED)
		dc.SetTextColor(m_clrTextSelected);
	else
		dc.SetTextColor(m_clrText);

	//设定背景色
	CBrush brush(m_clrMenu);
	dc.FillRect(&rect, &brush);

	CRect rcLeft = rect;
	rcLeft.right = rcLeft.left + m_szImage.cx + 5;

	CRect rcText = rect;
	rcText.left += (m_szImage.cx + 10);
	rcText.top += 4;
	
	dc.FillSolidRect(&rcLeft,/*RGB(49,105,222)*/m_clrLeft);
	
	//设定显示模式
	dc.SetBkMode(TRANSPARENT);

	//绘制侧边位图
	//DrawImageLeft(&dc,rect,lpItem);
		
	if(lpItem->uID==0)//分隔条
	{
		rcText.top =rcText.Height()/2+rcText.top ;
		rcText.bottom =rcText.top +2;
		rcText.right -=2;
		dc.Draw3dRect(rcText,RGB(64,0,128),RGB(255,255,255));
	}
	else
	{

		BOOL	bSelected =lpDIS->itemState & ODS_SELECTED;
		BOOL	bChecked  =lpDIS->itemState & ODS_CHECKED;
		BOOL	bGrayed	  =lpDIS->itemState & ODS_GRAYED;
		BOOL	bHasImage =(lpItem->uIndex != -1);
				  
		//绘制菜单文本
		TextMenu(&dc,rect,rcText,bSelected,bGrayed,lpItem);
		
		//绘制菜单位图
		DrawMenuItemImage(&dc,rect,bSelected,bChecked,bGrayed,bHasImage,lpItem);
	}
	dc.SetBkMode(OPAQUE);
	dc.Detach();
}

//////////////////////////////////////////////////////////
//由工具栏的位图来产生菜单所用的位图列表m_ImageList
int CMenuEx::GetImageFromToolBar(UINT uToolBar, CToolBar *pToolBar,COLORREF	crMask/*工具栏位图的掩码*/,UINT uBitmap)
{
	if(!pToolBar)
		return 0;

	CBitmap	bmp;
	int nWidth,nHeight;
	BITMAP	bmpInfo;

	bmp.LoadBitmap(uToolBar);
	bmp.GetBitmap(&bmpInfo);
	//得到位图的高度
	nHeight =bmpInfo.bmHeight;

	int nCount=0;
	int	ret =pToolBar->GetToolBarCtrl().GetButtonCount();

	//得到工具栏中位图的个数nCount
	for(int i=0;i<ret;i++)
	{
		UINT uID = pToolBar->GetItemID(i);
		for(int j = 0; j < m_MenuItemArr.GetSize(); j++)
		{
			MENUITEM *pMenuItem = m_MenuItemArr.GetAt(j);
			if(pMenuItem->uID == uID)
			{
				pMenuItem->uIndex = i;
			}
		}


		if(uID != ID_SEPARATOR)
			nCount ++;
	}

/*
	for(int j = 0; j < m_MenuItemArr.GetSize(); j++)
	{
		MENUITEM *pMenuItem = m_MenuItemArr.GetAt(j);
		pMenuItem->uIndex = 2;
	}
*/

	//计算出位图的宽度
	nWidth =bmpInfo.bmWidth/nCount;

	bmp.DeleteObject();

	TRACE("Menu Bitmap--width:%d\theight:%d\n",nWidth,nHeight);
	
	//创建位图列表
	BOOL bRet = FALSE;
	if(uBitmap)
	{
		m_ImageList.Create(uBitmap,nWidth,nHeight,crMask);
	}
	else
	{
		bRet = m_ImageList.Create(uToolBar,nWidth,ILC_COLOR24,crMask);
		
	}
	m_szImage.cx =nWidth;
	m_szImage.cy =nHeight;

	return nCount;
}

void CMenuEx::MeasureItem(LPMEASUREITEMSTRUCT  lpMIS)
{
	MENUITEM *lpItem =(LPMENUITEM)lpMIS->itemData;

	if(lpItem->uID==0)//分隔条
	{
		lpMIS->itemHeight =m_nSeparator;
	}
	else
	{
		//CWnd * pWnd = AfxGetMainWnd();
		CWnd * pWnd = CWnd::FromHandle(::GetDesktopWindow());
		if (pWnd == NULL)
			return;
		CDC	 *pDC = pWnd->GetDC();

		CString	strText=lpItem->strText;
		CSize  size;
		size=pDC->GetTextExtent(lpItem->strText);
		lpMIS->itemWidth = size.cx /*+m_szImage.cx+6 */ ;		

		lpMIS->itemWidth += m_szImage.cx;
		
		//lpMIS->itemWidth += 30;
		if(/*m_bHasImageLeft &&*/ lpItem->uPositionImageLeft >= 0)
			lpMIS->itemWidth += 30;
		
		if(lpItem->uID == -1)
		{
			lpMIS->itemWidth += 15;
		}

		
		lpMIS->itemHeight =m_szImage.cy+4;
		
		pWnd->ReleaseDC(pDC);
	}
}

void CMenuEx::SetImageLeft(UINT idBmpLeft,BOOL bIsPopupMenu)
{
	if(m_bHasImageLeft)
		m_bmpImageLeft.DeleteObject();
	m_bmpImageLeft.LoadBitmap(idBmpLeft);
	m_bHasImageLeft = TRUE;
	BITMAP	bmpInfo;
	m_bmpImageLeft.GetBitmap(&bmpInfo);
	m_szImageLeft.cx =bmpInfo.bmWidth;
	m_szImageLeft.cy =bmpInfo.bmHeight;
	for(int i=0; i < m_MenuItemArr.GetSize(); i++)
	{
		MENUITEM *pMenuItem = m_MenuItemArr.GetAt(i);
		pMenuItem->uPositionImageLeft = 1;
	}

	//重新计算菜单项左边位图的位置
/*
	int MenuItemCount = this->GetMenuItemCount();
	if(bIsPopupMenu)
	{
		CalculateLeftImagePos(this);
	}	
	else
	{
		int nMenuItemCount = this->GetMenuItemCount();
		for(int i = 0; i < nMenuItemCount; i++)
		{
			int nID = this->GetMenuItemID(i);
			if(nID == -1)
			{
				CMenu *pSubMenu = this->GetSubMenu(i);
				CalculateLeftImagePos(pSubMenu);
			}
		}
	}*/

}
void CMenuEx::SetTextColor(COLORREF crColor)
{
	m_clrText =crColor;
	for(int i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		pSubMenu->SetTextColor(crColor);
	}
}

void CMenuEx::SetBackColor(COLORREF crColor)
{
	m_clrMenu =crColor;
	for(int i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		pSubMenu->SetBackColor(crColor);
	}
}

void CMenuEx::SetHighLightTextColor(COLORREF crColor)
{
	m_clrTextSelected =crColor;
	for(int i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		pSubMenu->SetHighLightTextColor(crColor);
	}
}

void CMenuEx::SetHightLightRectColor( COLORREF clrRect , COLORREF clrBorder )
{
	m_clrSelectRect =clrRect;
	m_clrSelectBorder = clrBorder;
	for(int i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		pSubMenu->SetHightLightRectColor( clrRect , clrBorder );
	}
}
void CMenuEx::SetLeftColor( COLORREF crColor )
{
	m_clrLeft =crColor;
	for(int i = 0; i < m_SubMenuArr.GetSize(); i++)
	{
		CMenuEx *pSubMenu = m_SubMenuArr.GetAt(i);
		pSubMenu->SetLeftColor(crColor);
	}
}

void CMenuEx::LoadToolBar(UINT uToolBar, UINT uFace)
{
	CToolBar m_wndToolBar;
	if (!m_wndToolBar.CreateEx(::AfxGetMainWnd(), TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(uToolBar))
	{
		return;
	}
	GetImageFromToolBar(uToolBar,&m_wndToolBar,RGB(192,192,192),0);
	//InitMenu(this,uToolBar,&m_wndToolBar,uFace);
}

bool CMenuEx::AddIcon(UINT nCommandID, UINT nIconResource)
{
	HICON hIcon = ::AfxGetApp()->LoadIcon(nIconResource);
	return m_hicons.AddIcon(nCommandID, hIcon);
}

BOOL CMenuEx::RemoveMenuEx(UINT nPosition, UINT nFlags)
{
	UINT uItemID = 0;
	if(nFlags & MF_BYPOSITION)
	{
		uItemID = GetMenuItemID((int)nPosition);
	}
	else
	{
		uItemID = nPosition;
	}
	if((int) uItemID >= 0)			//普通菜单项或分割条
	{
		for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
		{
			MENUITEM *pMenuItem = m_MenuItemArr.GetAt(i);
			if(pMenuItem->uID == uItemID)
			{
				m_MenuItemArr.RemoveAt(i);
				break;
			}
		}
	}
	else							//子菜单
	{
	}
	return CMenu::RemoveMenu(nPosition,nFlags);
}

BOOL CMenuEx::AppendMenuEx(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	MENUITEM *pMenuItem = new MENUITEM;
	pMenuItem->strText = lpszNewItem;
	pMenuItem->uID = nIDNewItem;
	pMenuItem->uIndex = -1;
	pMenuItem->uPositionImageLeft = -1;
/*
	UINT uNewFlags = 0;
	if(nFlags & MF_CHECKED) uNewFlags |= MF_CHECKED;
	if(nFlags & MF_UNCHECKED) uNewFlags |= MF_UNCHECKED;
	if(nFlags & MF_SEPARATOR) uNewFlags |= MF_SEPARATOR;
	uNewFlags |= MF_OWNERDRAW;*/

	nFlags &= ~MF_STRING;
	nFlags |= MF_OWNERDRAW;

	m_MenuItemArr.Add(pMenuItem);
	return CMenu::AppendMenu(nFlags,nIDNewItem,(LPCTSTR)pMenuItem);
}

BOOL CMenuEx::LoadMenu(UINT uMenu)
{
	//重新读入菜单,创建为popup菜单,才能自画(由框架调用MesureItem() 和 DrawItem()
	HMENU hMenu = ::CreateMenu();
	this->Attach(hMenu);

	CMenu Menu;		//临时菜单
	UINT uID;
	Menu.LoadMenu(uMenu);
	for(int i = 0; i < (int)Menu.GetMenuItemCount(); i++)
	{
		uID = Menu.GetMenuItemID(i);
		if(uID == 0)			//分隔符
		{
			::AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
		}
		else if((int)uID == -1)		//弹出菜单(即子菜单)
		{
			CMenu *pSubMenu = Menu.GetSubMenu(i);
			
			//创建子菜单	
			HMENU hSubMenu = ::CreatePopupMenu();
			CString strPopup;
			Menu.GetMenuString(i,strPopup,MF_BYPOSITION);
			::InsertMenu(hMenu,i,MF_BYPOSITION | MF_POPUP | MF_STRING,(UINT)hSubMenu,strPopup);						
			
			//对子菜单递归调用ChangeMenuStyle(),把子菜单改为MF_OWNERDRAW风格
			ChangeMenuStyle(pSubMenu,hSubMenu);
		}
		else					//正常的菜单项
		{
			CString strText;
			Menu.GetMenuString(uID,strText,MF_BYCOMMAND);
			AppendMenu(MF_STRING,uID,strText);
		}
	}
	Menu.DestroyMenu();			//销毁临时菜单
	return TRUE;
}

void CMenuEx::ChangeMenuStyle(CMenu *pMenu,HMENU hNewMenu)
{	
	//关联为CMenuEx(关联为CMenuEx后才能自动重画
	//原因不明(CMenu封装的结果?)

	CMenuEx *pNewMenu;
	pNewMenu = new CMenuEx;
	pNewMenu->Attach(hNewMenu);
	m_SubMenuArr.Add(pNewMenu);


	UINT uID;
	int nItemCount = pMenu->GetMenuItemCount();
	for(int i = 0; i < nItemCount; i++)
	{
		uID = pMenu->GetMenuItemID(i);
		if(uID == 0)			//分隔符
		{
			::AppendMenu(hNewMenu,MF_SEPARATOR,0,NULL);
			//pNewMenu->AppendMenu(MF_SEPARATOR,0,NULL);
			CString strText;
			MENUITEM *pMenuItem = new MENUITEM;
			pMenuItem->uID = 0;
			pMenuItem->uIndex = -1;
			pMenuItem->uPositionImageLeft = -1;
			pMenuItem->pImageList = &m_ImageList;
			pMenuItem->hIcons = &m_hicons;
			m_MenuItemArr.Add(pMenuItem);
			
			::ModifyMenu(hNewMenu,i,MF_BYPOSITION | MF_OWNERDRAW,-1,(LPCTSTR)pMenuItem);
			//pNewMenu->AppendMenuEx(MF_SEPARATOR,0,NULL);
		}
		else if(uID == -1)		//弹出菜单(即子菜单)
		{
			CMenu *pSubMenu = pMenu->GetSubMenu(i);
			HMENU hPopMenu = ::CreatePopupMenu();
			CString strPopup;
			pMenu->GetMenuString(i,strPopup,MF_BYPOSITION);
			::InsertMenu(hNewMenu,i,MF_BYPOSITION | MF_POPUP,(UINT)hPopMenu,strPopup);

			MENUITEM *pMenuItem = new MENUITEM;
			pMenuItem->uID = -1;
			pMenuItem->strText = strPopup;
			pMenuItem->uIndex = -1;
			pMenuItem->uPositionImageLeft = -1;
			pMenuItem->pImageList = &m_ImageList;
			pMenuItem->hIcons = &m_hicons;
			m_MenuItemArr.Add(pMenuItem);
			::ModifyMenu(hNewMenu,i,MF_BYPOSITION | MF_OWNERDRAW,-1,(LPCTSTR)pMenuItem);
			
			//pNewMenu->ModifyMenuEx(i,MF_BYPOSITION,-1,strPopup);

			
			ChangeMenuStyle(pSubMenu,hPopMenu);
			
		}
		else					//正常的菜单项
		{
			CString strText;
			pMenu->GetMenuString(uID,strText,MF_BYCOMMAND);
			MENUITEM *pMenuItem = new MENUITEM;
			pMenuItem->uID = pMenu->GetMenuItemID(i);
			pMenu->GetMenuString(pMenuItem->uID,pMenuItem->strText,MF_BYCOMMAND);
			pMenuItem->uIndex = -1;
			pMenuItem->uPositionImageLeft = -1;
			pMenuItem->pImageList = &m_ImageList;
			pMenuItem->hIcons = &m_hicons;
			//::AppendMenu(hNewMenu,MF_STRING,uID,(LPCTSTR)pMenuItem->strText);
			m_MenuItemArr.Add(pMenuItem);

			UINT uState = pMenu->GetMenuState(i,MF_BYPOSITION);
			::AppendMenu(hNewMenu,MF_OWNERDRAW | MF_BYCOMMAND | uState,uID,(LPCTSTR)pMenuItem);

			//pNewMenu->AppendMenuEx(MF_STRING,uID,strText);
		}
	}
}

BOOL CMenuEx::ModifyMenuEx(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	UINT uItemID = 0;
	if(nFlags & MF_BYPOSITION)
	{
		uItemID = GetMenuItemID((int)nPosition);
	}
	else
	{
		uItemID = nPosition;
	}
	//普通菜单项和弹出式菜单
	int i;
	MENUITEM *pMenuItem = NULL;
	for( i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		pMenuItem = m_MenuItemArr.GetAt(i);
		if(pMenuItem->uID == uItemID)
		{
			pMenuItem->strText = lpszNewItem;
			if((int)nIDNewItem != 0)
				pMenuItem->uID = nIDNewItem;
			break;
		}
	}
	//没找到对应的菜单项
	if(i >= m_MenuItemArr.GetSize())
	{
		MENUITEM *pMenuItem = new MENUITEM;
		pMenuItem->strText = lpszNewItem;
		pMenuItem->uID = nIDNewItem;
		pMenuItem->pImageList = &m_ImageList;
			pMenuItem->hIcons = &m_hicons;
		m_MenuItemArr.Add(pMenuItem);
	}
	
/*
	UINT uNewFlags = 0;
	if(nFlags & MF_CHECKED) uNewFlags |= MF_CHECKED;
	if(nFlags & MF_UNCHECKED) uNewFlags |= MF_UNCHECKED;
	if(nFlags & MF_BYPOSITION) uNewFlags |= MF_BYPOSITION;
	if(nFlags & MF_BYCOMMAND) uNewFlags |= MF_BYCOMMAND;
	uNewFlags |= MF_OWNERDRAW;*/

	nFlags &= ~MF_STRING;
	nFlags |= MF_OWNERDRAW;
	
	return CMenu::ModifyMenu(nPosition,nFlags,nIDNewItem,(LPCTSTR)pMenuItem);
}

void CMenuEx::CalculateLeftImagePos(CMenu *pPopupMenu)
{
	int MenuItemCount = pPopupMenu->GetMenuItemCount();

	for(int i = MenuItemCount - 1 ; i >= 0; i--)
	{
		int nID = pPopupMenu->GetMenuItemID(i);
		CString strText;
		pPopupMenu->GetMenuString(i,strText,MF_BYPOSITION);
		for(int j = 0; j < m_MenuItemArr.GetSize(); j++)
		{
			MENUITEM *pMenuItem = m_MenuItemArr.GetAt(j);
			if((int)pMenuItem->uID == nID)
			{
				pMenuItem->uPositionImageLeft = MenuItemCount - 1 - i;
			}
		}
	}
}

BOOL CMenuEx::GetMenuItem( UINT nID , MENUITEM * item )
{
	MENUITEM *pMenuItem = NULL;
	int i;
	for(i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		pMenuItem = m_MenuItemArr.GetAt(i);
		if(pMenuItem->uID == nID)
		{
			break;
		}
	}
	if( i < m_MenuItemArr.GetSize() )
	{
		item->strText = pMenuItem->strText;
		item->uID = pMenuItem->uID;
		item->uIndex = pMenuItem->uIndex;
		item->nSubMenuIndex = pMenuItem->nSubMenuIndex;
		item->uPositionImageLeft = pMenuItem->uPositionImageLeft;
		item->pImageList = pMenuItem->pImageList;
		pMenuItem->hIcons = &m_hicons;
		item->pLeftBitmap = pMenuItem->pLeftBitmap;
		return TRUE;
	}
	else
		return FALSE;

}
