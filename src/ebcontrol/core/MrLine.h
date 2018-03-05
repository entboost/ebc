// MrLine.h file here
#pragma once
#ifndef __MrLine_h__
#define __MrLine_h__

#include "MrItem.h"
#include <vector>

#define RECEIPT_FLAG_TRUE 0x1	// 对方已接收消息回执
#define RECEIPT_FLAG_SHOW 0x2	// 显示消息回执标识

class CMrLine
	: public CMrCallback

{
public:
	typedef boost::shared_ptr<CMrLine> pointer;

	static CMrLine::pointer create(HWND pParent,mr::bigint nMsgId)
	{
		return CMrLine::pointer(new CMrLine(pParent,nMsgId));
	}

	void SetClassicMode(bool v) {m_bClassicMode = v;}
	void SetSelectedBkgnd(const Gdiplus::Color& colorBkgnd) {m_brushSelected.SetColor(colorBkgnd);}
	void SetAlignment(MR_ALIGNMENT nAlignment) {m_nAlignment = nAlignment;}
	//MR_ALIGNMENT GetAlignment(void) const {return m_nAlignment;}
	void SetAlignmentValue(int nAlignment1, int nAlignment2) {m_nAlignment1=nAlignment1; m_nAlignment2=nAlignment2;}
	void AddItem(const CMrItem::pointer& pItem);
	CMrItem::pointer GetItem(size_t nIndex) const;
	void ClearItem(void);

	void SetFrameArc(int nArcOffset,int nPoloygonWidth=8,int nPoloygonHeight=18);
	void SetFrameBorderColor(const Gdiplus::Color& colorFrame=Gdiplus::Color::Gray);
	void SetFrameBackGroundColor(const Gdiplus::Color& colorBkgnd=Gdiplus::Color::LightGray);
	void SetRectF(const RectF& rect) {m_rect=rect;}
	const RectF& GetRectF(void) const {return m_rect;}

	void SetLineId(int nid) {m_nLineId = nid;}
	int GetNextItemId(void) {return ++m_nCurrentItemId;}
	int GetLineId(void) const {return m_nLineId;}
	void SetMsgId(mr::bigint nid) {m_nMsgId = nid;}
	mr::bigint GetMsgId(void) const {return m_nMsgId;}
	//bool GetSelected(void) const {return m_bSelected;}
	void SetShowReceiptFlag(bool v) {m_bShowReceiptFlag = v;}
	bool GetShowReceiptFlag(void) const {return m_bShowReceiptFlag;}
	bool SetReceiptFlag(int v);
	int GetReceiptFlag(void) const {return m_nReceiptFlag;}
	//void UpdateMsgText(int v);

	void SetFontSize(unsigned int nSize);
	bool GetSelected(void) const;
	std::wstring GetSelectString(void) const;
	bool PtInLine(const Point& pPos, const RectF& layoutRect);
	bool OnDblClick(Gdiplus::Graphics& graphics,const Point& pPos, const RectF& layoutRect,CMrItem::pointer* pOutClickItem=NULL);
	bool Onclick(const Point& pPos, const RectF& layoutRect,bool bCheckPosOnly, CMrItem::pointer* pOutClickItem=NULL);
	// -1:不在位置 0:在位置，不处理 1:在位置，同时处理
	int OnMouseMove(const Point& pPos, const RectF& layoutRect, CMrItem::pointer* pOutInPtItem=NULL);
	void ClearSelect(Gdiplus::Graphics& graphics, const RectF& layoutRect);
	bool InSelect(const Point& pPos,const RectF& layoutRect,CMrItem::pointer* pOutInPtItem=NULL) const;
	bool InImgSelect(const Point& pPos,const RectF& layoutRect,CMrItem::pointer& pOutImgItem) const;
	void SelectAll(Gdiplus::Graphics& graphics, const RectF& layoutRect);
	bool Select(Gdiplus::Graphics& graphics, const Point& pSelectStart, const Point& pSelectEnd, const RectF& layoutRect);
	void OnSize(int y, int cx, int cy);
	void OnDraw(Gdiplus::Graphics& graphics, const RectF& layoutRect);
	int OnDrawGif(Gdiplus::Graphics& graphics,const RectF& layoutRect,DWORD dwNowTime);

private:
	MR_ALIGNMENT OnGetAlignment(void) const {return m_bClassicMode?MR_ALIGNMENT_LEFT:m_nAlignment;}
	int OnGetArcOffset(void) const {return m_bClassicMode?0:m_nArcOffset;}
	int OnGetPoloygonWidth(void) const {return m_nPolygonWidth;}
	int OnGetAlignmentValue1(void) const {return m_nAlignment1;}
	int OnGetAlignmentValue2(void) const {return m_nAlignment2;}
	int OnGetLineAlignmentValue(void) const {return 1;}
	const RectF& OnGetLineItemRect(void) const {return m_pItemRect;}

public:
	CMrLine(HWND pParent,mr::bigint nMsgId);
	virtual ~CMrLine(void);

	std::vector<CMrItem::pointer> m_pList;
private:
	MR_ALIGNMENT m_nAlignment;
	int m_nAlignment1;					// 左对齐，左边间隔
	int m_nAlignment2;					// 右对齐，左边间隔
	Gdiplus::Pen m_penArc;				// 弧度边框
	Gdiplus::SolidBrush m_brushArc;		// 弧度背景色
	int m_nArcOffset;					// 弧度
	int m_nPolygonWidth;				// 三角宽度
	int m_nPolygonHeight;				// 三角高度
	bool m_bClassicMode;

	Gdiplus::SolidBrush m_brushSelected;

	int m_nLineId;
	int m_nCurrentItemId;
	bool m_bSelected;
	RectF m_pItemRect;	// 中间写字，区域
	RectF m_rect;		// 整体区域
	HWND m_pParent;
	mr::bigint m_nMsgId;
	bool m_bShowReceiptFlag;			// 内部配置 default true
	int m_nReceiptFlag;						// 1:对方已接收消息回执 2:显示消息回执标识
	Gdiplus::Pen m_penReceiptFlag;						// 消息回执边框
	Gdiplus::SolidBrush m_brushReceiptFlag;		// 消息回执背景色
};

#endif // __MrLine_h__
