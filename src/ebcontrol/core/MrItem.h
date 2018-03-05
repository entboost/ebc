// MrItem.h file here
#pragma once
#ifndef __MrItem_h__
#define __MrItem_h__

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include "MrInfo.h"
#include "MrDrawItem.h"
using namespace Gdiplus;


class CMrItem
{
public:
	typedef boost::shared_ptr<CMrItem> pointer;
	static CMrItem::pointer create(int nItemId,MR_ITEM_TYPE nItemType=MR_ITEM_TYPE_TEXT,HWND pParent=NULL, int nFontSize=const_font_size)
	{
		return CMrItem::pointer(new CMrItem(nItemId,nItemType,pParent,nFontSize));
	}
	void SetCalllback(CMrCallback* pCallback) {m_pCallback = pCallback;}

	void SetPicInfo(int nPicType, RectF rect) {m_nPicType=nPicType; m_picRect = rect;}
	int GetItemId(void) const {return m_nItemId;}
	MR_ITEM_TYPE GetItemType(void) const {return m_nItemType;}
	void SetUrl(const std::wstring& sTitle, const std::wstring& sUrl) {m_sString=sTitle; m_sPath=sUrl;}
	const std::wstring& GetPath(void) const {return m_sPath;}
	void SetItemString(const std::wstring& sString) {m_sString=sString;}
	const std::wstring& GetItemString(void) const {return m_sString;}
	void SetItemParam(const std::wstring& sParam) {m_sParam=sParam;}
	const std::wstring& GetItemParam(void) const {return m_sParam;}
	void SetCtrlType(ULONG nParam) {m_nCtrlType=nParam;}
	ULONG  GetCtrlType(void) const {return m_nCtrlType;}
	void SetItemParam2(ULONG nParam) {m_nParam2=nParam;}
	ULONG  GetItemParam2(void) const {return m_nParam2;}
	void SetImage(Gdiplus::Bitmap * pImage, int nWidth, int nHeight);
	const Gdiplus::Bitmap * GetImage(void) const {return m_image;}
	void SetFont(const Gdiplus::Font& font);
	void SetBrush(const Gdiplus::Brush& brush);
	void SetPen(const Gdiplus::Pen& pen);

	void SetTime(time_t v) {m_tTime = v;}

	void SetRectF(const RectF& rect) {m_rect=rect;}
	const RectF& GetRectF(void) const {return m_rect;}
	const RectF& GetLastRect(void) const;
	//void Move2Left(int cx, int maxwidth);
	void Move2Left(const RectF& rect, int maxwidth);
	void Move2Right(int cx, int maxwidth);
	void Move2Right(const RectF& rect, int maxwidth);

	bool GetSelected(void) const;
	std::wstring GetSelectString(void) const;
	//bool OnDblClick(const Point& pPos, const RectF& layoutRect);
	bool Onclick(const Point& pPos, const RectF& layoutRect, bool bCheckPosOnly);
	bool OnDblClick(const Point& pPos, const RectF& layoutRect);
	// -1:不在位置 0:在位置，不处理 1:在位置，同时处理
	int OnMouseMove(const Point& pPos, const RectF& layoutRect);
	void ClearSelect(Gdiplus::Graphics& graphics,const RectF& layoutRect);
	void SelectAll(Graphics& pGraphics, const RectF& layoutRect);
	bool SelectAll(Graphics& pGraphics, const Point& pPos,const RectF& layoutRect);
	bool InSelect(const Point& pPos,const RectF& layoutRect) const;
	bool InImgSelect(const Point& pPos,const RectF& layoutRect) const;
	bool Select(Graphics& pGraphics, const Point& pSelectStart, const Point& pSelectEnd, const RectF& layoutRect);
	int OnSize(int y, int cx, int cy, const RectF& lastRect);
	void Draw(Graphics& pGraphics, const RectF& layoutRect);
	void DrawGif(Graphics& pGraphics, const RectF& layoutRect,DWORD dwNowTime);

public:
	CMrItem(int nItemId, MR_ITEM_TYPE nItemType,HWND pParent, int nFontSize);
	virtual ~CMrItem(void);

private:
	int m_nItemId;
	MR_ITEM_TYPE m_nItemType;
	std::wstring m_sString;		// text OR title
	ULONG m_nCtrlType;			// MR_ITEM_TYPE
	ULONG m_nParam2;
	std::wstring m_sParam;		// for click or dblclick
	std::wstring m_sPath;
	time_t m_tTime;
	int m_nPicType;		// for pic
	RectF m_picRect;	// for pic

	CMrCallback* m_pCallback;
	// ?
	HWND m_pParent;
	RectF m_rect;
	SizeF m_imgSize;
	std::vector<CMrDrawItem::pointer> m_pDrawLines;

	Gdiplus::Bitmap * m_image;
	int m_nImageWidth;
	int m_nImageHeight;
	UINT m_nFrameCount;						// 图像多少帧
	UINT m_nCurrentFrame;					// 当前显示第几帧
	DWORD m_dwCurrentTime;					// 当前显示时间
	Gdiplus::PropertyItem* m_pFrameDelay;	// 每帧显示时间
	Gdiplus::Font * m_font;
	Gdiplus::Brush * m_brush;
	Gdiplus::Pen * m_pen;
};
const CMrItem::pointer NullMrItem;

#endif // __MrItem_h__
