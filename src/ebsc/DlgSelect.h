#pragma once
#include "CDialogSK.h"
#include <vector>
#include "afxwin.h"
#include "EditTrans.h"


#define EBSC_WM_SELECT_MOVE		(WM_USER+0x201)
#define EBSC_WM_SELECT_FINISH	(WM_USER+0x202)

const int const_border_width = 4;

class CDrawInfo
{
public:
	enum DRAW_TYPE
	{
		DRAW_NULL
		, DRAW_TEXT				// 文字工具
		, DRAW_RECTANGLE		// 矩形工具
		, DRAW_CIRCLE			// 圆形工具
		, DRAW_ARROW			// 箭头工具

	};

	void Reset(void) {m_nDrawType=DRAW_NULL;}
	Gdiplus::Rect GetRect(void) const
	{
		CRect rect;
		rect.left = min(m_point1.X,m_point2.X);
		rect.top = min(m_point1.Y,m_point2.Y);
		rect.right = max(m_point1.X,m_point2.X);
		rect.bottom = max(m_point1.Y,m_point2.Y);
		return Gdiplus::Rect(rect.left,rect.top,rect.Width(),rect.Height());
	}
	Gdiplus::RectF GetRectF(void) const
	{
		CRect rect;
		rect.left = min(m_point1.X,m_point2.X);
		rect.top = min(m_point1.Y,m_point2.Y);
		rect.right = max(m_point1.X,m_point2.X);
		rect.bottom = max(m_point1.Y,m_point2.Y);
		return Gdiplus::RectF(rect.left,rect.top,rect.Width(),rect.Height());
	}
	DRAW_TYPE m_nDrawType;
	double m_fWidth;
	Gdiplus::Point m_point1;
	Gdiplus::Point m_point2;
	std::wstring m_sText;	// for DRAW_TEXT
	Gdiplus::Color m_color;

	CDrawInfo(void)
		: m_nDrawType(DRAW_NULL), m_fWidth(1.0)
	{
	}
	CDrawInfo(DRAW_TYPE nDrawType, double fWidth, const Gdiplus::Point& pos1, const Gdiplus::Point& pos2,const Gdiplus::Color& color)
		: m_nDrawType(nDrawType), m_fWidth(fWidth)
		, m_point1(pos1)
		, m_point2(pos2)
		//, m_rect(rect)
		, m_color(color)
	{
	}
	//CDrawInfo(DRAW_TYPE nDrawType,const Gdiplus::Rect& rect)
	//	: m_nDrawType(nDrawType)
	//	//, m_rect(rect)
	//{
	//}
	CDrawInfo(const CDrawInfo& pObj)
	{
		CDrawInfo::operator =(pObj);
	}
	const CDrawInfo& operator =(const CDrawInfo& pObj)
	{
		m_nDrawType = pObj.m_nDrawType;
		m_fWidth = pObj.m_fWidth;
		m_point1 = pObj.m_point1;
		m_point2 = pObj.m_point2;
		m_sText = pObj.m_sText;
		m_color = pObj.m_color;
		return *this;
	}
};

// CDlgSelect dialog

class CDlgSelect : public CDialogSK
{
	DECLARE_DYNAMIC(CDlgSelect)

public:
	CDlgSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelect();

	void Reset(void);
	CDrawInfo::DRAW_TYPE GetDrawType(void) const {return m_nDrawType;}
	void SetPointType(int nPointType);
	void SetToolsColor(void);
	void SetToolsText(bool bChecked);
	void SetToolsRectangle(bool bChecked);
	void SetToolsCircle(bool bChecked);
	void SetToolsArrow(bool bChecked);
	void SetToolsCancel(void);
	bool SetToolsSaveas(void);

// Dialog Data
	enum { IDD = IDD_DLG_SELECT };

protected:
	Gdiplus::Color m_colorTools;
	CDrawInfo::DRAW_TYPE m_nDrawType;
	double m_fDrawPointWidth;
	int m_nFontSize;
	std::vector<CDrawInfo> m_pDrawList;
	BOOL m_bMouseDown;
	Gdiplus::Point m_pSelectBegin;
	Gdiplus::Point m_pSelectEnd;
	CEditTrans m_editText;

	CSplitterControl m_wndSplitterLeft;
	CSplitterControl m_wndSplitterTop;
	CSplitterControl m_wndSplitterRight;
	CSplitterControl m_wndSplitterBottom;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void Draw(void);
	void Draw(Gdiplus::Graphics& graphics,const CDrawInfo& pDrawInfo);
	void AddDrawTextInfo(bool bReset);
	void SetEditFont(void);

	LRESULT OnMessageWindowResize(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeEditText();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
