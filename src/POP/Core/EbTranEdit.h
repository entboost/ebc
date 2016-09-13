#pragma once


// CEbTranEdit

class CEbTranEdit : public CEdit
{
	DECLARE_DYNAMIC(CEbTranEdit)

public:
	CEbTranEdit();
	virtual ~CEbTranEdit();

protected:
	//BOOL m_mousedown;

	void RedrawParent(void);
	DECLARE_MESSAGE_MAP()
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//afx_msg void OnEnChange();
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg UINT OnGetDlgCode();
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEnKillfocus();
};


