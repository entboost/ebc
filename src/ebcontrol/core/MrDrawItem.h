// MrDrawItem.h file here
#pragma once
#ifndef __MrDrawItem_h__
#define __MrDrawItem_h__

#include <boost/shared_ptr.hpp>
#include <string>
#include "MrInfo.h"
using namespace Gdiplus;

bool PtInRect(const Point& pos, const RectF& rect);
CRect Rect2Rect(const RectF& rect);

class CMrDrawItem
{
public:
	typedef boost::shared_ptr<CMrDrawItem> pointer;
	CMrDrawItem(MR_ITEM_TYPE nItemType, HWND pParent=NULL);
	virtual ~CMrDrawItem(void);

	static CMrDrawItem::pointer create(MR_ITEM_TYPE nItemType, HWND pParent)
	{
		return CMrDrawItem::pointer(new CMrDrawItem(nItemType, pParent));
	}

	MR_ITEM_TYPE GetItemtype(void) const {return m_nItemType;}
	void SetRect(const RectF& rect) {m_rect = rect;}
	//void Move2Left(const RectF& rect, int maxwidth);
	void Move2Right(const RectF& rect, int maxwidth);
	const RectF& GetRect(void) const {return m_rect;}
	CRect GetCRect(void) const {return CRect(m_rect.GetLeft(),m_rect.GetTop(),m_rect.GetRight(),m_rect.GetBottom());}
	void SetText(const std::wstring& sText) {m_sText = sText;}
	const std::wstring& GetText(void) const {return m_sText;}
	void SetPath(const std::wstring& sPath) {m_sPath = sPath;}

	//bool DblClick(Graphics& pGraphics, const Point& pPoint);
	bool GetSelected(void) const;
	std::wstring GetSelectString(void) const;
	bool Onclick(const Point& pPos, const RectF& layoutRect);
	//bool OnDblclick(const Point& pPos, const RectF& layoutRect);
	bool OnMouseMove(const Point& pPos, const RectF& layoutRect);
	bool ClearSelect(void);	// 前面有选择，返回true
	void SelectAll(const RectF& layoutRect);
	bool SelectAll(const Point& pPos,const RectF& layoutRect);
	bool InSelect(const Point& pPos,const RectF& layoutRect) const;
	bool Select(Graphics& pGraphics, const Gdiplus::Font* pFont, const Point& pSelectStart, const Point& pSelectEnd, const RectF& layoutRect);
	bool Draw(Graphics& pGraphics, const Gdiplus::Font* pFont, const Brush* pBrush, const RectF& layoutRect);
	void InvalidateRect(const RectF& layoutRect);

private:
	MR_ITEM_TYPE m_nItemType;
	HWND m_pParent;
	RectF m_rectSelect;
	RectF m_rectOldSelect;
	int m_nSelectStart;
	int m_nSelectEnd;
	RectF m_rect;
	std::wstring m_sText;
	std::wstring m_sPath;
};

#endif // __MrDrawItem_h__
