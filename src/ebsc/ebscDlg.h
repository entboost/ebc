
// ebscDlg.h : header file
//

#pragma once
#include "DlgSelect.h"
#include "DlgTools.h"
#include "CDialogSK.h"

#define EBDCBASEDIALOG CDialogSK

// CebscDlg dialog
class CebscDlg : public EBDCBASEDIALOG
{
// Construction
public:
	CebscDlg(CWnd* pParent = NULL);	// standard constructor

	std::string m_sWindowText;
	int m_nMsgID;
	HWND m_hWindow;
	bool m_bMinWindow;
	HWND m_hMinWindow;

// Dialog Data
	enum { IDD = IDD_EBSC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	BOOL m_bMouseDown;
	BOOL m_bSelectFinished;
	Gdiplus::Point m_pSelectBegin;
	Gdiplus::Point m_pSelectEnd;
	Gdiplus::Point theLastDrawEndPoint;
	CDlgSelect m_pDlgSelect;
	CDlgTools m_pDlgTools;
	bool m_bShowBottom;
	CDlgToolExt m_pDlgToolExt;

	void Draw(void);
	void MoveTools(void);
	void UpdateToolExt(void);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnMessageToolextPoint(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSelectMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsFinish(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckColor(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckText(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckRectangle(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckCircle(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckArrow(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckCancel(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageToolsClieckSaveas(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
