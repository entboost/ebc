///////////////////////////////////////////////////////////////////////////
// VividTree.h : Definition of Class VividTree
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

#pragma once

//#include "resource.h"

#define WM_ITEM_SEL_CHANGED (WM_USER+0x230)		// HTREEITEM item = (HTREEITEM)wp; VividTree* pOwner = (VividTree*)lp;
#define WM_ITEM_DOUBLE_CLICK (WM_USER+0x231)	// HTREEITEM item = (HTREEITEM)wp; VividTree* pOwner = (VividTree*)lp;
#define WM_ITEM_TRACK_HOT (WM_USER+0x232)		// HTREEITEM item = (HTREEITEM)wp; VividTree* pOwner = (VividTree*)lp;
//#define WM_ITEM_END_EDIT (WM_USER+0x233)		// HTREEITEM item = (HTREEITEM)wp; VividTree* pOwner = (VividTree*)lp;

// Helper Routine
extern void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad );

class CTreeCallback
{
public:
	// nOutState
	// 0:离线 显示灰色
	// 1:正常
	virtual bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,Gdiplus::Image*& pImage3,int& pState) const {return false;}
	virtual bool GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pIcon1, HICON& pIcon2,int& pState) const {return false;}
	//virtual int GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pOutIcon) const {return 1;}
	virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,const CBitmap** pBitmapOpened,const CBitmap** pBitmapClosed) const {return false;}
	virtual COLORREF GetItemTextColor(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, COLORREF colorDefault) const {return colorDefault;}
};

class VividTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(VividTree)

public:
	VividTree();
	virtual ~VividTree();

	// add by hd
	void SetCallback(CTreeCallback* v) {m_hCallback = v;}
	void Sort(HTREEITEM hItem, PFNTVCOMPARE lpfnCompare);
	
	bool IsOnDeleteItem(HTREEITEM hItem) {return m_pOnDeleteItem==hItem?true:false;/*m_pOnDeleteList.exist(hItem);*/}
	void SetTreeOpenClosedBmp(UINT uOpenedId, UINT uClosedId);
	//void SetTopOffset(int icooffset, int txtoffset) {m_top_ico_offset = icooffset;m_top_txt_offset = txtoffset;}
	void SetIconSize(int w, int h) {m_ico_width=w; m_ico_height=h;}
	int GetIconWidth(void) const {return m_ico_width;}
	int GetIconHeight(void) const {return m_ico_height;}
	int GetHSize(void) const {return m_h_size;}
	HTREEITEM GetTrackItem(void) const {return m_hTrackItem;}
	HTREEITEM SelectHitTest(void);

// Attributes
	void SetBkGradients( COLORREF from, COLORREF to ){	m_gradient_bkgd_from = from; 
	                                                    m_gradient_bkgd_to = to; }
	COLORREF GetBkGradientFrom(void) const {	return m_gradient_bkgd_from; }
	COLORREF GetBkGradientTo(void) const { return m_gradient_bkgd_to; }
	void SetRectangle(COLORREF v) {m_crRectangle = v;}

	enum BkMode { BK_MODE_BMP = 0, BK_MODE_GRADIENT, BK_MODE_FILL };
	BkMode GetBkMode( ) { return m_bkgd_mode; }
	void SetBkMode( BkMode bkgd_mode ) { m_bkgd_mode = bkgd_mode; }

	bool GetBitmapTiledMode() { return m_bmp_tiled_mode;}
	void SetBitmapTiledMode( bool tiled ) {	m_bmp_tiled_mode = tiled; }
	void SetBitmapID( UINT id );

	void SetSelectedColor(COLORREF color) {m_colorSelected = color;}
	void SetTrackedColor(COLORREF color) {m_colorTracked = color;}
	bool GetGradientHorz() { return m_gradient_horz; }
	void SetGradientHorz( bool horz ) { m_gradient_horz = horz; } 
	void SetIntervalLine(bool bDrawIntervalLine, COLORREF color0, COLORREF color1);
	
	virtual void  SetRootIcon( HICON icon ) { m_icoRoot = icon; };	// Overridable
	virtual void  SetParentIcon( HICON icon ) { m_icoParent = icon; };	// Overridable
	virtual void  SetItemIcon( HICON icon ) { m_icon = icon; };		// Overridable
	virtual HICON GetItemIcon( HTREEITEM item, bool hadchild);					// Overridable

// Operations
	bool ItemIsVisible( HTREEITEM item );

private:
	CTreeCallback* m_hCallback;
	COLORREF m_gradient_bkgd_from;		// Gradient variables
	COLORREF m_gradient_bkgd_to;
	COLORREF m_gradient_bkgd_sel;
	bool     m_gradient_horz;			// horz or vertical gradient
	COLORREF m_crRectangle;

	BkMode  m_bkgd_mode;				// Current Background mode
    CBitmap m_bmp_back_ground;			// Background bitmap image
	CBitmap m_bmp_tree_closed;			// Tree open image (marker)
	CBitmap m_bmp_tree_open;			// Tree closed image (marker)
	bool    m_bmp_tiled_mode;			// Tile background image

	CRect m_rect;						// The client rect when drawing
	int   m_h_offset;					// 0... -x (scroll offset)
	int   m_h_size;						// width of unclipped window
	int   m_v_offset;					// 0... -y (scroll offset)
	int   m_v_size;						// height of unclipped window
	//int   m_top_txt_offset;					// add by hd
	//int   m_top_ico_offset;					// add by hd
	int	  m_ico_width;
	int   m_ico_height;
    HTREEITEM m_hTrackItem;
    HTREEITEM m_pOnDeleteItem;

	COLORREF m_colorSelected;
	COLORREF m_colorTracked;
	bool m_bDrawIntervalLine;
	COLORREF m_crIntervalLine0;
	COLORREF m_crIntervalLine1;

	HICON m_icoRoot;
	HICON m_icoParent;
	HICON m_icon;

	void DrawBackGround( CDC* pDC );	// Background Paint code
	void DrawItems( CDC* pDC );  // Item Paint Code

protected:
	//CLockMap<HTREEITEM,bool> m_pOnDeleteList;
	void UpdateTrackItem(void);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
};


