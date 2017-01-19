#pragma once

#define POP_WM_WINDOW_SIZE (WM_USER+11)

/*
 * 对话框窗口基类
 * 实现对话框窗口各种基本操作功能，比如鼠标拖拉窗口，显示边框等。
 * 基本所有的对话框窗口都继承于该类。
 * 
*//////////////////////////////////////////////////////

// CEbDialogBase dialog
class CEbBackDrawInfo
{
public:
	int m_nHeight;		// 高度
	float m_fLight;		// 亮度
	bool m_bGradient;	// 渐变
	bool m_bLeft2Rigth;
	int m_nWidth;		// 宽度
	COLORREF m_nColor;

	CEbBackDrawInfo(int nHeight,float fLight,bool bGradient, bool bLeft2Rigth=false, int nWidth=0, COLORREF color=-1)
		: m_nHeight(nHeight), m_fLight(fLight), m_bGradient(bGradient),m_bLeft2Rigth(bLeft2Rigth),m_nWidth(nWidth),m_nColor(color)
	{}
	CEbBackDrawInfo(void)
		: m_nHeight(0)
		, m_fLight(0.0)
		, m_bGradient(false)
		, m_bLeft2Rigth(false),m_nWidth(0), m_nColor(-1)
	{
	}
};

/*
 * 恩布对话框界面基础库
 * 实现一些登录基本功能，比如可拖拉，背景色等；大部分界面都继承于该类
 * 
*//////////////////////////////////////////////////////

class CEbDialogBase : public CDialog
{
	DECLARE_DYNAMIC(CEbDialogBase)

public:
	CEbDialogBase(CWnd* pParent = NULL);   // standard constructor
	CEbDialogBase(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEbDialogBase();

	enum TransparentType
	{
		TT_MSGBOX		= 0x1
		, TT_EDIT		= 0x2
		, TT_LISTBOX	= 0x4
		, TT_BTN		= 0x8
		, TT_DLG		= 0x10
		, TT_SCROLLBAR	= 0x20
		, TT_STATIC		= 0x40
		, TT_MAX		= 0x80
	};

	static void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad );
	static int GetScreenWidth(void);
	static int GetScreenHeight(void);
	static bool GetInLButtonDown(void);

	void SetTransparentType(UINT nTransType = TT_STATIC|TT_DLG) {m_nTransType = nTransType;}
	void SetBgColor(COLORREF colorBg);
	void SetMouseMove(BOOL bEnable) {m_bMouseMove = bEnable;}
	BOOL GetMouseMove(void) const {return m_bMouseMove;}
	void DisableMoveBottom(BOOL bDisable) {m_bDisabelMoveBottom = bDisable;}
	void SetCircle(bool bCircle = true);
	void SetSplitterBorder(int nLimitWidth = 200);
	void SetDlgChildFont(int nFontSize, LPCTSTR lpszFace,bool bSetDlgFont=true);

	void ClearBgDrawInfo(void) {m_pBgDrawInfo.clear();}
	void AddBgDrawInfo(const CEbBackDrawInfo& pDrawInfo) {m_pBgDrawInfo.push_back(pDrawInfo);}
	void DrawPopBg(CDC * pDC, COLORREF colorBg, int nBorder=2, int nOffsetValue=0, int nOffsetType=0, COLORREF colorBorder=-1);	// nOffsetType: 1=left,2=top,3=?
	void SetToolTipText(UINT nID,LPCTSTR sToolTip);
	//void SetToolTipColor(COLORREF crText,COLORREF crBkgnd);
	int GetSplitterPressed(void) const;	// left=1,right=2;top=3;bottom=4 0=not in press

	static BOOL __stdcall SetChildFont(HWND hwnd, LPARAM lparam)
	{
		::SendMessage(hwnd, WM_SETFONT, lparam, TRUE);
		return TRUE;
	}
protected:
	HICON m_hIcon;
	CFont m_childFont;
	UINT m_nTransType;
	CBrush m_brush;
	BOOL m_bMouseMove;
	BOOL m_bDisabelMoveBottom;
	int m_nLimitWidth;
	CSplitterControl m_wndSplitterLeft;
	CSplitterControl m_wndSplitterTop;
	CSplitterControl m_wndSplitterRight;
	CSplitterControl m_wndSplitterBottom;
	int m_nSplitterPressed;
	std::map<UINT,CString> m_pToolTip;
	std::vector<CEbBackDrawInfo> m_pBgDrawInfo;
	bool m_bCircle;

	//virtual void OnDraw(void) {}
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void init();

	LRESULT OnMessageWindowResize(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnTipNotify(UINT id,NMHDR*pNMHDR,LRESULT*pResult);
	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	//afx_msg void OnPaint();
};
