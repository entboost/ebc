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
	//作用：自适应背景图
	void SetAutoSize(bool bAutoSize);

	void MovePoint(int x, int y);
	void SetChecked(bool b);
	bool GetChecked() const;

	int GetImgWidth(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetWidth();}
	int GetImgHeight(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetHeight();}
	void SetToolTipText(LPCTSTR s);
	void SetToolTipColor(COLORREF crText, COLORREF crBkgnd);

protected:
	//重绘
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	//{{AFX_MSG(CTraButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
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
	CToolTipCtrl m_tooltip;

private:
	//绘图
	void DrawBK(HDC dc, CImage* img,TRA_BTNSTATE btnstate);
	//写字
	void DrawBtnText(HDC dc, const CString& strText, int nMove, TRA_BTNSTATE btnstate);
	//改变按钮风格
	virtual void PreSubclassWindow();
};