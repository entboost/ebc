#pragma once
#include <atlimage.h>

typedef enum TRA_BTNSTATE{//按钮状态
	TRA_BTN_NOR,
	TRA_BTN_HOT,
	TRA_BTN_PRE,	// 按下或选择
	TRA_BTN_DIS
};

typedef struct _TRATRAPNGINFO_
{
	int nWidth;
	int nHeight;
	CImage*  pImg;
}TRAPNGINFO;


class CTraButton : public CButton
{
	// 标准构造函数
public:
	CTraButton();
	virtual ~CTraButton();

public:
	//作用：载入背景图
	void Load(UINT IDBkGroup, int width=-1, int height=0, const CString& resourceType = _T("PNG"));
	void Load(const CString& sFileName, int width=-1, int height=0, const CString& resourceType = _T("PNG"));
	//作用：自适应背景图
	void SetAutoSize(bool bAutoSize);

	void MovePoint(int x, int y, int nWidth=0, int nHeight=0);
	void SetChecked(bool b);
	bool GetChecked(void) const;

	int GetImgWidth(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetWidth();}
	int GetImgHeight(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetHeight();}
	void SetToolTipText(LPCTSTR s);
	void SetToolTipColor(COLORREF crText, COLORREF crBkgnd);
	void SetAutoFocus(bool v) {m_bAutoFocus = v;}
	void SetTextLeft(int v) {m_nTextLeft = v;}
	int GetTextLeft(void) const {return m_nTextLeft;}
	void SetTextTop(int v) {m_nTextTop = v;}

	void SetNorTextColor(COLORREF v) {m_crNorText = v;}
	void SetHotTextColor(COLORREF v) {m_crHotText = v;}
	void SetPreTextColor(COLORREF v) {m_crPreText = v;}
	//void SetCheckedTextColor(COLORREF v) {m_crCheckedText = v;}
	void SetDrawPanel(bool bDrawPanel, COLORREF crNor, COLORREF crHot=-1, COLORREF crPre=-1, COLORREF crDis=-1);	// default -1=crNor
	void SetDrawPanelRgn(bool bCreateRgn = true) {m_bCreateRgn = bCreateRgn;}	// default true
	void SetDrawClosePic(bool bDrawClose, COLORREF crNor, COLORREF crHot, COLORREF crPre=-1, COLORREF crDis=-1, int nWidth=2);
	// nButtonType 0:disable 1:最小化 2:最大化 3:还原 4:关闭
	// 10:圆点
	void SetDrawToolButtonPic(int nButtonType, COLORREF crNor, COLORREF crHot, COLORREF crPre=-1, COLORREF crDis=-1, int nWidth1=2, int nWidth2=8);
	void SetDrawNewPic(bool bDrawNew, COLORREF crNor, COLORREF crHot, COLORREF crPre=-1, COLORREF crDis=-1, int nWidth=2, int nHeight=8);
	// nTriangleType: 0=disable, 1=up; 2=down; 3=left; 4=right
	void SetDrawTrianglePic(int nTriangleType, COLORREF crNor, COLORREF crHot, COLORREF crPre=-1, COLORREF crDis=-1, int nWidth=8,int nHeight=8);
	void SetDrawArrowPic(int nArrowType, bool bDrawLine, int nPenWidth, COLORREF crNor, COLORREF crHot, COLORREF crPre=-1, COLORREF crDis=-1, int nWidth=5);
	// nLineType: 0=disable 1=up; 2=down; 3=left; 4=right 5=all
	void SetDrawLine(int nLineType, int nLineWidth, int nLineHeight, COLORREF crNor, COLORREF crHot=-1, COLORREF crPre=-1, COLORREF crDis=-1, int nBgLineWeight=0, COLORREF crBgLineColor=0);
	void SetDrawLineNorExt(int nLineWidth, int nLineHeight);
	void SetDrawImage(bool bDestroyDelete, bool bGrayImage, Gdiplus::Image * pImage, const Gdiplus::Rect& rect);
	void SetDrawImage(bool bGrayImage);

	void SetTextHotMove(bool bTextHotMove) {m_bTextHotMove = bTextHotMove;}
	bool GetTextHotMove(void) const {return m_bTextHotMove;}

	void SetFormat(UINT v) {m_nFormat = v;}

	//void SetWndFocus(CWnd* v) {m_pWndFocus = v;}
	//CWnd*m_pWndFocus;

protected:
	void Load(CImage& orgImg, int width=-1, int height=0, const CString& resourceType = _T("PNG"));
	//重绘
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	//{{AFX_MSG(CTraButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

private:
	TRAPNGINFO		m_btninfoNor;				//Normal时的图片
	TRAPNGINFO		m_btninfoHot;				//Hot时的图片
	TRAPNGINFO		m_btninfoPre;				//Press时的图片
	TRAPNGINFO		m_btninfoDis;				//Disable时的图片
	bool			m_bHot;						//是否为Hot
	bool			m_bPress;					//是否按下
	bool			m_bSelected;				// 是否选择 add by hd 

	bool			m_bAutoSize;				//自动适应
	bool			m_bAutoFocus;
	int				m_nTextLeft;
	int				m_nTextTop;
	CToolTipCtrl m_tooltip;
	COLORREF		m_crNorText;
	COLORREF		m_crHotText;
	COLORREF		m_crPreText;
	//COLORREF		m_crCheckedText;
	bool			m_bDrawPanel;
	COLORREF		m_crNorPanel;
	COLORREF		m_crHotPanel;
	COLORREF		m_crPrePanel;
	COLORREF		m_crDisPanel;
	bool			m_bCreateRgn;
	bool			m_bDrawClosePic;
	COLORREF		m_crClosePicNor;
	COLORREF		m_crClosePicHot;
	COLORREF		m_crClosePicPre;
	COLORREF		m_crClosePicDis;
	int				m_nClosePicWidth;
	int				m_nDrawToolButtonPic;
	COLORREF		m_crToolButtonPicNor;
	COLORREF		m_crToolButtonPicHot;
	COLORREF		m_crToolButtonPicPre;
	COLORREF		m_crToolButtonPicDis;
	int				m_nToolButtonPicWidth1;
	int				m_nToolButtonPicWidth2;
	bool			m_bDrawNewPic;
	COLORREF		m_crNewPicNor;
	COLORREF		m_crNewPicHot;
	COLORREF		m_crNewPicPre;
	COLORREF		m_crNewPicDis;
	int				m_nNewPicWidth;
	int				m_nNewPicHeight;
	int				m_nDrawTriangleType;
	COLORREF		m_crTrianglePicNor;
	COLORREF		m_crTrianglePicHot;
	COLORREF		m_crTrianglePicPre;
	COLORREF		m_crTrianglePicDis;
	int				m_nTrianglePicWidth;
	int				m_nTrianglePicHeight;
	int				m_nDrawArrowType;
	int				m_nArrowPenWidth;
	bool			m_bDrawArrowLine;
	COLORREF		m_crArrowPicNor;
	COLORREF		m_crArrowPicHot;
	COLORREF		m_crArrowPicPre;
	COLORREF		m_crArrowPicDis;
	int				m_nArrowPicWidth;
	int				m_nDrawLineType;
	int				m_nLineWidth;
	int				m_nLineHeight;
	int				m_nLineNorWidth;
	int				m_nLineNorHeight;
	COLORREF		m_crLinePicNor;
	COLORREF		m_crLinePicHot;
	COLORREF		m_crLinePicPre;
	COLORREF		m_crLinePicDis;
	int					m_nBgLineWeight;
	COLORREF		m_crBgLineColor;

	bool			m_bDestroyDelete;
	bool			m_bGrayImage;
	Gdiplus::Image* m_pImage;
	Gdiplus::Rect	m_rcImage;
  UINT		m_nFormat;
	UINT m_nFrameCount;						// 图像多少帧
	UINT m_nCurrentFrame;					// 当前显示第几帧
	DWORD m_dwCurrentTime;					// 当前显示时间
	Gdiplus::PropertyItem* m_pFrameDelay;	// 每帧显示时间

	bool			m_bTextHotMove;				// 鼠标移动时，文字会上下移动；
	//CWnd*			m_pWndFocus;
private:
	//绘图
	void DrawBK(HDC dc, CImage* img,TRA_BTNSTATE btnstate);
	//写字
	void DrawBtnText(HDC dc, const CString& strText, int nMoveX, int nMoveY, TRA_BTNSTATE btnstate);
	//改变按钮风格
	virtual void PreSubclassWindow();
	LRESULT OnMsgReInvalidate(WPARAM wParam, LPARAM lParam);
};